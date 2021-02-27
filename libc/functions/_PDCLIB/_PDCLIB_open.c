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
#include <squire_crypt.h>

extern const _PDCLIB_fileops_t _PDCLIB_fileops;

bool _PDCLIB_open( _PDCLIB_fd_t * pFd, const _PDCLIB_fileops_t ** pOps,
                   char const * const filename, unsigned int mode ){

	extern char _signature[33];
    size_t msg_size = VFS_MSG_LEN(sizeof(squire_vfs_submessage_file_t));
    squire_vfs_message_header_t * msg;
    squire_vfs_submessage_file_t * f = (squire_vfs_submessage_file_t*) squire_vfs_create_message(&msg, msg_size, _signature, SQUIRE_VFS_SUBMESSAGE_OPEN);

    f->box = 255;
    f->pid = squire_procthread_getpid();
    f->nonce = 0;

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

	// Check if message has correct signature, else return error
	if(memcmp(_signature, msg->signature, 32)){
        f->status = -2;
	}

    // Check status
    if(f->status){
        free(msg);
        errno = ENOTSUP;
        return false;
    }

    // Decrypt driver signature
    squire_chacha(_signature, 0, 0, f->signature, 32);
    memcpy(pFd->signature, f->signature, 32);

    *pOps = &_PDCLIB_fileops;

    pFd->fdesc = f->fdesc;
    pFd->mountpoint = f->mountpoint;
    pFd->dpid = f->dpid;
    pFd->dbox = f->dbox;
    pFd->offset = 0;
    pFd->nonce = f->nonce;
    free(msg);
    return true;
}