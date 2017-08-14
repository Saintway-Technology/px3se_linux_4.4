#include "message_queue.h"

//全局变量
int msgid;

int msg_que_get(void)
{
	
	msgid=msgget(MSGKEY,0660|IPC_CREAT);
	
	if(msgid == -1){
		printf(" get msgid fail\n");
	}
	
	return msgid;
	
}	

void send_msg(int msg_num)
{
	struct msgform msg1;
	int *pint;
	msgid=msg_que_get();
	pint=(int *)msg1.mtext;
	*pint=msg_num;
	msg1.mtype=1;
	msgsnd(msgid,&msg1,sizeof(int),IPC_NOWAIT);
}

int msg_rcv_block(void)
{	
	int *pint,play_tag;
	
	msgrcv(msgid,&msg,MTEXT_SIZE,1,0);
	pint=(int*)msg.mtext;
	play_tag=*pint;
	
	return play_tag;
}

int msg_rcv_unblock(void)
{
	int *pint,play_tag;
	
	msgrcv(msgid,&msg,MTEXT_SIZE,1,IPC_NOWAIT);
	pint=(int*)msg.mtext;
	play_tag=*pint;
	
	return play_tag;
}

int msg_queue_del(void)
{
	msgctl(msgid,IPC_RMID,0);
	
	return 0;
}

int send_list_msg(char *music_msg,long rk_mtype)
{
	struct msg_list msglist;
	int msglistid, ret;
	
	msglistid = msgget(MSGKEY_LIST,IPC_EXCL);
	if(msglistid < 0){
		msglistid = msgget(MSGKEY_LIST,IPC_CREAT|0660);
		if(msglistid < 0){
			printf("Fail to creat msglistid\n");
			return -1;
		}
	}
	strcpy(msglist.mtext,music_msg);
	msglist.mtype = rk_mtype;
	ret = msgsnd(msglistid,&msglist,MTEXT_SIZE,IPC_NOWAIT);
	if(ret == -1){
//		printf("message send error !\n");
		return -1;
	}
	return 0;
}

int send_list_msg_block(char *list_msg,long rk_mtype)
{
	struct msg_list msglist1;
	int listid, ret;
	
	listid = msgget(MSGKEY_LIST,IPC_EXCL);
	if(listid < 0){
		listid = msgget(MSGKEY_LIST,IPC_CREAT|0660);
		if(listid < 0){
			printf("Fail to creat listid\n");
			return -1;
		}
	}
	strcpy(msglist1.mtext,list_msg);
	msglist1.mtype = rk_mtype;
	ret = msgsnd(listid,&msglist1,MTEXT_SIZE,IPC_NOWAIT);
	if(ret == -1){
//		printf("message send error !\n");
		return -1;
	}
	return 0;
}










