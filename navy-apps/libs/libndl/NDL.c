#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<assert.h>
static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w =0,canvas_h=0;
static int canvas_x=0,canvas_y=0;
uint32_t NDL_GetTicks() {
  struct timeval cur;
  gettimeofday(&cur,NULL);
  return cur.tv_sec*1000+cur.tv_usec/1000;
}

int NDL_PollEvent(char *buf, int len) {
  //memset(buf,0,len);
  int fd=open("/dev/events",0,0);
  int ret=read(fd,buf,len);
  close(fd);
  if(ret==0) return 0;
  else return 1;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  
  int buf_size = 1024; 
  char * buf = (char *)malloc(buf_size * sizeof(char));
  int fd = open("/proc/dispinfo", 0, 0);
  int ret = read(fd, buf, buf_size);
   if(close(fd)!=0){printf("Can't close\n");assert(0);}
   if(ret>buf_size){printf("read wrong\n");assert(0);}
  int i = 0;
  int width = 0, height = 0;
  printf("buf %s\n",buf);
  assert(strncmp(buf + i, "WIDTH", 5) == 0);
  i += 5;
  for (; i < buf_size; i++) {
      if (buf[i] == ':') { i++; break; }
      //assert(buf[i] == ' ');
  }
  for (; i < buf_size; i++) {
      if (buf[i] >= '0' && buf[i] <= '9') break;
      //assert(buf[i] == ' ');
  }
  for (; i < buf_size; i++) {
      if (buf[i] >= '0' && buf[i] <= '9') width = width * 10 + buf[i] - '0';
      else break;
  }
  assert(buf[i++] == '\n');
  assert(strncmp(buf + i, "HEIGHT", 6) == 0);
  i += 6;
  for (; i < buf_size; i++) {
      if (buf[i] == ':') { i++; break; }
      assert(buf[i] == ' ');
  }
  for (; i < buf_size; ++i) {
      if (buf[i] >= '0' && buf[i] <= '9') break;
      assert(buf[i] == ' ');
  }
  for (; i < buf_size; ++i) {
      if (buf[i] >= '0' && buf[i] <= '9') height = height * 10 + buf[i] - '0';
      else break;
  } 
  free(buf);
  screen_w = width;
  screen_h = height;
if (*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
  }
  canvas_w = *w;
  canvas_h = *h;
  canvas_x=(screen_w - canvas_w) / 2;
  canvas_y=(screen_h - canvas_h) / 2;
  //printf("canvas_w %d canvas_h %d canvas_x %d canvas_y%d \n",canvas_w,canvas_h,canvas_x,canvas_y);
}
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  //printf("NDL_DrawRect\n");
  //printf("x %d y %d w %d h %d\n",x,y,w,h);
 
  int fd=open("/dev/fb",0,0);
  for (int i=0;i<h &&i<canvas_h-y;i++){
    lseek(fd,(x+canvas_x+(y+canvas_y+i)*screen_w)*4,0);\
    //printf("seek suc %d\n",i);
    if(w+x<canvas_w) write(fd,i*w+pixels,w*4);
    else write(fd,i*w+pixels,(canvas_w-x)*4);
   // printf("write suc %d\n",i);
  }
  //if(close(fd)!=0){printf("Can't close\n");assert(0);}
  close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
