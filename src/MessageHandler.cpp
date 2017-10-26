#include "MessageHandler.h"
#include "AudioService.h"

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

MessageHandler::MessageHandler()
{
    m_clientConnected = false;

    initMessageQueue();
}

void MessageHandler::initMessageQueue()
{
    // message queue for sent to client.
    send_queueId = msgget(RCV_QUEUE_KEY,MSG_QUEUE_FLAG | IPC_CREAT);
    log_info("message_queue for sent to client init.queueId: %d", send_queueId);
    if (send_queueId == -1)
        log_err("init message queue for sent to client with error %s", strerror(errno));

    // message queue for client request.
    recv_queueId = msgget(REQ_QUEUE_KET, MSG_QUEUE_FLAG | IPC_CREAT);
    log_info("message_queue for client request init.queueId: %d", recv_queueId);
    if (recv_queueId == -1)
        log_err("init message queue for client request with error %s", strerror(errno));

    // message queue for client request feedback
    m_feedbackQueueId = msgget(REQ_FEEDBACK_QUEUE_KEY, MSG_QUEUE_FLAG | IPC_CREAT);
    log_info("message_queue request feedback init.queueId: %d", m_feedbackQueueId);
    if (m_feedbackQueueId == -1)
        log_err("init message queue for request feedback with error %s", strerror(errno));
}

void MessageHandler::onClientConnectStateChanged(bool isConnected)
{
    m_clientConnected = isConnected;
}

void MessageHandler::sendMessageToClient(long int msgType, long long value)
{
    if (!m_clientConnected)
        return;

    if (send_queueId != -1) {
        struct state_message message;
        memset(&message, 0, sizeof(message));
        message.msg_type = msgType;
        message.value = value;
        //log_info("Send message with type: %ld, value:%lld",message.msg_type,message.value);

        if (msgsnd(send_queueId, (void*)&message, sizeof(long long), IPC_NOWAIT) == -1)
            log_err("Send message failed with message type: %ld", msgType);
    }
}

bool MessageHandler::recvReqFromClient(control_message *message)
{
    //memset(&message,0,sizeof(message));
    if (msgrcv(recv_queueId, (void*)message, MSG_BUFF_LEN + sizeof(long long), 0, 0) != -1)
        return true;

    return false;
}

void MessageHandler::sendMessageBack(long int msgType, long long intValue , const char *textValue)
{
    if (!m_clientConnected)
        return;

    if (m_feedbackQueueId != -1) {
        struct control_message message;
        memset(&message, 0, sizeof(message));
        message.msg_type = msgType;
        message.intValue = intValue;

        if (textValue != NULL)
            strcpy(message.textValue, textValue);
        else
            strcpy(message.textValue, "");

        if (msgsnd(m_feedbackQueueId, (void*)&message, MSG_BUFF_LEN + sizeof(long long), IPC_NOWAIT) == -1)
            log_info("send message back failed.");
    }
}
