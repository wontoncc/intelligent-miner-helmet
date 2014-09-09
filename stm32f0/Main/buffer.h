#ifndef __BUFFER_H
#define __BUFFER_H

typedef struct{
	char array[512];
	unsigned int end;
} BufferType;

void fillBuffer(BufferType *buf, char ch);
int pushToSerialBuffer(char ch);
void bufferReset(BufferType *buf);

extern BufferType serialBuffer;

#endif
