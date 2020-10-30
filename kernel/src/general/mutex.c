#include <general/mutex.h>
#include <general/kmalloc.h>
#include <general/schedule.h>
#include <general/arch/proc.h>

mutex_t * mutex_create(){
	mutex_t * mtx = (mutex_t*) kmalloc(sizeof(mutex_t));
	spinlock_init(&mtx->access_lock);
	mtx->lock = 0;
	mtx->waiting = 0;
	mtx->next = 0;

	// Add mutex to process structure
	proc_proc_t * p = proc_proc_get_current();
	mutex_t * m = p->mutexes;
	if(!m){
		p->mutexes = mtx;
	}else{
		while(m->next)
			m = m->next;
		m->next = mtx;
	}

	return mtx;
}

void mutex_destroy(mutex_t * mutex){
	// Unlock all waiting threads
	mutex_unlock(mutex);
	// Directly lock access to mutex itself
	spinlock_lock(&mutex->access_lock);
	// Delete waiting structures
	mutex_waiting_t * w = mutex->waiting;
	while(w){
		mutex_waiting_t * tmp = w;
		w = w->next;
		kfree(tmp);
	}

	// TODO delete from process structure

	kfree(mutex);
}

void mutex_lock(mutex_t * mutex){
	while(1){
		// Wait for other threads to let go of structure
		spinlock_lock(&mutex->access_lock);
		if(!mutex->lock){
			// Mutex is unlocked, lock it
			mutex->lock = 1;
			spinlock_unlock(&mutex->access_lock);
			return;
		}
		// Mutex is locked
		// Add own thread to waiting list of mutex (if not already there)
		mutex_waiting_t * waiting = mutex->waiting;
		schedule_schedulable_t * ownthread = schedule_get(0, 0);
		if(!waiting){
			mutex->waiting = (mutex_waiting_t *) kmalloc(sizeof(mutex_waiting_t));
			mutex->waiting->next = 0;
			mutex->waiting->schedulable = ownthread;
			waiting = mutex->waiting;
		}else{
			mutex_waiting_t * own = 0;
			while(1){
				if(waiting->schedulable==ownthread)
					own = waiting;
				if(waiting->next)
					waiting = waiting->next;
				else
					break;
			}
			if(!own){
				own = (mutex_waiting_t *) kmalloc(sizeof(mutex_waiting_t));
				own->next = 0;
				own->schedulable = ownthread;
				waiting->next = own;
				waiting = own;
			}
		}
		spinlock_unlock(&mutex->access_lock);
		schedule_set_state(0, SCHEDULE_STATE_IDLE);
		schedule();
	}
}

void mutex_unlock(mutex_t * mutex){
	spinlock_lock(&mutex->access_lock);
	mutex->lock = 0;
	// Start all waiting threads
	mutex_waiting_t * w = mutex->waiting;
	while(w){
		schedule_set_state(w->schedulable, SCHEDULE_STATE_RUNNING);
		w = w->next;
	}
	spinlock_unlock(&mutex->access_lock);
}

unsigned int mutex_get(mutex_t * mutex){
	unsigned int res = 0;
	spinlock_lock(&mutex->access_lock);
	res = mutex->lock;
	spinlock_unlock(&mutex->access_lock);
	return res;
}
