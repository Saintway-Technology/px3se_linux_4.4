#ifndef __AUD_FUN_H_
#define __AUD_FUN_H_

#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>

#define RK_PLAY 1
#define RK_PAUSE 2
#define RK_SINGLE_CYCLE 3
#define RK_LOOP_PLAY 4
#define RK_PLAY_IN_ORDER 0
#define RK_NEXT 5
#define RK_BEFORE 6
#define M_SIZE 256

extern GstElement *playbin;
extern char Art[M_SIZE];
extern char Tit[M_SIZE];
extern char Alb[M_SIZE];
char Unk[M_SIZE];

extern GstElement *playbin;
extern int prepare_to_play(gchar* file_name);
extern void pause_press(void);
extern void volume_ctl(double val);
extern void play_press(void);
extern void stop_press(void);
extern void playbin_del(void);
extern int seek_to(gdouble percentage);
extern char *time_change(gint64 time_nano);
extern int pos_and_dur (void);
extern char *music_tag(GstMessage *message);
extern void jump_to_play(void);

#endif