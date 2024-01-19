#include <assert.h>
#include <stdio.h>
#include<sys/time.h>
/*
int main() {
  struct timeval old;
  assert(gettimeofday(&old,NULL)==0);
  struct timeval cur;
  int loop=1;
  while (1) {
   assert(gettimeofday(&cur,NULL)==0);
   uint64_t gap=(cur.tv_sec-old.tv_sec)*1000000+(cur.tv_usec-old.tv_usec);
   if(gap>500000*loop){
     printf("Hello %d times\n",loop);
     loop++;
   }
  }
  return 0;
}
*/
int main() {
  uint64_t old=NDL_GetTicks()*1000;
  int loop=1;
  while (1) {
   uint64_t gap=NDL_GetTicks()*1000-old;
   if(gap>5*loop){
     printf("Hello %d times\n",loop);
     loop++;
   }
  }
  return 0;
}
