#include <general/user_io.h>
#include <general/arch/proc.h>
#include <general/kmalloc.h>

user_io_isr_registration_t * user_io_isr_registrations = 0;
user_io_port_registration_t * user_io_port_registrations = 0;

void user_io_clean(unsigned int pid){
	// Delete registration from isr list
	user_io_isr_registration_t * isrreg = user_io_isr_registrations;
	if(isrreg){
		user_io_isr_registration_t * prev = 0;
		while(isrreg){
			if(isrreg->PID == pid){
				// Found! Now delete
				if(prev){
					prev->next = isrreg->next;
				}else{
					user_io_isr_registrations = isrreg->next;
				}
				prev = isrreg;
				isrreg = isrreg->next;

				user_io_arch_unregister_isr(isrreg->id, pid, &isrreg->archdata);
				kfree(prev);

				continue;
			}
			prev = isrreg;
			isrreg = isrreg->next;
		}
	}

	// Delete registration from port list
	user_io_port_registration_t * portreg = user_io_port_registrations;
	if(portreg){
		user_io_port_registration_t * prev = 0;
		while(portreg){
			if(portreg->PID == pid){
				// Found! Now delete
				if(prev){
					prev->next = portreg->next;
				}else{
					user_io_port_registrations = portreg->next;
				}
				prev = portreg;
				portreg = portreg->next;

				for(int i=0; i<prev->range; i++)
					user_io_arch_unregister_port(prev->port+i, prev->flags, pid, &prev->archdata);
				kfree(prev);

				continue;
			}
			prev = portreg;
			portreg = portreg->next;
		}
	}
}

int user_io_register_isr(unsigned int id, unsigned int PID){

	// Check if architecture allows this registration
	if(user_io_arch_may_register_isr(id)){
		// Not allowed
		return 1;
	}

	// Check if already registered
	if(user_io_isr_registrations){
		user_io_isr_registration_t * reg = user_io_isr_registrations;
		while(reg){
			if(reg->id == id){
				// Already registered
				return 1;
			}
			reg = reg->next;
		}
	}

	// May register
	user_io_isr_registration_t * newreg = (user_io_isr_registration_t*)kmalloc(sizeof(user_io_isr_registration_t));
	newreg->next = 0;
	newreg->id = id;
	newreg->PID = PID;

	// Add to list
	if(user_io_isr_registrations){
		user_io_isr_registration_t * reg = user_io_isr_registrations;
		while(reg->next){
			reg = reg->next;
		}
		reg->next = newreg;
	}else{
		user_io_isr_registrations = newreg;
	}
	
	// Register
	user_io_arch_register_isr(id, PID, &newreg->archdata);

	return 0;
}

int user_io_register_port(unsigned int port, unsigned int range, unsigned int flags, unsigned int PID){

	for(unsigned int i=0; i<range; i++){
		// Check if architecture allows this registration
		if(user_io_arch_may_register_port(port+i, flags)){
			// Not allowed
			return 1;
		}
	}

	// Check if already registered
	if(user_io_port_registrations){
		user_io_port_registration_t * reg = user_io_port_registrations;
		unsigned int startport = port;
		unsigned int endport = port+range-1;
		while(reg){
			if((startport<reg->port && endport>=reg->port) ||
			   (endport>=reg->port+reg->range && startport<reg->port+reg->range) ||
			   (startport>=reg->port && endport<reg->port+reg->range)){
				// Already registered
				return 1;
			}
			reg = reg->next;
		}
	}

	// May register
	user_io_port_registration_t * newreg = (user_io_port_registration_t*)kmalloc(sizeof(user_io_port_registration_t));
	newreg->next = 0;
	newreg->port = port;
	newreg->PID = PID;
	newreg->flags = flags;
	newreg->range = range;

	// Add to list
	if(user_io_port_registrations){
		user_io_port_registration_t * reg = user_io_port_registrations;
		while(reg->next){
			reg = reg->next;
		}
		reg->next = newreg;
	}else{
		user_io_port_registrations = newreg;
	}
	
	// Register
	for(unsigned int i=0; i<range; i++)
		user_io_arch_register_port(port+i, flags, PID, &newreg->archdata);

	return 0;
}

int user_io_port_outb(unsigned int port, unsigned char val){
	// Get current pid
	unsigned int pid = proc_proc_get_current()->id;

	// Check if permission
	if(user_io_port_registrations){
		user_io_port_registration_t * reg = user_io_port_registrations;
		while(reg){
			if(port>=reg->port && port<reg->port+reg->range){
				// Found registration
				if(reg->PID==pid){ // TODO flags
					user_io_arch_outb(port, val);
					return 0;
				}
				return 1;
			}
			reg = reg->next;
		}
	}
	return 1;
}

int user_io_port_inb(unsigned int port, unsigned char * val){
	// Get current pid
	unsigned int pid = proc_proc_get_current()->id;
	// Check if permission
	if(user_io_port_registrations){
		user_io_port_registration_t * reg = user_io_port_registrations;
		while(reg){
			if(port>=reg->port && port<reg->port+reg->range){
				// Found registration
				if(reg->PID==pid){ // TODO flags
					*val = user_io_arch_inb(port);
					return 0;
				}
				return 1;
			}
			reg = reg->next;
		}
	}
	return 1;
}

