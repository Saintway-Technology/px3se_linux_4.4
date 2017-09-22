#ifndef __AUDIO_PLAYER_H__
#define __AUDIO_PLAYER_H__

#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>
#include <assert.h>

#include "MessageHandler.h"
#include "message_queue_constant.h"
#include "MediaList.h"

class AudioPlayer{

public:
    static AudioPlayer* getInstance();
private:
    /* singleton pattern */
    AudioPlayer();
    bool m_clientConnected;
    static AudioPlayer* m_player;

    std::string m_currentMediaPath;
    int m_volume;
    GstTagList *m_tagList;
    std::string m_mediaArtist;
    std::string m_mediaTitle;
    std::string m_mediaLocation;

    MediaList *m_list;
    void updateState(State state);
public:
    /**
     * Gstremer audio playbin init.
     */
    bool initPlayer(int argc, char *argv[]);
    void setMessageHandler(MessageHandler *handler);
    void createThreadForBusMessage();
    void parseTagList(const GstTagList *tags);

    void setMedia(const char* filePath);
    std::string getMediaName();
    void sendMediaName();

    bool play();
    bool pause();
    void stop();
    void nextSong();

    void sendStateBack();

    long long getDuration();
    void sendDurationBack();

    void setVolume(int volume);
    void sendVolumeBack();

    void setPosition(long long position);
    long long getPosition();
    void sendPositionBack();

    void sendArtistBack();
    void sendTitleBack();

    void processEOS();
    void onClientConnectStateChanged(bool clientConnected);
    bool isClientConnected(){return m_clientConnected;}

    void changePlayMode(int playMode);
};
#endif // __AUDIO_PLAYER_H__
