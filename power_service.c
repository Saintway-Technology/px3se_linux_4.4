
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <linux/input.h>


//#define DEV_PATH "/dev/input/event2"   //difference is possible
#define DEV_PATH "/dev/input/event0"   //difference is possible

#define KEY_POWER            116

static void* key_event_handler(void *arg)
{
	int keys_fd;
    struct input_event t;

	printf("[PowerManager] key_event_handler enter! \n");

	keys_fd=open(DEV_PATH, O_RDONLY);
    if(keys_fd <= 0)
    {
        printf("[PowerManager] open /dev/input/event2 device error!\n");
        return NULL;
    }
    while(1)
    {
        if(read(keys_fd, &t, sizeof(t)) == sizeof(t))
        {
                printf("[PowerManager] key_event_handler key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
                if( t.value==1 && t.code==KEY_POWER)
                {
                	printf("[PowerManager] key_event_handler key Power\n");
                    //system("echo mem > /sys/power/state");
                }
        }
    }
    close(keys_fd);

	printf("[PowerManager] key_event_handler exit unexpectedly ! \n");
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t key_event_thread;
	int ret = 0;


	pm_init();

	ret = pthread_create(&key_event_thread, NULL, key_event_handler, NULL);
	if (ret)
	{
		printf( "[PowerManager] create key_event_thread failed \n");
	}

    while(1)
    {
    	sleep(5);
		continue;
    }

 	printf("[PowerManager] exit unexpectedly ! \n");

	return 0;
}
