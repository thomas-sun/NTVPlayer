#ifndef _x_time_h__
#define _x_time_h__
#include "x_type.h"

#if defined(X_WIN32)
#include "windows.h"
#include "mmsystem.h"
#elif defined(X_LINUX)
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#elif defined(X_ANDROID) || defined(X_OSX)
#include <sys/stat.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif


extern void x_time_sleep(unsigned int t);
extern unsigned int x_time_get_tick();


#ifdef __cplusplus
}
#endif


#endif