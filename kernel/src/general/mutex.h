#ifndef __H_MUTEX
#define __H_MUTEX 1

#include <general/arch/spinlock.h>

typedef struct schedule_schedulable_s schedule_schedulable_t;

typedef struct mutex_waiting_s{
	struct mutex_waiting_t * next;
	schedule_schedulable_t * schedulable;
} mutex_waiting_t;

typedef struct mutex_s{
	spinlock_t access_lock;
	unsigned int lock;
	mutex_waiting_t * waiting;
	// For list of mutexes in process
	struct mutex_s * next;
} mutex_t;

mutex_t * mutex_create();
void mutex_destroy(mutex_t * mutex);
void mutex_lock(mutex_t * mutex);
void mutex_unlock(mutex_t * mutex);
unsigned int mutex_get(mutex_t * mutex);

#endif
