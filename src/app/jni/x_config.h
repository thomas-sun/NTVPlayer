#ifndef __x_config_h__
#define __x_config_h__



//
#if defined(WIN32)
#define X_WIN32
#elif defined(__APPLE_CC__) || defined(__APPLE_CPP_)
#define X_OSX
#elif defined(X_LINUX)
	// define by compiler
#else
	#define X_ANDROID
#endif




#endif
