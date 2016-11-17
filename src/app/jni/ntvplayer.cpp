
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/thread.h>

#include <sys/eventfd.h>  
#include <sys/socket.h>
#include <arpa/inet.h>

#include "x_thread.h"
#include "x_queue.h"
#include "x_lock.h"
#include "x_time.h"
#include "tsheader.h"


#include <android/log.h>
#include <jni.h>


#define tag "native-NTVPlayer"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, tag, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, tag, __VA_ARGS__))

#define queueSize	1024 * 1024

#define Quality_FULLHD	0
#define Quality_HD		1
#define Quality_DVD		2



//----------------------------------------
#define StreamType_Standard	0
#define StreamType_ZidooX9	1
int g_bCheckStreamType = 1;
int g_StreamgType = StreamType_ZidooX9;
//----------------------------------------




struct sockaddr_in local;
struct event_base *g_base;
int g_recvSize = 0;
int g_speed = 0;

unsigned char tsheader[188 * 4];
unsigned char tmpbuf[queueSize];
x_queue *g_queue;
x_lock queueLock, infoLock;

struct bufferevent *g_bev = NULL;

unsigned int g_totalRecvSize = 0;
unsigned int lastUpdate = 0;
int ls;

int gRecord;
char logingKeyp[128];

/*---------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------*/
void conn_readcb(struct bufferevent *bev, void *ptr)
{
	char tmp[8192];
	char *dataPtr;
	size_t n;

	while (1) {
		dataPtr = tmp;
		n = bufferevent_read(bev, tmp, sizeof(tmp));

		if (n <= 0)
			break; /* No more data. */

		if (g_bCheckStreamType == 1) {
			g_bCheckStreamType = 0;
			tmp[n] = 0;
			LOGI("Stream Type string:%s", tmp);
			if (strncmp(tmp, "stream_type:std", 15) == 0) {
				g_StreamgType = StreamType_Standard;
				dataPtr = dataPtr + 15;
				LOGI("Stream Type:standard");
				n -= 15;
				if (n <= 0)
					continue;
			}
			else {
				g_StreamgType = StreamType_ZidooX9;
				LOGI("Stream Type:ZidooX9");
			}
		}			
		

		x_lock_lock(&queueLock);
		g_recvSize += n;
		g_totalRecvSize += n;
		x_queue_write(g_queue, (unsigned char *)dataPtr, n);
		x_lock_unlock(&queueLock);

	}


	if ((x_time_get_tick() - lastUpdate) > 1000) {

		x_lock_lock(&infoLock);
		g_speed = ((g_recvSize / 1024) * 1000) / (x_time_get_tick() - lastUpdate);
		lastUpdate = x_time_get_tick();
		g_recvSize = 0;
		x_lock_unlock(&infoLock);

	}

}

/*---------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------*/
static void
conn_eventcb(struct bufferevent *bev, short events, void *user_data)
{

	if (events & BEV_EVENT_CONNECTED) {
		//bConnected = 1;
		char cmd[256];
		g_bev = bev;

		g_bCheckStreamType = 1;
		g_StreamgType = StreamType_ZidooX9;
		
		sprintf(cmd, "login:%s", logingKeyp);
		bufferevent_write(g_bev, cmd, strlen(cmd));
		LOGI("connection success");
	} 
	else if (events & BEV_EVENT_EOF) {
		LOGI("Connection closed.\n");
		g_bev = NULL;

	}
	else if (events & BEV_EVENT_ERROR) {
		LOGI("Got an error on the connection\n");
		g_bev = NULL;
	}
	else {
		LOGI("other event %x\n", events);
	}
	/* None of the other events can happen here, since we haven't enabled
	* timeouts */
	//bufferevent_free(bev);
}


int bStop;
x_thread_proc(SendThread, void *noUse)
{
	int iStep = 1;
	unsigned char buf[1316];
	

	x_queue *tsQueue;
	tsQueue = x_queue_alloc(queueSize + 4096);
	int qc;
	int bRecState = 0;
	FILE *fp = NULL;


	while (!bStop) {

		if (x_queue_count(tsQueue) < 1316) {
			x_lock_lock(&queueLock);
			qc = x_queue_count(g_queue);
			if (qc > 0) {
				x_queue_read(g_queue, tmpbuf, qc);
			}
			x_lock_unlock(&queueLock);
			if (qc > 0)
				x_queue_write(tsQueue, tmpbuf, qc);
			else {
				x_time_sleep(1000);
				continue;
			}
		}
		else {
			
			if (g_StreamgType == StreamType_Standard) {
									x_queue_read(tsQueue, buf, 1316);
					sendto(ls, (const char *)buf, 1316, 0, (const struct sockaddr*)&local, sizeof(struct sockaddr_in));

					if (gRecord != bRecState) {
						if (gRecord) {
							time_t timep; 
							struct tm *p; 
							time(&timep); 
							p=localtime(&timep);

							char fn[128];
							sprintf(fn,"/sdcard/NTVPlayerRecorder/NTVRec%04d%02d%02d%02d%02d%02d.ts",  (1900+p->tm_year), (1+p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
							fp = fopen(fn, "wb");
							if (fp) {
								LOGI("start record file(std): %s", fn);
								
							} else {
								LOGI("open record file: %s fail", fn);
							}
						}
						else {
							if (fp) {
								fclose(fp); 
								fp = NULL;
							}
						}
						bRecState = gRecord;
					}

					if (fp) {
						if (fwrite(buf, 1, 1316, fp) != 1316) {
							fclose(fp);
							fp = NULL;
						}
					}
					
					
				
			} else { // zidoo x9
				
			if (iStep == 1) {
				x_queue_read(tsQueue, tsheader, 188 * 4); // backup ts header
				iStep = 2;
			}
			else {
				if (iStep == 2) {
					for (int x = 0; x < 1316; x++) {
						if (tsQueue->data[tsQueue->Start] == 0x47) {
							iStep = 3;
							break;
						}
						else {
							unsigned char drop;
							x_queue_getch(tsQueue, &drop);
						}
					}
				}
				else if (iStep == 3) {
					sendto(ls, (const char *)tsheader, 188 * 4, 0, (const struct sockaddr*)&local, sizeof(struct sockaddr_in));
					iStep = 0;
				}
				else {
					x_queue_read(tsQueue, buf, 1316);
					sendto(ls, (const char *)buf, 1316, 0, (const struct sockaddr*)&local, sizeof(struct sockaddr_in));

					if (gRecord != bRecState) {
						if (gRecord) {
							time_t timep; 
							struct tm *p; 
							time(&timep); 
							p=localtime(&timep);

							char fn[128];
							sprintf(fn,"/sdcard/NTVPlayerRecorder/NTVRec%04d%02d%02d%02d%02d%02d.ts",  (1900+p->tm_year), (1+p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
							fp = fopen(fn, "wb");
							if (fp) {
								LOGI("open record file: %s", fn);
								if (fwrite(tsheader, 1, 188 * 4, fp) != (188 * 4)) {
									fclose(fp);
									fp = NULL;
								}
							}
							else {
								LOGI("open record file: %s fail", fn);
							}
						}
						else {
							if (fp) {
								fclose(fp); 
								fp = NULL;
							}
						}
						bRecState = gRecord;
					}

					if (fp) {
						if (fwrite(buf, 1, 1316, fp) != 1316) {
							fclose(fp);
							fp = NULL;
						}
					}
				}
			}
			
			}
			

		}



	};

	if (fp) {
		fclose(fp);
	}

	x_queue_free(tsQueue);
	x_thread_return_success;
}




extern "C" {
    JNIEXPORT jint JNICALL Java_tw_ironThomas_ntvplayer_NTVService_start(JNIEnv * env, jobject obj, jstring server, jint server_port, jint local_port, jstring key);
	JNIEXPORT void JNICALL Java_tw_ironThomas_ntvplayer_NTVService_stop(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_tw_ironThomas_ntvplayer_NTVService_command(JNIEnv * env, jobject obj, jstring cmd);
	JNIEXPORT void JNICALL Java_tw_ironThomas_ntvplayer_NTVService_quality(JNIEnv * env, jobject obj, jint qt);
	JNIEXPORT jstring JNICALL Java_tw_ironThomas_ntvplayer_NTVService_info(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_tw_ironThomas_ntvplayer_NTVService_record(JNIEnv * env, jobject obj, jint state, jint w, jint h);
};




JNIEXPORT void JNICALL Java_tw_ironThomas_ntvplayer_NTVService_record(JNIEnv * env, jobject obj, jint state, jint w, jint h)
{
	if(state) {
		if (w == 720) {
			memcpy(tsheader, ts_720_480, 188 * 4);
		} else if (w == 1280) {
			memcpy(tsheader, ts_1280_720, 188 * 4);
		} else if (w == 1920) {
			memcpy(tsheader, ts_1920_1080, 188 * 4);
		} 
	}
	gRecord = state;
	LOGI("record: %d", state);
}


JNIEXPORT jstring JNICALL Java_tw_ironThomas_ntvplayer_NTVService_info(JNIEnv * env, jobject obj)
{
		
	char info[256];
	unsigned int currentTime = x_time_get_tick();
	
	if((currentTime - lastUpdate) > 1500) { // too long time
		sprintf(info, "speed: 0 kb, total received %d kb", (g_totalRecvSize / 1024) );

	} else {
		sprintf(info, "speed: %d kb, total received %d kb", g_speed, (g_totalRecvSize / 1024));
	}

	return env->NewStringUTF(info);
}



#define Quality_FULLHD	0
#define Quality_HD		1
#define Quality_DVD		2



JNIEXPORT void JNICALL Java_tw_ironThomas_ntvplayer_NTVService_quality(JNIEnv * env, jobject obj, jint qt)
{
	if (g_bev) {
		if (qt == Quality_FULLHD) {
			//memcpy(tsheader, ts_1920_1080, 188 * 4);
			bufferevent_write(g_bev, "cmd:qt_fhd", strlen("cmd:qt_fhd"));
		}
		else if (qt == Quality_HD) {
			//memcpy(tsheader, ts_1280_720, 188 * 4);
			bufferevent_write(g_bev, "cmd:qt_hd", strlen("cmd:qt_hd"));
		}
		else {
			//memcpy(tsheader, ts_720_480, 188 * 4);
			bufferevent_write(g_bev, "cmd:qt_dvd", strlen("cmd:qt_dvd"));
		}
	}
}




JNIEXPORT void JNICALL Java_tw_ironThomas_ntvplayer_NTVService_command(JNIEnv * env, jobject obj, jstring cmd)
{
	    // convert Java string to UTF-8

	const char *utf8 = env->GetStringUTFChars(cmd, NULL);
	
	if(g_bev) {
		bufferevent_write(g_bev, utf8, strlen(utf8));
	}
    
	// release the Java string and UTF-8
    env->ReleaseStringUTFChars(cmd, utf8);
	

}



JNIEXPORT void JNICALL Java_tw_ironThomas_ntvplayer_NTVService_stop(JNIEnv * env, jobject obj)
{
	if(g_base) {
		LOGI("send loopexit signal");
		event_base_loopexit(g_base, NULL);
	}
}




int CreateLocalhost(int local_port)
{
	int s;
    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        LOGI("socket() failed");
        return -1;
    }
	
	int buffsize = 512 * 1024; // 512 K
	setsockopt(s, SOL_SOCKET, SO_SNDBUF, (const char*)&buffsize, sizeof(buffsize));

    //setup address structure
    memset((char *) &local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(local_port);
	local.sin_addr.s_addr = inet_addr("127.0.0.1");
    //---------------------------------------------------------------------------------------	

	return s;
}


JNIEXPORT jint JNICALL Java_tw_ironThomas_ntvplayer_NTVService_start(JNIEnv * env, jobject obj, jstring server, jint server_port, jint local_port, jstring key)
{
    
    struct evutil_addrinfo *ai = NULL;
	struct evutil_addrinfo hints;
	int result;
	struct bufferevent *bev;
	struct sockaddr_in *sin;
	
	
	
	
    // convert Java string to UTF-8
	char serverName[128];
    const char *utf8 = env->GetStringUTFChars(server, NULL);
	strcpy(serverName, utf8);
	// release the Java string and UTF-8
    env->ReleaseStringUTFChars(server, utf8);
	
	
	
	
	utf8 = env->GetStringUTFChars(key, NULL);
	strcpy(logingKeyp, utf8);
    env->ReleaseStringUTFChars(server, utf8);
	
 

	gRecord = 0;
	
    if ( (ls=CreateLocalhost(local_port)) < 0)
    {
        LOGI("socket() failed");
        return -1;
    }


	evthread_use_pthreads();

    g_base = event_base_new();
	if (!g_base) {
		LOGI("Could not initialize libevent!");
		close(ls);
		g_base = NULL;
		return -2;
	}


	/* Now do some actual lookups. */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

	//result = evutil_getaddrinfo("thomassun.ddns.net", NULL, &hints, &ai);
	result = evutil_getaddrinfo(serverName, NULL, &hints, &ai);

	memset(&sin, 0, sizeof(sin));
	sin = (struct sockaddr_in*)ai->ai_addr;
	//sin->sin_port    = htons(80);
	sin->sin_port    = htons(server_port);


	bev = bufferevent_socket_new(g_base, -1, BEV_OPT_CLOSE_ON_FREE);
	if (!bev) {
		LOGI("Error constructing bufferevent!");
		event_base_loopbreak(g_base);
		event_base_free(g_base);
		g_base = NULL;
		close(ls);
		return -3;
	}


	bufferevent_setcb(bev, conn_readcb, NULL, conn_eventcb, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

	if (bufferevent_socket_connect(bev, (struct sockaddr *) sin, sizeof(struct sockaddr))) {
		LOGI("connect fail");
		close(ls);
		bufferevent_free(bev);
		event_base_loopbreak(g_base);
		event_base_free(g_base);
		g_base = NULL;
		return -4;
	}
	
	
	g_queue = x_queue_alloc(queueSize);
	x_lock_init(&queueLock);
	x_lock_init(&infoLock);
	
	bStop = 0;
	x_thread	MyThread;
	x_thread_start(&MyThread, SendThread, NULL);	

		

	event_base_dispatch(g_base);
	
	
	bStop	= 1;
	x_time_sleep(1000 * 1000);
	
	
	bufferevent_free(bev);
	event_base_free(g_base);
	g_base = NULL;
	close(ls);
	x_queue_free(g_queue);
	x_lock_free(&queueLock);
	x_lock_free(&infoLock);
	LOGI("server has stop");
	return 0;
}

