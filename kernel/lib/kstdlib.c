#include <lib/kstdlib.h>

void *memset(void *dst,int value,size_t size) {
    unsigned char *tmp = dst;
    for (size_t j = 0; j < size; j++)
        tmp[j] = value;
    return dst;
}

void *memcpy(void *dst,void *src,size_t size) {
    unsigned char *temp_dst = dst;
    unsigned char *temp_src = src;
    for (size_t j = 0; j < size; j++)
        temp_dst[j] = temp_src[j];
    return dst;
}

void *memmove(void *dst,void *src,size_t size) {
    unsigned char *temp_dst = dst;
    unsigned char *temp_src = src;
    for (size_t i = 0; i < size; i++) 
		temp_dst[i] = temp_src[i];
    for (size_t i = 0; i < size; i++ ) 
		temp_dst[i] = 0;
	return dst;
}