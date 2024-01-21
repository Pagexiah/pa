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
  char buf[32];//=( char *)malloc(32*sizeof(char));
  //memset(buf,0,32);
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
   }//free(buf);
   printf("type %d key %d\n",ev->key.type,ev->key.keysym.sym);
   return 1;
  }
  //free(buf);
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
/*
  while(SDL_PollEvent(event)==0);
  //{
    //if(SDL_PollEvent(event)){ 
      //printf("type %d key %d\n",event->key.type,event->key.keysym.sym);
    //break;}
  //}
  //SDL_PollEvent(event);
  return 1;
  */
   unsigned buf_size = 32;
  char buf[32];
  if (NDL_PollEvent(buf, buf_size)) {
      if (strncmp(buf, "kd", 2) == 0) {event->key.type = SDL_KEYDOWN;}
      if (strncmp(buf, "ku", 2) == 0) {event->key.type = SDL_KEYUP;}
      for (int i = sizeof(keyname) / sizeof(keyname[0])-1;i>=0; i--) {
          if (strncmp(buf + 3, keyname[i] , strlen(buf)-4) == 0&& strlen(keyname[i]) == strlen(buf) - 4) {
              event->key.keysym.sym = i;
              break;
          }
      }
  }
  else  {event->key.type = SDL_USEREVENT; event->key.keysym.sym = 0;}
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  
  return 0;
}
//unsigned char keystate[sizeof(keyname) / sizeof(keyname[0])];
uint8_t* SDL_GetKeyState(int *numkeys) {
/*
  SDL_Event event;
  memset(keystate, 0, sizeof(keystate));
  for(int i=0;i<=1000;i++){
  if (SDL_PollEvent(&event) == 1 && event.key.type == SDL_KEYDOWN) {
        keystate[event.key.keysym.sym] = 1;
    }
  }
  return keystate;*/
  return NULL;
}
