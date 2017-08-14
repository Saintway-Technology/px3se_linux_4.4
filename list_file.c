#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <assert.h>
#include "list_file.h"

int count = 0;
char fileList[MTEXT_SIZE][MTEXT_SIZE];

void listAllFiles(const char *dirname,int depth)
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
				strcpy(fileList[count++],path);
//				send_list_msg_block(filename->d_name,100);
//				printf("%s\n",path);
			}
		}
	}
	closedir(dir);
}





