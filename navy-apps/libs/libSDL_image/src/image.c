#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  FILE * fp=fopen(filename,"r");
  if(fp==NULL) {printf("fail to open file");return NULL;}
  //printf("File %s\n",filename);
  fseek(fp,0,SEEK_END);
  long size=ftell(fp);
  //printf("size %ld\n",size);
  fseek(fp,0,SEEK_SET);
  char * buf=(char *)malloc(size*sizeof(char));
  fread(buf,1,sizeof(buf),fp);
  SDL_Surface  *s=STBIMG_LoadFromMemory(buf,size);
  //printf("suc");
  fclose(fp);
  free(buf);
  printf("NULL? %d\n",s==NULL);
  return s;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
