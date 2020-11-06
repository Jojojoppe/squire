#ifndef __H_DEVMAN_DEVMAN
#define __H_DEVMAN_DEVMAN 1

#include <stdint.h>
#include <squire_driver.h>

#define MSG_BOX_DEVMAN_SIMPLE 0

typedef struct devman_driver_s{
	struct devman_driver_s * next;
	char name[64];
	uint8_t id[64];
	uint32_t functions;
	unsigned int PID;
	unsigned int simple_box;
} devman_driver_t;

typedef struct devman_device_s{
	struct devman_device_s * childs;
	struct devman_device_s * next;

	uint8_t id[64];
	uint32_t functions;
	devman_driver_t * driver;
} devman_device_t;



int devman_main(void * p);

#endif
