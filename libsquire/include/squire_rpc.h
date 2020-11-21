#ifndef __H_SQUIRE_RPC
#define __H_SQUIRE_RPC 1

#include <stddef.h>

int squire_rpc_box(unsigned int ownbox, unsigned pid, unsigned int box, void * data, size_t length, void * buffer, size_t buffer_length);

#endif
