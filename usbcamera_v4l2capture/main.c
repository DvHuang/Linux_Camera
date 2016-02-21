#include "capture.h"

//main function 
int main(int argc ,char ** argv)
{
	open_video_device();                    //open device 
	query_video_device();                    //query camera device
	change_video_properties();         //change camera properties 
	set_video_format();                //set video format
	init_video_mmap();
	start_video_capturing();                 //start capture video
	read_video_loop();                            //read frame
	stop_video_capturing();
	uninit_video_device(); 
	close_video_device();                   //close camera
	return 0;
}
