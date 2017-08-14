/*** block  from ../../../docs/manual/basics-helloworld.xml ***/
#include "list_file.h"
#include "aud_fun.h"
#include "message_queue.h"

typedef struct _CustomData {  
  gboolean terminate;    /* Should we terminate execution? */   
} CustomData;

int main(int argc, char *argv[])
{	
	CustomData data;
	GstBus *bus;
	GstMessage *message_tag;
	int play_tag,pause_ctl,rk_seek,rk_seek_plag,vol,get_tag,rk_single,rk_list,depth;
	int i = 0;
	int current_vol = 101;
	int print_flg = 0;
	int rk_power_on_flag = 0;//when power on,we need to get any msg from qt,then 0 become 1,and music start
	rk_single = 0;
	rk_list = 1;
	char music_title[256];
	
	while(1){
		count = 0;
		depth = 1;
		listAllFiles("/",depth);
		if(count == 0 && print_flg == 0){
			printf("there is no music file\n");
			print_flg = 1;
		}
		if(i == count || i > count){
			i = 0;
		}
		while (i<count){

			msg_que_get();
			
			g_print("NEW SONG BEGIN and THE count is %d\n",count);
			play_tag = 0;
			pause_ctl = 0; //this flag prevent pause failure
			rk_seek =0; //this flag remind us the type of seek
			rk_seek_plag = 0;

			get_tag = 0;
			
			gst_init(&argc, &argv);
			
			prepare_to_play(fileList[i]);
			i++;
			
			bus = gst_element_get_bus(playbin);
				
			data.terminate = FALSE;
			while(!data.terminate){
	/* 			if(rk_seek == 0){
					g_print("Coming in\n");
				} */
				if(current_vol != 101){
					volume_ctl((float)current_vol/10);
				}
				if(pause_ctl == 0 && rk_power_on_flag == 1)
					jump_to_play();
				
					play_tag = msg_rcv_unblock();
					
					/* PLAY */	
					while(play_tag == RK_PLAY){
						rk_power_on_flag = 1;
						play_press();
						pos_and_dur();
						play_tag = msg_rcv_unblock();
						message_tag = gst_bus_pop_filtered(bus, GST_MESSAGE_TAG|GST_MESSAGE_EOS|GST_MESSAGE_ERROR);
						if(message_tag != NULL){
							if(GST_MESSAGE_TYPE(message_tag) == GST_MESSAGE_TAG & get_tag == 0){
//								strcpy(music_title, music_tag(message_tag));
								printf("开始发送歌曲信息\n");
								music_tag(message_tag);
								gst_message_unref(message_tag);
								get_tag = 1;
//								printf("%s\n",music_title);
							}
							if(GST_MESSAGE_TYPE(message_tag) == GST_MESSAGE_ERROR){
								gchar *debug;
								GError *err;
								gst_message_parse_error (message_tag, &err, &debug);
								g_printerr ("Debugging info: %s\n", (debug) ? debug : "none");
								g_free (debug);
								g_print ("Error: %s\n", err->message);
								g_error_free (err);
								gst_message_unref(message_tag);
								g_object_unref (bus);
								playbin_del();
								data.terminate = TRUE; 
								count = 0;
								break;
							}
							
							
							if(GST_MESSAGE_TYPE(message_tag) == GST_MESSAGE_EOS){
								g_print("deleting playbin,stop!!!\n");
								gst_message_unref(message_tag);
								g_object_unref (bus);
								playbin_del();
								data.terminate = TRUE; 
								count = 0;
								break;
							}
						}else{
							send_list_msg(Tit,104);
							send_list_msg(Art,105);
							send_list_msg(Alb,108);
						}
					}			

					/* SEEK */
					while(play_tag > 6 && play_tag < 101){
						rk_power_on_flag = 1;
						rk_seek ++;
						rk_seek_plag = play_tag;
						play_tag = msg_rcv_unblock();
						if(rk_seek == 1){
							seek_to((gdouble)play_tag);    //play_tag from outside of while
						}
						if(play_tag < 7 || play_tag > 100){
							rk_seek = 0;
						}else{
							if(rk_seek_plag != play_tag){
								seek_to((gdouble)play_tag); //play_tag of inside
							}
						}
						pos_and_dur();
						
						if(gst_bus_pop_filtered (bus, GST_MESSAGE_EOS) != NULL){	
							g_print("deleting playbin,stop!!!\n");
							g_object_unref (bus);
							playbin_del();
							data.terminate = TRUE; 
							count = 0;
							break;
						}
					}
					/*single loop*/
					if(play_tag == RK_SINGLE_CYCLE){
						rk_power_on_flag = 1;
						printf("single loop\n");
						rk_single = 1;
						rk_list = 0;
						jump_to_play();
					}
					
					/*list loop*/
					if(play_tag == RK_LOOP_PLAY){
						rk_power_on_flag = 1;
						printf("list loop\n");
						rk_list = 1;
						rk_single = 0;
						jump_to_play();
					}
					
					/* NEXT */
					if(play_tag == RK_NEXT){
						rk_power_on_flag = 1;
						g_print("Playing next song !\n");
						g_object_unref (bus);
						playbin_del();
						if(rk_list == 0 && rk_single == 1)
							i++;
						data.terminate = TRUE; 
						count = 0;
						continue;
					}
					/*BEFORE*/
					if(play_tag == RK_BEFORE){
						rk_power_on_flag = 1;
						if(i<2){
							i = count-1;
						}else{
							i = i-2;
						}
						g_print("Playing the song before!\n");
						g_object_unref (bus);
						playbin_del();
						if(rk_list == 0 && rk_single == 1)
							i++;
						data.terminate = TRUE;
						count = 0;
						continue;	
					}
					/*SWITCH TO THE ith SONG*/
					if(play_tag  > 200){
						rk_power_on_flag = 1;
						i=play_tag - 201;
						g_print("switch to the song of %d\n",i+1);
						g_object_unref (bus);
						playbin_del();
						count = 0;
						data.terminate = TRUE; 
						continue;
					}
					
					/*SET VOLUME*/
					if(play_tag > 100 && play_tag < 201){
						rk_power_on_flag = 1;
						vol = play_tag - 101;
						volume_ctl((float)vol/10);
						current_vol = vol;
						jump_to_play();
					}
						
					/* PAUESE*/
					if(play_tag == RK_PAUSE){
						rk_power_on_flag = 1;
						pause_press();
						pause_ctl = 1;
					}
			}
			if(rk_list == 0 && rk_single == 1)
				i--;
			
			if(count == i){
				i = 0;
				continue;
			}
			msg_queue_del();
		}
	}
    return 0;
}

	 

























