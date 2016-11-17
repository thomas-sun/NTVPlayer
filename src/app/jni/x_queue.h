#ifndef _x_queue_h__
#define _x_queue_h__
#include "x_type.h"

typedef struct
{
	x_int32 Count;
	x_int32 Start;
	x_int32 Size;
	x_uint8 *data;
} x_queue;

#ifdef __cplusplus
extern "C" {
#endif

extern void x_queue_free(x_queue *This);
extern x_queue *x_queue_alloc(int size);
extern x_int32 x_queue_read(x_queue *This, x_uint8 *buf, x_int32 s);
extern void x_queue_clear(x_queue *This);
extern x_int32 x_queue_copy(x_queue *This, x_int32 pos, x_uint8 *buf, x_int32 s);
extern x_int32 x_queue_write(x_queue *This, x_uint8 *buf, x_int32 size);
extern x_int32 x_queue_count(x_queue *This);
extern x_int32 x_queue_getch(x_queue *This, unsigned char *buf);

#ifdef __cplusplus
}
#endif

#endif