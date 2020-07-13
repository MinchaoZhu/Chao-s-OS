#ifndef KERNEL_KERNEL_H_
#define KERNEL_KERNEL_H_
#include <stddef.h>

#define container_of(ptr, type, member) ({ \
     const typeof( ((type *)0)->member ) *__mptr = (ptr); \
     (type *)( (char *)__mptr - offsetof(type,member) );})   




#endif // KERNEL_KERNEL_H_