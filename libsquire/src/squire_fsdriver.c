#include <squire.h>
#include <squire_fsdriver.h>

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

extern SQUIRE_FSDRIVER_INFO * _fsdriver_info;

int fsdriver_main(int argc, char ** argv){

	// Get the paramters
	if(argc!=3){
		printf("FSDriver must be executed with 3 parameters: name VFS_PID VFS-FSDRIVER_box\r\n");
		return EXIT_FAILURE;
	}
	char * param_name = argv[0];
	unsigned int vfs_pid = atoi(argv[1]);
	unsigned int vfs_box = atoi(argv[2]);	

	// Register fs driver for filesystem types
	squire_fsdriver_message_t msg;
	msg.function = FSDRIVER_FUNCTION_REGFSDRIVER_R;
	strcpy(msg.string0, _fsdriver_info->fsdriver_info->name);
	msg.uint1 = _fsdriver_info->fsdriver_info->simple_box;
	msg.uint2 = squire_procthread_getpid();
	msg.uint3 = _fsdriver_info->fsdriver_info->version_major<<16 | _fsdriver_info->fsdriver_info->version_minor;
	squire_fsdriver_supported_t * supported = _fsdriver_info->fsdriver_info->supported;
	while(supported->id[0]){
		memcpy(msg.string1, supported->id, 64);
		msg.uint0 = supported->flags;
		squire_message_simple_box_send(&msg, sizeof(msg), VFS_FSDRIVER_PID, VFS_FSDRIVER_BOX);
		supported++;
	}

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		size_t length = 4096;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, _fsdriver_info->fsdriver_info->simple_box);

		squire_fsdriver_message_t * msg = (squire_fsdriver_message_t*)msg_buffer;
		_fsdriver_info->function_callback(from, msg);
	}
	return 0;
}
