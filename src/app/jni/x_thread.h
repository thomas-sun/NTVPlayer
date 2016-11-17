#ifndef __x_thread__h__
#define __x_thread__h__


#include "x_type.h"


#if defined(X_WIN32)
#include <process.h>
#include <windows.h>
#elif defined(X_ANDROID) || defined(X_OSX) || defined(X_LINUX)
#include <pthread.h>
#endif





typedef struct 
{
#if defined(X_WIN32)
	unsigned int	addr;
	uintptr_t		thread_id;
#else
	pthread_t		thread_id;
#endif
} x_thread;



#if defined(X_WIN32)

#define x_thread_return_success return 0;

typedef unsigned int (__stdcall *x_thread_proc_type)(void *arg);

#define x_thread_proc(name, arg) \
	unsigned int __stdcall name(arg)


#elif defined(X_ANDROID) || defined(X_OSX) || defined(X_LINUX)

#define x_thread_return_success return NULL;

typedef void *(*x_thread_proc_type)(void *arg);

#define x_thread_proc(name, arg) \
	void * name(arg)

#endif






#ifdef __cplusplus
extern "C" {
#endif

#define x_thread_start(a,b,c) x_thread_start2(a, (x_thread_proc_type) b, c)

extern void x_thread_start2(x_thread *This, x_thread_proc_type proc, void *arg);


#ifdef __cplusplus
}
#endif


#endif