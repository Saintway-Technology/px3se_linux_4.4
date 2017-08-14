#ifndef __LIST_FILE_H_
#define __LIST_FILE_H_
#include "message_queue.h"

extern int count; 
extern char fileList[MTEXT_SIZE][MTEXT_SIZE];
extern void listAllFiles(const char *dirname,int depth);
#endif
