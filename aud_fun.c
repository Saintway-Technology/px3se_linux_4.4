
#include "aud_fun.h"

GstElement *playbin;
char Art[M_SIZE];
char Tit[M_SIZE];
char Alb[M_SIZE];
char Unk[M_SIZE]="unknown";

int prepare_to_play(gchar* file_name){
	gchar *uri;
	char name_buf[512];
	
	playbin=gst_element_factory_make("playbin",NULL);
	
	if (!playbin) {
	  g_print ("'playbin' gstreamer plugin missing\n");
	  return -1;
    }
	
	sprintf(name_buf,"%s",file_name);
	if (gst_uri_is_valid (name_buf))
	   uri = g_strdup (name_buf);
    else
       uri = gst_filename_to_uri (name_buf, NULL);
   
    g_object_set (playbin, "uri", uri, NULL);
    g_free (uri);
	
	return 0;
}

void volume_ctl(double val){
	g_object_set(playbin,"volume",val,NULL);
}

void play_press(void){	
	gst_element_set_state(playbin, GST_STATE_PLAYING);	
}

void pause_press(void){
//	g_print("pause was pressed\n");
	gst_element_set_state(playbin, GST_STATE_PAUSED);
	
}

void stop_press(void)
{
	g_print("stop was pressed\n");
	if(playbin != NULL) {
       gst_element_set_state(playbin,GST_STATE_NULL);
	   gst_object_unref(playbin);
       g_print("deleting playbin,stop!!!\n");   
    }   	
}

int seek_to(gdouble percentage){
	GstFormat fmt =GST_FORMAT_TIME;
	gint64 length;
	
	if(playbin && gst_element_query_duration(playbin, fmt, &length)){
		gint64 target =((gdouble)length * (percentage / 100.0));
		g_print("Start seeking !!!\n");
		if(!gst_element_seek(playbin, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET, target, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)){
			g_warning("Failed to seek to desired position\n");
			return -1;
		}
	}
	return 0;	
}

void playbin_del(void){
	gst_element_set_state(playbin, GST_STATE_NULL);
	gst_object_unref(playbin);
}


char *time_change(gint64 time_nano)
{
	static char buf[50];
	gint32 sec_all,sec,min;
	
	sec_all=time_nano/1000000000;
	
	min = sec_all/60;
	sec = sec_all%60;
	sprintf(buf,"%02d:%02d",min,sec);
	
	return buf;
}

int pos_and_dur (void)
{
	gint64 pos, len;
	int pos_s,len_s,i;
	char pos_buf[50],len_buf[50],poslen_buf[100];
	char pos_len_buf[2][50];
	GstFormat format = GST_FORMAT_TIME;
	if (gst_element_query_position (playbin, format, &pos) && gst_element_query_duration (playbin, format, &len)) {	
//		g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r",GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
		sprintf(pos_buf,"%ld",pos/1000000000);
		sprintf(len_buf,"%ld",len/1000000000);
//  	sprintf(poslen_buf,"%s/%s",pos_buf,len_buf);
//		send_list_msg(poslen_buf,101);
//		printf("%s\r",poslen_buf);
		send_list_msg(pos_buf,106);
		send_list_msg(len_buf,107);
	}else{
		return -1;
	}
	return 0;
}


char *music_tag(GstMessage *message){
	 
	gchar *artist;
	gchar *title;
	gchar *album;
	GstTagList *newTags,*tags;
	newTags = NULL;
	tags = NULL;
	static char music_message[256]; 
	
	gst_message_parse_tag(message, &newTags);
	tags = gst_tag_list_merge(tags, newTags, GST_TAG_MERGE_PREPEND);
    gst_tag_list_free(newTags);
	if(!gst_tag_list_get_string(tags, GST_TAG_ARTIST, &artist))
	{
		printf("artist did not exist\n");
		strcpy(Art,Unk);
		strcpy(Tit,Unk);
		strcpy(Alb,Unk);
	}else{
		gst_tag_list_get_string(tags, GST_TAG_TITLE, &title);
		gst_tag_list_get_string(tags, GST_TAG_ALBUM, &album);
		strcpy(Art,artist);
		strcpy(Tit,title);
		strcpy(Alb,album);
	}	
	gst_tag_list_free(tags);
	
	sprintf(music_message,"%s--%s--%s\n",Art,Tit,Alb);
//	send_list_msg(music_message,103);
	send_list_msg(Tit,104);
	send_list_msg(Art,105);
	send_list_msg(Alb,108);
	return music_message;
}

void jump_to_play(void){
	send_msg(RK_PLAY);
}
 
 
 
 
 
 
 
 
 
 