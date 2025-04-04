#ifndef __COMMON_H__
#define __COMMON_H__

/* Uncomment these macros to enable corresponding functionality. */
#define HAS_CTE
//#define HAS_VME
//#define MULTIPROGRAM
//#define TIME_SHARING

#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <debug.h>
void do_syscall(Context *);
size_t serial_write(const void *, size_t , size_t );
size_t events_read(void *, size_t , size_t );
size_t dispinfo_read(void* , size_t , size_t ) ;
size_t fb_write(const void* , size_t , size_t );
#endif
