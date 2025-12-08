#ifndef _KERNEL_STDINT_H
#define _KERNEL_STDINT_H

/* Minimal stdint.h for 32-bit freestanding build */

typedef unsigned char       uint8_t;
typedef signed char         int8_t;
typedef unsigned short      uint16_t;
typedef signed short        int16_t;
typedef unsigned int        uint32_t;
typedef signed int          int32_t;
typedef unsigned long long  uint64_t;
typedef signed long long    int64_t;

/* pointer-sized unsigned integer for 32-bit */
typedef unsigned int        uintptr_t;

#endif /* _KERNEL_STDINT_H */
