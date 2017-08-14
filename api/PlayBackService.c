
#include "PlayBackService.h"

char songList[MTEXT_SIZE][MTEXT_SIZE];
char fileList[MTEXT_SIZE][MTEXT_SIZE];
char arti_msg[MTEXT_SIZE];
char titl_msg[MTEXT_SIZE];
char albu_msg[MTEXT_SIZE];
char pos[MTEXT_SIZE];
char dur[MTEXT_SIZE];
int musicNum;

void send_msg(int msg_num)
{
	struct msgform msg;
	int msgid,*pint;
	msgid=msgget(MSGKEY,0660);
	pint=(int *)msg.mtext;
	*pint=msg_num;
	msg.mtype=1;
	msgsnd(msgid,&msg,sizeof(int),IPC_NOWAIT);
}

int msg_id_get(void)
{
	int msgid;
	msgid=msgget(MSGKEY_LIST,0660|IPC_CREAT);
	if(msgid == -1){
		printf(" get msgid fail\n");
	}
	return msgid;
}

/* void stop(void)
{
	send_msg(RK_STOP);
} */

void r_pause(void)
{
	send_msg(RK_PAUSE);
}

void r_play(void)
{
	send_msg(RK_PLAY);
}

void r_pre(void)
{
	send_msg(RK_PRE);
}

void r_next(void)
{
	send_msg(RK_NEXT);
}

void r_volume(int vol){
	send_msg(vol+100);
}

void r_switch(int mis_num)
{
	send_msg(mis_num);
}

//秒
long duration(void)
{	
	struct msg_list msg;
	int msgid;
	char *end;
	msgid = msg_id_get();
	if(msgrcv(msgid,&msg,MTEXT_SIZE,107,IPC_NOWAIT) < 0){
		return -1;
	}else{
		strcpy(dur,msg.mtext);
		msgctl(msgid,IPC_RMID,0);
	}
	return strtol(dur,&end,10);
}

long position(void)
{
	struct msg_list msg;
	int msgid;
	char *end;
	msgid = msg_id_get();
	if(msgrcv(msgid,&msg,MTEXT_SIZE,106,IPC_NOWAIT) < 0){
		return -1;	
	}else{
		strcpy(pos,msg.mtext);
		msgctl(msgid,IPC_RMID,0);
	}
	return strtol(pos,&end,10); 
}

void seek(double percentage){
	if((int)percentage < 7)
		send_msg(7);
	if((int)percentage > 6){
		send_msg((int)percentage);
	}
}

char *getArtistName(void)
{
	struct msg_list msg;
	int msgid;
	msgid = msg_id_get();
	if(msgrcv(msgid,&msg,MTEXT_SIZE,105,IPC_NOWAIT) < 0){
		return NULL;
	}else{
		strcpy(arti_msg,msg.mtext);
		msgctl(msgid,IPC_RMID,0);
	}
	return arti_msg;
}

char *getSongName(void)
{
	struct msg_list msg;
	int msgid;
	msgid = msg_id_get();
	if(msgrcv(msgid,&msg,MTEXT_SIZE,104,IPC_NOWAIT) < 0){
		return NULL;
	}else{
		strcpy(titl_msg,msg.mtext);
		msgctl(msgid,IPC_RMID,0);
	}
	return titl_msg;
}

char *getAlbumName(void)
{
	struct msg_list msg;
	int msgid;	
	msgid = msg_id_get();
	if(msgrcv(msgid,&msg,MTEXT_SIZE,108,IPC_NOWAIT) < 0){
		return NULL;
	}else{
		strcpy(albu_msg,msg.mtext);
		msgctl(msgid,IPC_RMID,0);
	}
	return albu_msg;
}

void setRepeatMode(int repeatmode)
{
	if(repeatmode == RK_SINGLE_CYCLE )
		send_msg(RK_SINGLE_CYCLE);
	if(repeatmode == RK_LOOP_PLAY )
		send_msg(RK_LOOP_PLAY);
}

/* int getQueueAndCount(void)
{
	int count=0;
	struct msg_list msg;
	int msgid;
	msgid = msg_id_get();
	while(msgrcv(msgid,&msg,MTEXT_SIZE,100,IPC_NOWAIT)!= -1){
		strcpy(songList[count ++],msg.mtext);
	}
	msgctl(msgid,IPC_RMID,0);
	return count;
} */

int getQueueAndCount(void)
{
	int count = 0;
	int depth = 1;
	musicNum = 0;
	listAllFiles("/",depth);
	count = musicNum;
	return count;
} 

void openFile(char *path)
{
	int i;
	int depth = 1;
	char pathcom[MTEXT_SIZE];
	
	musicNum = 0;
	
	sprintf(pathcom,"/%s",path);
	listAllFiles("/",depth);
	for(i=0;i<musicNum;i++){
		if(strcmp(pathcom,fileList[i])==0){
			i=i+201;
			r_switch(i);
			break;
		}
		if(i == musicNum)
			printf("no such file !\n"); 
	}
}

void listAllFiles(const char *dirname, int depth)
{
	char path[MTEXT_SIZE];
	int fileSize;
	struct stat s;
	struct dirent *filename;
	DIR *dir;
	
	assert(dirname != NULL); 
	dir = opendir(dirname);
	
	if(dir == NULL)
	{
		printf("open dir %s error!\n",dirname);
		exit(1);
	}
	 
	while((filename = readdir(dir)) != NULL)
	{
		if(!strcmp(filename->d_name,".")||!strcmp(filename->d_name,"..")||!strcmp(filename->d_name,"proc"))
			continue;
	   
		fileSize = strlen(filename->d_name);  
		sprintf(path,"%s/%s",dirname,filename->d_name);
	  	  
		if(lstat(path,&s) >= 0 && S_ISDIR(s.st_mode) && depth <= 3)
		{
			listAllFiles(path,depth+1);
		}else{
			if(strcmp((filename->d_name+(fileSize - 4)),".mp3") == 0)
			{
				strcpy(fileList[musicNum++],path);
//				printf("%s\n",path);
			}
		}
	}
	closedir(dir);
}

