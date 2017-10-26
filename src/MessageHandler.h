#ifndef __MESSAGE_HANDLER_H__
#define __MESSAGE_HANDLER_H__

#include "message_queue_constant.h"

class MessageHandler
{
public:
    MessageHandler();

    void sendMessageToClient(long int msgType, long long value = 0);
    bool recvReqFromClient(control_message *message);
    void sendMessageBack(long int msgType, long long intValue = 0, const char *textValue = 0);

    void onClientConnectStateChanged(bool isConnected);

private:
    /* send_queueId is for message queue that send message into client
       and recv_queueId is for message queue that used for recv request from client*/
    int send_queueId;
    int recv_queueId;
    int m_feedbackQueueId;

    bool m_clientConnected;
    void initMessageQueue();
};

#endif // __MESSAGE_HANDLER_H__
