#include <common.h>
#include "syscall.h"

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
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  printf("sys:%d\n",c->GPR1);
  switch (a[0]) {
    case 1: printf("SYS_yield\n");c->GPRx=0;yield();break;
    case 0:printf("SYS_exit\n");c->GPRx=0;halt(c->GPR2);break; 
    case 4:printf("SYS_write\n");sys_write((int)c->GPR2,(void *)c->GPR3,(size_t)c->GPR4);break;
    case 9:printf("SYS_brk\n");c->GPRx=0;break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
