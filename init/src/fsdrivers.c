#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_fsdriver.h>

void function_callback(unsigned int from, squire_fsdriver_message_t * msg){
	if(!strcmp(msg->id, "initramfs")){
		switch(msg->function){

			case FSDRIVER_FUNCTION_MOUNT:{
				printf("init_fsdrivers] mount '%s-%08x' at mp%d [%s]\r\n", msg->string0, msg->uint1, msg->uint0, msg->id);

				// Return could not mount
				msg->function = FSDRIVER_FUNCTION_MOUNT_R;
				msg->uint0 = VFS_RPC_RETURN_CANNOT_MOUNT;
				squire_message_simple_box_send(msg, sizeof(squire_fsdriver_message_t), from, VFS_FSDRIVER_BOX);				
			}break;

			default:
				break;
		}
	}
}

squire_fsdriver_t init_fsdrivers = {
	"init-fsdrivers",
	1,0,
	32,
	{
		{"initramfs", 0},
		{0}
	}
};
SQUIRE_FSDRIVER_INFO fsdriver_info = {
	&init_fsdrivers,
	function_callback
};
//FSDRIVER(fsdriver_info) // only for driver without main function
SQUIRE_FSDRIVER_INFO * _fsdriver_info = &fsdriver_info;

int init_fsdrivers_start(void * p){
	void * tar_start = p;
	char ** argv_a[3];
	char * argv0 = "init-fsdrivers"; argv_a[0] = argv0;
	char * argv1 = "1"; argv_a[1] = argv1;	// VFS_PID
	char * argv2 = "2"; argv_a[2] = argv2;	// VFS-FSDRIVER BOX
	atoi("0");
	return fsdriver_main(3, argv_a);
}
