#ifndef _x_lock_h__
#define _x_lock_h__
#include "x_type.h"

#if defined(X_WIN32)
#include "windows.h"
#elif defined(X_ANDROID) || defined(X_OSX) || defined(X_LINUX)
#include <pthread.h>
#endif


typedef struct 
{

#if defined(X_WIN32)
CRITICAL_SECTION	mutex;
#elif defined(X_ANDROID) || defined(X_OSX) || defined(X_LINUX)
pthread_mutex_t		mutex;
#endif

} x_lock;

#ifdef __cplusplus
extern "C" {
#endif

extern void x_lock_init(x_lock *This);
extern void x_lock_free(x_lock *This);
extern void x_lock_lock(x_lock *This);
extern void x_lock_unlock(x_lock *This);

#ifdef __cplusplus
}
#endif

#endif