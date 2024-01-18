#include <common.h>
#include "syscall.h"
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case 1: printf("SYS_yield\n");break;
    case 0:printf("SYS_exit\n");break; 
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
