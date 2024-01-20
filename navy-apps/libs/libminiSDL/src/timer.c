#include <NDL.h>
#include <sdl-timer.h>
#include <stdio.h>

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
  return 1;
}

uint32_t SDL_GetTicks() {
  struct timeval cur;
  gettimeofday(&cur,NULL);
  return cur.tv_sec*1000+cur.tv_usec/1000;
  //return 0;
}

void SDL_Delay(uint32_t ms) {
}
