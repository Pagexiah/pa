#include <NDL.h>
#include <SDL.h>
#include<stdlib.h>
#include<string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char *buf=(char *)malloc(32*sizeof(char));
  memset(buf,0,32);
  if(NDL_PollEvent(buf,32)==1){
   if(strncmp(buf,"ku",2)==0) ev->key.type=SDL_KEYUP;
   if(strncmp(buf,"kd",2)==0) ev->key.type=SDL_KEYDOWN;
   //printf("buf :%s\n",buf);
   for(int i=0;i<sizeof(keyname)/sizeof(keyname[0]);i++){
     //printf("%s %s %d\n",buf+3,keyname[i],strlen(buf)-4);
     if(strncmp(buf+3,keyname[i],strlen(buf)-4)==0 && strlen(buf)-4==strlen(keyname[i])){
       ev->key.keysym.sym=i;
       break;
     }
   }free(buf);
   printf("type %d key %d\n",ev->key.type,ev->key.keysym.sym);
   return 1;
  }
  free(buf);
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  while(1){
    if(SDL_PollEvent(event)){ 
      printf("type %d key %d\n",event->key.type,event->key.keysym.sym)
    break;}
  }
  //SDL_PollEvent(event);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
