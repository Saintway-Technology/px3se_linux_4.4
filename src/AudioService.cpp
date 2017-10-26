#include "AudioService.h"
#include "AudioPlayer.h"
#include "MessageHandler.h"

#include <stdlib.h>

int main(int argc, char *argv[])
{
    log_info("Start audio service.");
    
    MessageHandler *messageHandler = new MessageHandler();
    
    // init gstreamer music player.
    AudioPlayer *player = AudioPlayer::getInstance();
    player->initPlayer(argc, argv);
    player->createThreadForBusMessage();
    player->setMessageHandler(messageHandler);
    
    control_message message;

    while (true) {
        if (messageHandler->recvReqFromClient(&message)) {
            switch (message.msg_type) {
            case REQ_TYPE_SET_MEDIA:
                player->setMedia(message.textValue);
                break;
            case REQ_TYPE_GET_MEDIA:
                player->sendMediaName();
                break;
            case REQ_TYPE_PLAY:
                player->play();
                break;
            case REQ_TYPE_PAUSE:
                player->pause();
                break;
            case REQ_TYPE_STOP:
                player->stop();
                break;
            case REQ_TYPE_GET_DURATION:
                player->sendDurationBack();
                break;
            case REQ_TYPE_GET_STATE:
                player->sendStateBack();
                break;
            case REQ_TYPE_SET_VOLUME:
                player->setVolume(message.intValue);
                break;
            case REQ_TYPE_GET_VOLUME:
                player->sendVolumeBack();
                break;
            case REQ_TYPE_SET_POSITION:
                player->setPosition(message.intValue);
                break;
            case REQ_TYPE_GET_POSITION:
                player->sendPositionBack();
                break;
            case REQ_TYPE_GET_MEDIA_TITLE:
                player->sendTitleBack();
                break;
            case REQ_TYPE_GET_MEDIA_ARTIST:
                player->sendArtistBack();
                break;
            case REQ_TYPE_PLAY_MODE_CHANGED:
                player->changePlayMode(message.intValue);
                break;
            case REQ_TYPE_CONNECT_STATE_CHANGE: {
                bool isClientConneted = message.intValue==0 ? false : true;
                messageHandler->onClientConnectStateChanged(isClientConneted);
                player->onClientConnectStateChanged(isClientConneted);
                break;
            }
            default:
                break;
            }
        }
    }

    return 0;
}

