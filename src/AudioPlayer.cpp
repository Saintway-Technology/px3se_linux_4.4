#include "AudioPlayer.h"
#include "AudioService.h"
#include "MessageHandler.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h> 
#include <sys/resource.h>

#include <dirent.h>
#include <stdlib.h>
#include <errno.h>  
#include <sys/stat.h>
#include <string>

MessageHandler *m_messageHandler = NULL;
State m_currentState = StoppedState;
long long m_lastPosition = 0;
bool m_stop_position_send = false;
GstElement* m_playbin = NULL;
int m_duration = 0;
long long m_lastSendPosition = 0;
MediaStatus m_mediaStatus = NoMedia;

AudioPlayer* AudioPlayer::m_player = NULL;

gint sendCurrentPosition(gpointer data){
    bool *forceSendPosition = reinterpret_cast<bool*>(data);
    
    m_lastPosition = AudioPlayer::getInstance()->getPosition();
    /* Send potion to client every second */
    if(m_lastPosition - m_lastSendPosition > 1000 || forceSendPosition){
        m_messageHandler->sendMessageToClient(RCV_TYPE_POSITION_CHANGE,m_lastPosition);
        m_lastSendPosition = m_lastPosition;
    }
    
    return m_stop_position_send == true?0:1;
}

AudioPlayer::AudioPlayer():
    m_volume(-1),
    m_currentMediaPath(""),
    m_tagList(NULL),
    m_clientConnected(false)
{
    m_list = new MediaList();
    m_list->updateList();
}

AudioPlayer* AudioPlayer::getInstance(){
    if(m_player == NULL){
        m_player = new AudioPlayer();
    }
    return m_player;
}

bool AudioPlayer::initPlayer(int argc, char *argv[]){
    gst_init(&argc,&argv);
    m_playbin = gst_element_factory_make("playbin", NULL);
    if(!m_playbin){
        log_info("'playbin' gstreamer plugin missing\n");
        return false;
    }
    return true;
}

void AudioPlayer::changePlayMode(int playMode)
{
    m_list->setPlayMode(PlayMode(playMode));
}

void AudioPlayer::onClientConnectStateChanged(bool clientConneted)
{
    m_clientConnected = clientConneted;
    if(clientConneted){
        m_list->updateList();
        m_messageHandler->sendMessageToClient(RCV_TYPE_STATE_CHANGED, m_currentState);
        if(m_currentState == PlayingState || m_currentState == PausedState){
            m_messageHandler->sendMessageToClient(RCV_TYPE_POSITION_CHANGE, getPosition());
            m_messageHandler->sendMessageToClient(RCV_TYPE_META_DATA_AVAILABLE);
        }
    }
}

void AudioPlayer::setMedia(const char* filePath){
    log_info("AudioPlayer: set media path: %s",filePath);
    stop();
    
    m_mediaStatus = LoadingMedia;
    if(m_playbin && filePath != NULL){
        m_currentMediaPath = filePath;
        m_mediaArtist = "";
        m_mediaTitle = "";
        m_list->updateList();
        
        gchar *uri;
        if(gst_uri_is_valid(filePath)){
            uri = g_strdup(filePath);
        }else{
            uri = gst_filename_to_uri(filePath, NULL);
        }
        g_object_set(G_OBJECT(m_playbin), "uri", uri, NULL);
    }
}

bool AudioPlayer::play(){
    log_info("AudioPlayer: play media: %s",m_currentMediaPath.c_str());
    m_list->onPlayItemChanged(m_currentMediaPath);
    
    if(m_mediaStatus == NoMedia){
        return false;
    }
    
    if(m_playbin){
        if(gst_element_set_state(m_playbin, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE){
            return false;
        }else{
            return true;
        }
    }
}

bool AudioPlayer::pause(){
    log_info("AudioPlayer: pause media: %s",m_currentMediaPath.c_str());
    
    if(m_mediaStatus == NoMedia){
        return false;
    }
    
    if(m_playbin){
        if(gst_element_set_state(m_playbin, GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE){
            return false;
        }else{
            return true;
        }
    }
}

void AudioPlayer::stop(){
    log_info("AudioPlayer: stop media");
    m_currentState = StoppedState;
    if(m_playbin){
        gst_element_set_state(m_playbin, GST_STATE_NULL);
        m_currentMediaPath = "";
        m_mediaStatus = NoMedia;
    }
    // Send a zero position to client for update slider.
    m_messageHandler->sendMessageToClient(RCV_TYPE_POSITION_CHANGE,0);
}

void AudioPlayer::nextSong(){
    setMedia(m_list->getNextSongPath().c_str());
    play();
}

void AudioPlayer::setMessageHandler(MessageHandler *handler){
    m_messageHandler = handler;
}

long long AudioPlayer::getDuration(){
    long long gstDuration = 0;
    int duration = -1;
    
    if (m_playbin && gst_element_query_duration(m_playbin, GST_FORMAT_TIME, &gstDuration))
        duration = gstDuration / 1000000;
    
    if (m_duration != duration) {
        m_duration = duration;
    }
    
    return duration;
}

void AudioPlayer::sendDurationBack(){
    long long duration = getDuration();
    //log_info("AudioPlayer: send state with value %lld",duration);
    m_messageHandler->sendMessageBack(REQ_TYPE_GET_DURATION, duration);
}

void AudioPlayer::sendStateBack(){
    //log_info("AudioPlayer: send state with value %d",m_currentState);
    m_messageHandler->sendMessageBack(REQ_TYPE_GET_STATE, m_currentState);
}

void AudioPlayer::setVolume(int volume){
    log_info("AudioPlayer: set volume with value %d",volume);
    if(m_volume != volume){
        m_volume = volume;
        g_object_set(G_OBJECT(m_playbin), "volume", m_volume / 100.0, NULL);
    }
}

void AudioPlayer::sendVolumeBack(){
    log_info("AudioPlayer: send Volume with value %d",m_volume);
    m_messageHandler->sendMessageBack(REQ_TYPE_GET_VOLUME, m_volume);
}

void AudioPlayer::setPosition(long long ms){
    //seek locks when the video output sink is changing and pad is blocked
    if(m_playbin && m_currentState != StoppedState){
        ms = ms > 0 ? ms : 0;
        long long position = ms * 1000000;
        
        MediaStatus preMediaStatus = m_mediaStatus;
        m_mediaStatus = BufferingMedia;
        
        bool isSeeking = gst_element_seek(m_playbin,
                                          1.0,
                                          GST_FORMAT_TIME,
                                          GstSeekFlags(GST_SEEK_FLAG_FLUSH),
                                          GST_SEEK_TYPE_SET,
                                          position,
                                          GST_SEEK_TYPE_NONE,
                                          0);
        if (isSeeking){
            m_lastPosition = ms;
            sendCurrentPosition((bool*)true);
        }else{
            m_mediaStatus = preMediaStatus;
        }
    }
}

long long AudioPlayer::getPosition(){
    gint64 position = 0;
    
    if (m_playbin && gst_element_query_position(m_playbin, GST_FORMAT_TIME, &position))
        m_lastPosition = position / 1000000;
    
    return m_lastPosition;
}

void AudioPlayer::sendPositionBack(){
    long long position = getPosition();
    //log_info("AudioPlayer: sendPosition with value %lld",position);
    m_messageHandler->sendMessageBack(REQ_TYPE_GET_POSITION, position);
}

void AudioPlayer::sendMediaName(){
    log_info("AudioPlayer: send media path with value %s",m_currentMediaPath.c_str());
    m_messageHandler->sendMessageBack(REQ_TYPE_GET_MEDIA,0,m_currentMediaPath.c_str());
}

void AudioPlayer::sendTitleBack(){
    log_info("AudioPlayer: send media title with value %s",m_mediaTitle.c_str());
    m_messageHandler->sendMessageBack(REQ_TYPE_GET_MEDIA_TITLE,0,m_mediaTitle.c_str());
}

void AudioPlayer::sendArtistBack(){
    log_info("AudioPlayer: send media artist with value %s",m_mediaArtist.c_str());
    m_messageHandler->sendMessageBack(REQ_TYPE_GET_MEDIA_ARTIST,0,m_mediaArtist.c_str());
}

void AudioPlayer::processEOS(){
    log_info("processEOS");
    m_mediaStatus = EndOfMedia;
    m_currentState = StoppedState;
    m_currentMediaPath = "";
    m_messageHandler->sendMessageToClient(RCV_TYPE_MEDIA_STATE_CHANGED,m_mediaStatus);
    m_messageHandler->sendMessageToClient(RCV_TYPE_POSITION_CHANGE,0);
    
    // Process next song path when the client not conneted in.
    if(!m_clientConnected){
        nextSong();
    }
}


void AudioPlayer::parseTagList(const GstTagList *tags){
    if(m_tagList == NULL || !gst_tag_list_is_equal(m_tagList,tags)){	
        gchar *str;
        
        if(gst_tag_list_get_string(tags,GST_TAG_ARTIST,&str)){
            m_mediaArtist = str;
        }
        if(gst_tag_list_get_string(tags,GST_TAG_TITLE,&str)){
            m_mediaTitle = str;
        }
        
        m_messageHandler->sendMessageToClient(RCV_TYPE_META_DATA_AVAILABLE);
        m_tagList = gst_tag_list_copy(tags);
    }
}

gboolean processBusMessage(GstBus *bus, GstMessage *msg, gpointer data){
    
    if(msg){
        //tag message comes from elements inside playbin, not from playbin itself
        if(GST_MESSAGE_TYPE(msg) == GST_MESSAGE_TAG){
            GstTagList *tag_list;
            gst_message_parse_tag(msg, &tag_list);
            if(!gst_tag_list_is_empty(tag_list)){
                AudioPlayer::getInstance()->parseTagList(tag_list);
            }
            gst_tag_list_free(tag_list);
        }else if(GST_MESSAGE_TYPE(msg) == GST_MESSAGE_DURATION) {
            log_info("GST_MESSAGE_DURATION");
            //updateDuration();
        }
        
        if(GST_MESSAGE_SRC(msg) == GST_OBJECT_CAST(m_playbin)){
            switch(GST_MESSAGE_TYPE(msg)){
            case GST_MESSAGE_STATE_CHANGED:{
                if(m_mediaStatus == BufferingMedia){
                    break;
                }
                
                GstState oldState;
                GstState newState;
                GstState pending;
                
                gst_message_parse_state_changed(msg, &oldState, &newState, &pending);
                
                switch(newState){
                case GST_STATE_VOID_PENDING:
                case GST_STATE_NULL:
                    // setSeekable(false);
                    if(m_currentState != StoppedState)
                        m_messageHandler->sendMessageToClient(RCV_TYPE_STATE_CHANGED,m_currentState = StoppedState);
                    m_mediaStatus = NoMedia;
                    break; 
                case GST_STATE_READY:
                    if(m_currentState != StoppedState)
                        m_messageHandler->sendMessageToClient(RCV_TYPE_STATE_CHANGED,m_currentState = StoppedState);
                    m_mediaStatus = LoadedMedia;
                    break;
                case GST_STATE_PAUSED:{
                    State prevState = m_currentState;
                    m_currentState = PausedState;
                    
                    // if(oldState == GST_STATE_READY)
                    //    updateDuration();
                    
                    if(m_currentState != prevState)
                        m_messageHandler->sendMessageToClient(RCV_TYPE_STATE_CHANGED,m_currentState);
                    m_mediaStatus = LoadedMedia;
                    break;
                }
                case GST_STATE_PLAYING:
                    if(m_currentState != PlayingState){
                        m_messageHandler->sendMessageToClient(RCV_TYPE_STATE_CHANGED,m_currentState = PlayingState);
                        // For rtsp streams duration information might not be available
                        // until playback starts.
                        // if (m_duration <= 0) {
                        //     m_durationQueries = 5;
                        //         updateDuration();
                        // }
                        sendCurrentPosition((bool*)true);
                        m_stop_position_send = false;
                        g_timeout_add(200,sendCurrentPosition,(bool*)false);
                    }
                    m_mediaStatus = LoadedMedia;
                    break;
                }	
                break;
            }
            case GST_MESSAGE_EOS:{
                AudioPlayer::getInstance()->processEOS();
                break;
            }
            case GST_MESSAGE_TAG:
            case GST_MESSAGE_STREAM_STATUS:
            case GST_MESSAGE_UNKNOWN:
                break;
            case GST_MESSAGE_ERROR:{
                log_info("GST_MESSAGE_ERROR");
                AudioPlayer::getInstance()->stop();
                GError *err;
                gchar *debug;
                gst_message_parse_error(msg, &err, &debug);
                if (err->domain == GST_STREAM_ERROR && err->code == GST_STREAM_ERROR_CODEC_NOT_FOUND)
                    m_messageHandler->sendMessageToClient(RCV_TYPE_ERROR, FormatError);
                else
                    m_messageHandler->sendMessageToClient(RCV_TYPE_ERROR, ResourceError);
                
                g_error_free(err);
                g_free(debug);
                
                // When error occur, play next song when no client connected in.
                if(!AudioPlayer::getInstance()->isClientConnected()){
                    AudioPlayer::getInstance()->nextSong();
                }
                break;
            }
            case GST_MESSAGE_SEGMENT_START:{
                log_info("GST_MESSAGE_SEGMENT_START");
                const GstStructure *structure = gst_message_get_structure(msg);
                gint64 position = g_value_get_int64(gst_structure_get_value(structure, "position"));
                position /= 1000000;
                m_lastPosition = position;
                
                m_messageHandler->sendMessageToClient(RCV_TYPE_POSITION_CHANGE,position);
                break;
            }
            case GST_MESSAGE_ASYNC_DONE:{
                log_info("GST_MESSAGE_ASYNC_DONE");
                m_mediaStatus = LoadedMedia;
                gint64  position = 0;
                if(gst_element_query_position(m_playbin, GST_FORMAT_TIME, &position)) {
                    position /= 1000000;
                    m_lastPosition = position;
                    m_messageHandler->sendMessageToClient(RCV_TYPE_POSITION_CHANGE,position);
                }
                break;
            }
            default:
                break;
            }
        }else if(GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR){
            log_info("GST_MESSAGE_ERROR: AccessDeniedError");
            AudioPlayer::getInstance()->stop();
            m_messageHandler->sendMessageToClient(RCV_TYPE_ERROR, AccessDeniedError);
            
            // When error occur, play next song when no client connected in.
            if(!AudioPlayer::getInstance()->isClientConnected()){
                AudioPlayer::getInstance()->nextSong();
            }	
        }
    }
    
    return true;	
}

gpointer beginBusMessageLoop(gpointer playbin){
    if(playbin){
        GMainLoop *loop = g_main_loop_new(NULL, false);
        GstBus *bus = gst_element_get_bus((GstElement*)playbin);
        gst_bus_add_watch_full(bus, G_PRIORITY_DEFAULT, processBusMessage, loop, NULL);
        g_main_loop_run(loop);
    }
}

void AudioPlayer::createThreadForBusMessage(){
    g_thread_new("busPrecess",beginBusMessageLoop,(gpointer)m_playbin);
}

