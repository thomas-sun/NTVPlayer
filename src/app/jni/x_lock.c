#include "x_lock.h"

//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
void x_lock_init(x_lock *This)
{
#if defined(X_WIN32)
	InitializeCriticalSection(&This->mutex);
#elif defined(X_ANDROID) || defined(X_OSX) || defined(X_LINUX)
	pthread_mutex_init(&This->mutex, NULL);
#endif
}

//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
void x_lock_free(x_lock *This)
{
#if defined(X_WIN32)
	DeleteCriticalSection(&This->mutex);
#elif defined(X_ANDROID) || defined(X_OSX) || defined(X_LINUX)
	//
#endif
}
//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
void x_lock_lock(x_lock *This)
{
#if defined(X_WIN32)
	EnterCriticalSection(&This->mutex);
#elif defined(X_ANDROID) || defined(X_OSX) || defined(X_LINUX)
	pthread_mutex_lock(&This->mutex);
#endif
}
//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
void x_lock_unlock(x_lock *This)
{
#if defined(X_WIN32)
	LeaveCriticalSection(&This->mutex);
#elif defined(X_ANDROID) || defined(X_OSX) || defined(X_LINUX)
	pthread_mutex_unlock(&This->mutex);
#endif
} 
//----------------------------------------------------------------------------------------------------