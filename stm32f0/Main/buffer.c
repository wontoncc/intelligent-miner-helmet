#include "buffer.h"
#include "usart.h"

BufferType serialBuffer = { "", 0 };

void fillBuffer(BufferType *buf, char ch){
	int i;
	for(i = 0; i < 512; i++){
		buf->array[i] = ch;
	}
}

int pushToSerialBuffer(char ch){
	if(serialBuffer.end + 1 < 512 - 1){
		serialBuffer.array[serialBuffer.end++] = ch;
		return serialBuffer.end;
	} else{
		return -1;
	}
}

void bufferReset(BufferType *buf){
	fillBuffer(buf, 0);
	buf->end = 0;
}
