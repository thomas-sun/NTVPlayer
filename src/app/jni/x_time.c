#include "x_time.h"

//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
unsigned int x_time_get_tick()
{
#if defined(WIN32)
	return timeGetTime();
#else
    struct timeval t;
    t.tv_sec = t.tv_usec = 0;
    if(gettimeofday(&t, NULL) == -1) {
        return 0;
    }
    return (unsigned int)(t.tv_sec*1000LL + t.tv_usec/1000LL);
#endif
}
//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
void x_time_sleep(unsigned int t)
{
#if defined(WIN32)
	Sleep(t/1000);
#else
	usleep(t);
#endif
}



