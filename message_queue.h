#ifndef __MESSAGE_QUEUE_H_
#define __MESSAGE_QUEUE_H_

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/resource.h>

#define MSGKEY 75
#define MSGKEY_LIST 85
#define MTEXT_SIZE 256

extern int msgid;

struct msg_list{
	long mtype;
	char mtext[MTEXT_SIZE];
};

struct msgform{
	long mtype;
	char mtext[MTEXT_SIZE];
}msg;

extern int msg_que_get(void);
extern void send_msg(int msg_num);
extern int msg_rcv_block(void);
extern int msg_rcv_unblock(void);
extern int msg_queue_del(void);
extern int send_list_msg(char *music_msg,long rk_mtype);
extern int send_list_msg_block(char *list_msg,long rk_mtype);
#endif