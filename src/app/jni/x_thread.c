#include "x_thread.h"

//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
void x_thread_start2(x_thread *This, x_thread_proc_type proc, void *arg)
{
#if defined(X_WIN32)
	unsigned int	addr;
	uintptr_t		thread_id;
	if(This)
		This->thread_id = _beginthreadex(NULL, 0, proc, arg, 0, &This->addr);
	else
		thread_id = _beginthreadex(NULL, 0, proc, arg, 0, &addr);
#else
	pthread_t		thread_id;
	int				error_code;

	if(This)
		error_code	= pthread_create(&This->thread_id, NULL, proc, arg);
	else
		error_code	= pthread_create(&thread_id, NULL, proc, arg);

#endif	
}