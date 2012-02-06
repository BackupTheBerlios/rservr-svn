extern "C" {
#include "socket-mutex.h"
}

#include <pthread.h> //'pthread_mutex_lock', etc.

#include "global/condition-block.hpp"


static auto_mutex socket_mutex;


int lock_mutex()
{
	if (!socket_mutex.valid()) return false;
	return pthread_mutex_lock(socket_mutex) == 0;
}


int unlock_mutex()
{
	if (!socket_mutex.valid()) return false;
	return pthread_mutex_unlock(socket_mutex) == 0;
}
