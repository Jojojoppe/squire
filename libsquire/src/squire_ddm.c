#include <squire.h>
#include <squire_rpc.h>
#include <squire_ddm.h>

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <threads.h>

// libc internals
// --------------
struct _thrd_arg{
    thrd_start_t func;
    void * arg;
    void * stack;
};

int _thrd_start(struct _thrd_arg * param){
    int retval = param->func(param->arg);
    free(param->stack);
    free(param);
    return retval;
}

int thrd_create(thrd_t * thr, thrd_start_t func, void * arg){
    struct _thrd_arg * param = malloc(sizeof(struct _thrd_arg));
    param->arg = arg;
    param->func = func;
    param->stack = malloc(THRD_STACK_LENGTH);    
    *thr = squire_procthread_create_thread(_thrd_start, param->stack, THRD_STACK_LENGTH, 0, param);
    return thrd_success;
}

void strcpy(char * dst, char * src){
    while(*src){
        *dst = *src;
        dst++; src++;
    }
}
// --------------

extern squire_ddm_driver_t * __ddm_driver_info;

int squire_ddm_driver_child_main(void * p){
	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, __ddm_driver_info->child_box);
        squire_ddm_message_header_t * hdr = (squire_ddm_message_header_t*) msg_buffer;
        squire_ddm_submessage_header_t * smsg_hdr = (squire_ddm_submessage_header_t*)(hdr+1);
        for(int i=0; i<hdr->messages; i++){
            if(__ddm_driver_info->interdriver){
                __ddm_driver_info->interdriver(smsg_hdr, smsg_hdr->length, from);
            }
            smsg_hdr = (squire_ddm_submessage_header_t*)((void*)smsg_hdr + smsg_hdr->length);
        };
	}
	return EXIT_SUCCESS;
}

int squire_ddm_driver_main(int argc, char ** argv){

    // Register driver
    size_t message_size = sizeof(squire_ddm_message_header_t) + sizeof(squire_ddm_submessage_header_t) + sizeof(squire_ddm_driver_t);
    size_t submessage_size = sizeof(squire_ddm_submessage_header_t) + sizeof(squire_ddm_driver_t);
    uint8_t * msg = (uint8_t *) malloc(message_size);
    // Fill message content
    squire_ddm_message_header_t * hdr = (squire_ddm_message_header_t*) msg;
    hdr->messages = 1;
    hdr->length = message_size;
    squire_ddm_submessage_header_t * smsg_hdr = (squire_ddm_submessage_header_t*)(hdr+1);
    smsg_hdr->submessage_type = SQUIRE_DDM_SUBMESSAGE_REGISTER_DRIVER;
    smsg_hdr->length = submessage_size;
    squire_ddm_driver_t * smsg_content = (squire_ddm_driver_t*)(smsg_hdr+1);
    __ddm_driver_info->pid = squire_procthread_getpid();
    memcpy(smsg_content, __ddm_driver_info, sizeof(squire_ddm_driver_t));
    squire_message_simple_box_send(msg, message_size, SQUIRE_DDM_PID, SQUIRE_DDM_DRIVER_BOX);
    free(msg);

    if(__ddm_driver_info->child_box){
        // Start child box receive thread
        thrd_t t;
        thrd_create(&t, squire_ddm_driver_child_main, 0);
    }

	uint8_t * msg_buffer = (uint8_t *) squire_memory_mmap(0, 4096, MMAP_READ|MMAP_WRITE);
	unsigned int from;
	for(;;){
		// Wait for message and block main thread
		size_t length = 4096;;
		squire_message_status_t status = squire_message_simple_box_receive(msg_buffer, &length, &from, RECEIVE_BLOCKED, __ddm_driver_info->box);

        squire_ddm_message_header_t * hdr = (squire_ddm_message_header_t*) msg_buffer;
        squire_ddm_submessage_header_t * smsg_hdr = (squire_ddm_submessage_header_t*)(hdr+1);
        for(int i=0; i<hdr->messages; i++){
            switch(smsg_hdr->submessage_type){

                case SQUIRE_DDM_SUBMESSAGE_INIT:{
                    squire_ddm_submessage_init_t * smsg_init = (squire_ddm_submessage_init_t*)(smsg_hdr+1);
                    if(__ddm_driver_info->init)
                        __ddm_driver_info->init(smsg_init->device, smsg_init->type);
                } break;

                case SQUIRE_DDM_SUBMESSAGE_ENUM:{
                    squire_ddm_submessage_enum_t * smsg_init = (squire_ddm_submessage_enum_t*)(smsg_hdr+1);
                    if(__ddm_driver_info->enum)
                        __ddm_driver_info->enum(smsg_init->device, smsg_init->type);
                } break;

                default:
                    break;
            }
            smsg_hdr = (squire_ddm_submessage_header_t*)((void*)smsg_hdr + smsg_hdr->length);
        };
	}
	return EXIT_SUCCESS;
}

void squire_ddm_driver_register_device(char * device, char * type, squire_ddm_device_type_t device_type, char * parent){
    size_t message_size = sizeof(squire_ddm_message_header_t) + sizeof(squire_ddm_submessage_header_t) + sizeof(squire_ddm_submessage_register_device_t);
    size_t submessage_size = sizeof(squire_ddm_submessage_header_t) + sizeof(squire_ddm_submessage_register_device_t);
    uint8_t * msg = (uint8_t *) malloc(message_size);
    memset(msg, 0, message_size);
    // Fill message content
    squire_ddm_message_header_t * hdr = (squire_ddm_message_header_t*) msg;
    hdr->messages = 1;
    hdr->length = message_size;
    squire_ddm_submessage_header_t * smsg_hdr = (squire_ddm_submessage_header_t*)(hdr+1);
    smsg_hdr->submessage_type = SQUIRE_DDM_SUBMESSAGE_REGISTER_DEVICE;
    smsg_hdr->length = submessage_size;
    squire_ddm_submessage_register_device_t * smsg_content = (squire_ddm_submessage_register_device_t*)(smsg_hdr+1);

    smsg_content->device_type = device_type;
    strcpy(smsg_content->device, device);
    strcpy(smsg_content->type, type);
    strcpy(smsg_content->parent, parent);

    squire_message_simple_box_send(msg, message_size, SQUIRE_DDM_PID, SQUIRE_DDM_DRIVER_BOX);
    free(msg);
}

void squire_ddm_driver_request_parent(squire_ddm_driver_t * parent){
    size_t message_size = sizeof(squire_ddm_message_header_t) + sizeof(squire_ddm_submessage_header_t) + sizeof(squire_ddm_submessage_request_parent_t);
    squire_ddm_message_header_t * msg = (squire_ddm_message_header_t*)malloc(message_size);
    squire_ddm_submessage_header_t * smsg_header = (squire_ddm_submessage_header_t*)(msg+1);
    msg->length = message_size;
    msg->messages = 1;
    smsg_header->length = sizeof(squire_ddm_submessage_header_t) + sizeof(squire_ddm_submessage_request_parent_t);
    smsg_header->submessage_type = SQUIRE_DDM_SUBMESSAGE_REQUEST_PARENT;
    squire_ddm_submessage_request_parent_t * rprnt = (squire_ddm_submessage_request_parent_t*)(smsg_header+1);
    rprnt->box = 255;
    strcpy(rprnt->driver, __ddm_driver_info->name);

    uint8_t buf[sizeof(squire_ddm_driver_t)+sizeof(squire_ddm_message_header_t)+sizeof(squire_ddm_submessage_header_t)];
    squire_rpc_box(255, SQUIRE_DDM_PID, SQUIRE_DDM_DRIVER_BOX, msg, message_size, buf, sizeof(squire_ddm_driver_t)+sizeof(squire_ddm_submessage_header_t)+sizeof(squire_ddm_message_header_t));

    memcpy(parent, buf+sizeof(squire_ddm_message_header_t)+sizeof(squire_ddm_submessage_header_t), sizeof(squire_ddm_driver_t));

    free(msg);
}