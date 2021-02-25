#include <stdio.h>
#include <stdint.h>
#include "_PDCLIB_glue.h"
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>

#include <squire.h>
#include <squire_vfs.h>

DIR * opendirs;

DIR * opendir(const char * dirname){
    if(!dirname){
        errno = ENOTDIR;
        return NULL;
    }

	size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_dir_t);
	size_t msg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
	squire_vfs_message_header_t * msg = (squire_vfs_message_header_t*)malloc(msg_size);
	memset(msg, 0, msg_size);
	msg->length = msg_size;
	msg->messages = 1;
	squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(msg+1);
	smsg_header->length = smsg_size;
	smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_OPENDIR;

    squire_vfs_submessage_dir_t * d = (squire_vfs_submessage_dir_t*)(smsg_header+1);
    d->box = 255;
    d->pid = squire_procthread_getpid();

    // Get mountpoint
    char * pt = strchr(dirname, ':');
    if(!pt){
        free(msg);
        errno = ENOTDIR;
        return NULL;
    }
    *pt = 0;
    // Path starts after :
    strcpy(d->path, pt+1);
    d->mountpoint = atoi(dirname);

	squire_rpc_box(255, SQUIRE_VFS_PID, SQUIRE_VFS_USER_BOX, msg, msg_size, msg, msg_size);

    // Check status
    if(d->status){
        free(msg);
        errno = ENOTDIR;
        return;
    }

    // Create DIR entry
    DIR * dir = (DIR*)malloc(sizeof(DIR));
    dir->next = 0;
    if(opendirs){
        DIR * l = opendirs;
        while(l->next) l=l->next;
        l->next = dir;
    }else{
        opendirs = dir;
    }
    dir->current_entry = 0;
    dir->pid = d->dpid;
    dir->box = d->dbox;
    
	free(msg);

    return dir;
}

void closedir(DIR * dir){
}

struct dirent* readdir(DIR * dir){
    if(dir==0 || dir->pid==0 || dir->box==0) return NULL;

    size_t smsg_size = sizeof(squire_vfs_submessage_header_t) + sizeof(squire_vfs_submessage_dir_t);
	size_t msg_size = sizeof(squire_vfs_message_header_t) + smsg_size;
	squire_vfs_message_header_t * msg = (squire_vfs_message_header_t*)malloc(msg_size);
	memset(msg, 0, msg_size);
	msg->length = msg_size;
	msg->messages = 1;
	squire_vfs_submessage_header_t * smsg_header = (squire_vfs_submessage_header_t*)(msg+1);
	smsg_header->length = smsg_size;
	smsg_header->submessage_type = SQUIRE_VFS_SUBMESSAGE_READDIR;

    squire_vfs_submessage_dir_t * d = (squire_vfs_submessage_dir_t*)(smsg_header+1);
    d->box = 255;
    d->pid = squire_procthread_getpid();
    d->dirent.d_fileno = dir->current_entry;

	squire_rpc_box(255, dir->pid, dir->box, msg, msg_size, msg, msg_size);

    // Check status
    if(d->status){
        free(msg);
        return NULL;
    }

    memcpy(&dir->dirent, &d->dirent, sizeof(struct dirent));
    dir->current_entry++;
    free(msg);
    return &dir->dirent;
}