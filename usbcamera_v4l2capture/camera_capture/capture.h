#ifndef _CAPTURE_H
#define _CAPTURE_H 

#define CLEAR(x) memset (&(x),0,sizeof(x))

void process_image(const void * p,unsigned int size);
int  read_video_frame (void);
void read_video_loop(void);
void start_video_capturing  (void);
void init_video_mmap(void );
void open_video_device (void);
void query_video_device (void);
void change_video_properties(void);
void set_video_format(void);
void stop_video_capturing(void);
void uninit_video_device(void);
void close_video_device (void);

#endif

