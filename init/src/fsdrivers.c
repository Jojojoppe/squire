#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <squire.h>
#include <squire_fsdriver.h>

#include "tar.h"

void function_callback(unsigned int from, squire_fsdriver_message_t * msg){
	if(!strcmp(msg->id, "initramfs")){
		switch(msg->function){

			// MOUNT
			case FSDRIVER_FUNCTION_MOUNT:{
				printf("init_fsdrivers] mount '%s-%08x' at mp%d [%s]\r\n", msg->string0, msg->uint1, msg->uint0, msg->id);

				unsigned int mountpoint = msg->uint0;
				unsigned int devinstance = msg->uint1;

				msg->function = FSDRIVER_FUNCTION_MOUNT_R;
				msg->uint0 = VFS_RPC_RETURN_NOERR;
				msg->uint1 = mountpoint;
				msg->voidp0 = devinstance; // private_mount
				msg->voidp1 = 0; // private_root
				// msg->id = initramfs
				squire_message_simple_box_send(msg, sizeof(squire_fsdriver_message_t), from, VFS_FSDRIVER_BOX);				
			}break;

			// UNMOUNT
			case FSDRIVER_FUNCTION_UNMOUNT:{
				printf("init_fsdrivers] unmount mp%d\r\n", msg->uint0);

				unsigned int mountpoint = msg->uint0;

				msg->function = FSDRIVER_FUNCTION_UNMOUNT_R;
				msg->uint0 = VFS_RPC_RETURN_NOERR;
				msg->uint1 = mountpoint;
				// msg->id = initramfs
				squire_message_simple_box_send(msg, sizeof(squire_fsdriver_message_t), from, VFS_FSDRIVER_BOX);				
			} break;

			// OPEN
			case FSDRIVER_FUNCTION_OPEN:{
				printf("init_fsdrivers] open mp%d:%s%s [%08x]\r\n", msg->uint0, msg->string0, msg->string1, msg->uint1);

				unsigned int mountpoint = msg->uint0;
				char * path = msg->string0;
				char * fname = msg->string1;

				// Check if file exists
				if(!tar_exists(msg->voidp0, fname)){
					msg->function = FSDRIVER_FUNCTION_OPEN_R;
					msg->uint0 = VFS_RPC_RETURN_FILE_NOT_EXIST;
					msg->uint1 = mountpoint;
					//msg->voidp0 private_mount
					//msg->voidp1 private_root
					//msg->id = initramfs
					squire_message_simple_box_send(msg, sizeof(squire_fsdriver_message_t), from, VFS_FSDRIVER_BOX);		
					break;
				}

				size_t fsize;
				void * fdata = tar_get(msg->voidp0, fname, &fsize);

				msg->function = FSDRIVER_FUNCTION_OPEN_R;
				msg->uint0 = VFS_RPC_RETURN_NOERR;
				msg->uint1 = mountpoint;
				//msg->voidp0 private_mount
				//msg->voidp1 private_root
				//msg->id = initramfs
				msg->uint2 = fdata;	// File ID
				squire_message_simple_box_send(msg, sizeof(squire_fsdriver_message_t), from, VFS_FSDRIVER_BOX);				
			} break;

			// CLOSE
			case FSDRIVER_FUNCTION_CLOSE: {
				printf("init_fsdrivers] close %08x\r\n", msg->uint1);

				msg->function = FSDRIVER_FUNCTION_CLOSE_R;
				msg->uint0 = VFS_RPC_RETURN_NOERR;
				// msg->id = initramfs
				squire_message_simple_box_send(msg, sizeof(squire_fsdriver_message_t), from, VFS_FSDRIVER_BOX);		
			} break;

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
	char ** argv_a[3];
	char * argv0 = "init-fsdrivers"; argv_a[0] = argv0;
	char * argv1 = "1"; argv_a[1] = argv1;	// VFS_PID
	char * argv2 = "2"; argv_a[2] = argv2;	// VFS-FSDRIVER BOX
	atoi("0");
	return fsdriver_main(3, argv_a);
}
