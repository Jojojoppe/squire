/* _PDCLIB_open( char const * const, int )

   This file is part of the Public Domain C Library (PDCLib).
   Permission is granted to use, modify, and / or redistribute at will.
*/

/* This is a stub implementation of open.
*/

#include <stdio.h>
#include <errno.h>
#include "_PDCLIB_glue.h"
#include <squire.h>
#include <squire_vfs.h>

bool _PDCLIB_open( _PDCLIB_fd_t * pFd, const _PDCLIB_fileops_t ** pOps,
                   char const * const filename, unsigned int mode ){

	size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_file_t);
	size_t msg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
	squire_vfs_message_header_t * msg = (squire_vfs_message_header_t*)malloc(msg_size);
	memset(msg, 0, msg_size);
	msg->length = msg_size;
	msg->messages = 1;
	squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(msg+1);
	smsg_header->length = smsg_size;
	smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_OPEN;

    squire_vfs_submessage_file_t * f = (squire_vfs_submessage_file_t*)(smsg_header+1);
    f->box = 255;
    f->pid = squire_procthread_getpid();

    // Get mountpoint
    char * pt = strchr(filename, ':');
    if(!pt){
        errno = ENOTSUP;
        return false;
    }
    *pt = 0;
    // Path starts after :
    strcpy(f->path, pt+1);
    f->mountpoint = atoi(filename);

	squire_rpc_box(255, SQUIRE_VFS_PID, SQUIRE_VFS_USER_BOX, msg, msg_size, msg, msg_size);

    // Check status
    if(f->status){
        free(msg);
        errno = ENOTSUP;
        return false;
    }

    free(msg);
    pFd->pointer = 0x123456;
    return true;
}