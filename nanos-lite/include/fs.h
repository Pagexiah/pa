#ifndef __FS_H__
#define __FS_H__

#include <common.h>

#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif
size_t fs_open(const char *,int ,int );
size_t fs_write(int ,const void* ,size_t );
size_t fs_read(int ,void* , size_t );
size_t fs_close(int );
size_t fs_lseek(int ,size_t , int );
#endif
