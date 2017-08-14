#ifndef __PLAYBACKSERVICE_H_
#define __PLAYBACKSERVICE_H_

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/resource.h>

#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>

#define RK_PLAY 1
#define RK_PAUSE 2
#define RK_SINGLE_CYCLE 3
#define RK_LOOP_PLAY 4
#define RK_NEXT 5
#define RK_PRE 6
//#define RK_STOP 7


#define MSGKEY 75
#define MSGKEY_LIST 85
#define MSGKEY_POS 95
#define MTEXT_SIZE 256

extern char songList[MTEXT_SIZE][MTEXT_SIZE];
extern char fileList[MTEXT_SIZE][MTEXT_SIZE];
extern char arti_msg[MTEXT_SIZE];
extern char titl_msg[MTEXT_SIZE];
extern char pos[MTEXT_SIZE];
extern char dur[MTEXT_SIZE];
extern char albu_msg[MTEXT_SIZE];
extern int musicNum;

struct msgform{
	long mtype;
	char mtext[MTEXT_SIZE];
};

struct msg_list{
	long mtype;
	char mtext[MTEXT_SIZE];
};

extern void send_msg(int msg_num);

//extern void stop(void);
extern void r_pause(void);
extern void r_play(void);
extern void r_pre(void);
extern void r_next(void);
extern void r_volume(int vol);
extern void r_switch(int mis_num);
extern long duration(void);
extern long position(void);
extern void seek(double percentage);
extern char *getArtistName(void);
extern char *getSongName(void);
extern char *getAlbumName(void);
extern void setRepeatMode(int repeatmode);
extern int getQueueAndCount(void);
extern void openFile(char *path);
extern void listAllFiles(const char *dirname,int depth);
#endif
