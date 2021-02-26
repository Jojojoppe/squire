#include <stdio.h>
#include <stdint.h>
#include "_PDCLIB_glue.h"
#include <errno.h>

#include <squire.h>

static bool readf( _PDCLIB_fd_t self, void * buf, size_t length, 
                   size_t * numBytesRead )
{

	size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_file_t);
	size_t msg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
	squire_vfs_message_header_t * msg = (squire_vfs_message_header_t*)malloc(msg_size+length);
	memset(msg, 0, msg_size);
	msg->length = msg_size;
	msg->messages = 1;
	squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(msg+1);
	smsg_header->length = smsg_size;
	smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_READ;

    squire_vfs_submessage_file_t * f = (squire_vfs_submessage_file_t*)(smsg_header+1);
    f->box = 255;
    f->pid = squire_procthread_getpid();
    f->mountpoint = self.sval;
    f->fdesc = self.fdesc;
    f->length = length;
    f->offset = self.offset;

	squire_rpc_box(255, self.dpid, self.dbox, msg, msg_size, msg, msg_size);

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
    errno = ENOTSUP;
}

const _PDCLIB_fileops_t _PDCLIB_fileops = {
    .read  = readf,
    .write = writef,
    .seek  = seekf,
    .close = closef,
};