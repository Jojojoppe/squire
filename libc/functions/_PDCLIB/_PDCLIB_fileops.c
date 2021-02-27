#include <stdio.h>
#include <stdint.h>
#include "_PDCLIB_glue.h"
#include <errno.h>

#include <squire.h>
#include <squire_vfs.h>
#include <squire_crypt.h>

static bool readf( _PDCLIB_fd_t self, void * buf, size_t length, 
                   size_t * numBytesRead )
{

    extern char _signature[33];
    size_t msg_size = VFS_MSG_LEN(sizeof(squire_vfs_submessage_file_t)) + length;
    squire_vfs_message_header_t * msg;
    squire_vfs_submessage_file_t * f = (squire_vfs_submessage_file_t*) squire_vfs_create_message(&msg, msg_size, self.signature, SQUIRE_VFS_SUBMESSAGE_READ);
    // Encrypt signature with sig_u | nonce
    squire_chacha(_signature, 0, self.nonce, msg->signature, 32);

    f->box = 255;
    f->pid = squire_procthread_getpid();
    f->mountpoint = self.sval;
    f->fdesc = self.fdesc;
    f->length = length;
    f->offset = self.offset;

	squire_rpc_box(255, self.dpid, self.dbox, msg, msg_size, msg, msg_size);

	// Check if message has correct signature, else return error
	if(memcmp(self.signature, msg->signature, 32)){
        f->status = -2;
	}

    // Check status
    if(f->status){
        free(msg);
        errno = ENOTSUP;
        return false;
    }

    errno = ENOTSUP;
    *numBytesRead = f->length;
    char * dat = (char*)(f+1);
    memcpy(buf, dat, f->length);
    free(msg);
    return true;
}

static bool writef( _PDCLIB_fd_t self, const void * buf, size_t length, 
                   size_t * numBytesWritten )
{

    squire_debug_log(buf, length);
    *numBytesWritten = length;

    return true;
}
static bool seekf( _PDCLIB_fd_t self, int_fast64_t offset, int whence,
    int_fast64_t* newPos )
{
    errno = ENOTSUP;
    return false;
}

static void closef( _PDCLIB_fd_t self )
{
    extern char _signature[33];
    size_t msg_size = VFS_MSG_LEN(sizeof(squire_vfs_submessage_file_t));
    squire_vfs_message_header_t * msg;
    squire_vfs_submessage_file_t * f = (squire_vfs_submessage_file_t*) squire_vfs_create_message(&msg, msg_size, self.signature, SQUIRE_VFS_SUBMESSAGE_CLOSE);
    // Encrypt signature with sig_u | nonce
    squire_chacha(_signature, 0, self.nonce, msg->signature, 32);

    f->box = 255;
    f->pid = squire_procthread_getpid();
    f->mountpoint = self.sval;
    f->fdesc = self.fdesc;

	squire_rpc_box(255, self.dpid, self.dbox, msg, msg_size, msg, msg_size);

	// Check if message has correct signature, else return error
	if(memcmp(self.signature, msg->signature, 32)){
        f->status = -2;
	}

    free(msg);
    return;
}

const _PDCLIB_fileops_t _PDCLIB_fileops = {
    .read  = readf,
    .write = writef,
    .seek  = seekf,
    .close = closef,
};