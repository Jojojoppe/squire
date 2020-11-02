#include <general/user_io.h>

int user_io_register_isr(unsigned int id, unsigned int PID){

	// Check if architecture allows this registration
	if(user_io_arch_may_register_isr(id)){
		// Not allowed
		return 1;
	}
	
	// Register
	user_io_arch_register_isr(id, PID);

	return 0;
}
