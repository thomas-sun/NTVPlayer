#include "x_queue.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>


//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
x_queue *x_queue_alloc(int size)
{
	x_queue *p = (x_queue *)malloc(sizeof(x_queue));
	p->Count = 0;
	p->Start = 0;
	p->Size = size;
	p->data = (unsigned char *)malloc(size);
	return p;
}
//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
void x_queue_clear(x_queue *This)
{
	This->Count = 0;
	This->Start = 0;
}
//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
void x_queue_free(x_queue *This)
{
	if(This) {
		if(This->data) {
			free(This->data);
		}
		free(This);
	}
}
//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
x_int32 x_queue_read(x_queue *This, unsigned char *buf, int s)
{
	if(This->Count < s)
		return 0;

	if((This->Start + s) > This->Size) {
		int first = This->Size - This->Start;
		int other = s - first;
		memcpy(buf, This->data+This->Start, first);
		memcpy(buf+first, This->data, other);
		This->Count -= s;
		This->Start=other;
	}
	else {
		memcpy(buf, This->data+This->Start, s);
		This->Start += s;
		This->Start %= This->Size;
		This->Count -= s;
	}
	return s;

}
//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
x_int32 x_queue_getch(x_queue *This, unsigned char *buf)
{
	if(This->Count < 0)
		return 0;

	*buf = This->data[This->Start];
	This->Start++;
	This->Start %= This->Size;
	This->Count--;
	return 1;

}
//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
x_int32 x_queue_copy(x_queue *This, x_int32 pos, unsigned char *buf, int s)
{
	if(This->Count < s) // out of count
		return 0;


	if((pos + s) > This->Count)
		return 0;

	x_int32 ptr = (This->Start + pos)%This->Size;

	if((ptr + s) > This->Size) {
		int first = This->Size - ptr;
		int other = s - first;
		memcpy(buf, This->data+ptr, first);
		memcpy(buf+first, This->data, other);
	}
	else {
		memcpy(buf, This->data+ptr, s);
	}
	return s;

}
//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
x_int32 x_queue_write(x_queue *This, unsigned char *buf, int size)
{
	int ptr = (This->Start + This->Count)%This->Size;
	if((size + This->Count) > This->Size)
		return 0;

	if((ptr + size) > This->Size) {
		int first = This->Size - ptr;
		int other = size - first;
		memcpy(This->data+ptr, buf, first);
		memcpy(This->data, buf+first, other);
	}
	else {
		memcpy(This->data+ptr, buf, size);
	}
	This->Count += size;
	return size;
}
//----------------------------------------------------------------------------------------------------
//
//
//----------------------------------------------------------------------------------------------------
x_int32 x_queue_count(x_queue *This)
{
	return This->Count;
}
