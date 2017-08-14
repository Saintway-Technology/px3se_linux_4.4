#include "PlayBackService.h"

int main(int argc ,char *argv[]){
	
	int song_num, i;
	
	if(argc != 2){
		printf("Usage: client path\n");
		return -1;
	}
	
	openFile(argv[1]);
	song_num = getQueueAndCount();
	for(i=0;i<song_num;i++){
		printf(" %d：%s\n",i+1,fileList[i]);
	}
	return 0;
}