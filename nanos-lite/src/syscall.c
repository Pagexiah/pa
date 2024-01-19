#include <common.h>
#include "syscall.h"
//pa3
#include<fs.h>
#include<sys/time.h>
//
void sys_write(int fd,void* buf, size_t count){
  char *buff=(char *)buf;
  if(fd==1 ||fd==2){
    int i=0;
    while(buff[i]!='\0' && i<count){
      putch(buff[i]);
      i++;
    }
  }
  //return i;
}

//void sys_brk();
int sys_gettimeofday(struct timeval *tv){
   uint64_t us = io_read(AM_TIMER_UPTIME).us;
   tv->tv_sec=us/1000000;
   tv->tv_usec=us-us/1000000*1000000;
   return 0;
}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  printf("sys:%d\n",c->GPR1);
  switch (a[0]) {
    case 1: printf("SYS_yield\n");c->GPRx=0;yield();break;
    case 0:printf("SYS_exit\n");c->GPRx=0;halt(c->GPR2);break; 
    case 2: printf("SYS_open\n");c->GPRx=fs_open((const char*)c->GPR2,(int)c->GPR3,(size_t)c->GPR4);break;
    case 3:printf("SYS_read\n");c->GPRx=fs_read((int)c->GPR2,( void *)c->GPR3,(size_t)c->GPR4);break;
    case 4:printf("SYS_write\n");c->GPRx=fs_write((int)c->GPR2,(void *)c->GPR3,(size_t)c->GPR4);break;
    case 7:printf("SYS_close\n");c->GPRx=fs_close((int)c->GPR2);break;
    case 8:printf("SYS_lseek\n");c->GPRx=fs_lseek((int)c->GPR2,(size_t)c->GPR3,(int)c->GPR4);break;
    case 9:printf("SYS_brk\n");c->GPRx=0;break;\
    case 19:printf("SYS_gettimeofday\n");c->GPRx=sys_gettimeofday((struct timeval *)c->GPR2);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
