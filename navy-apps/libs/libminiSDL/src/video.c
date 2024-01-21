#include <NDL.h>
#include <sdl-video.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include<stdio.h>
void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
/*
  assert(dst && src);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  printf("BLIT bits %d, 8? %d\n",dst->format->BitsPerPixel,dst->format->palette==NULL);
  int d_x,d_y;
  if(dst->format->palette==NULL){
    uint32_t * d=(uint32_t *)dst->pixels;
    uint32_t * s=(uint32_t *)src->pixels;
    if(dstrect==NULL){
    d_x=0;
    d_y=0;
  }
  else{
    d_x=dstrect->x;
    d_y=dstrect->y;
  }
  if(srcrect!=NULL){
    int w = srcrect->w >= (dst->w - d_x) ? (dst->w - d_x):srcrect->w  ;
    int h = srcrect->h >= (dst->h - d_y) ?  (dst->h - d_y):srcrect->h ;
    for(int i=0;i<h;i++){
      for(int j=0;j<w;j++){
        d[d_x+j+dst->w*(i+d_y)]= s[srcrect->x+j+src->w*(i+srcrect->y)];
      }
    }
  }
  else{
    int w = src->w >= (dst->w - d_x) ? (dst->w - d_x):src->w  ;
    int h = src->h >= (dst->h - d_y) ?  (dst->h - d_y):src->h ;
    for(int i=0;i<h;i++){
      for(int j=0;j<w;j++){
         d[d_x+j+dst->w*(i+d_y)]= s[j+src->w*i];
      }
    }
  }
  }
  else{
    uint8_t *d=(uint8_t *)dst->pixels;
    uint8_t *s=(uint8_t *)src->pixels; 
     if(dstrect==NULL){
    d_x=0;
    d_y=0;
  }
  else{
    d_x=dstrect->x;
    d_y=dstrect->y;
  }
  if(srcrect!=NULL){
    int w = srcrect->w >= (dst->w - d_x) ? (dst->w - d_x):srcrect->w  ;
    int h = srcrect->h >= (dst->h - d_y) ?  (dst->h - d_y):srcrect->h ;
    for(int i=0;i<h;i++){
      for(int j=0;j<w;j++){
        d[d_x+j+dst->w*(i+d_y)]= s[srcrect->x+j+src->w*(i+srcrect->y)];
      }
    }
  }
  else{
    int w = src->w >= (dst->w - d_x) ? (dst->w - d_x):src->w  ;
    int h = src->h >= (dst->h - d_y) ?  (dst->h - d_y):src->h ;
    for(int i=0;i<h;i++){
      for(int j=0;j<w;j++){
         d[d_x+j+dst->w*(i+d_y)]= s[j+src->w*i];
      }
    }
  }
  }
  */
   assert(dst && src);
    assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
    int src_w = src->w;
    int src_h = src->h;
    int dst_w = dst->w;
    int dst_h = dst->h;
    int dstrect_x = dstrect?dstrect->x:0;
    int dstrect_y = dstrect?dstrect->y:0;
    if (src->format->BytesPerPixel!=1){
            uint32_t *base = (uint32_t *)dst->pixels;
            uint32_t *data = (uint32_t *)src->pixels;
	    if (srcrect == NULL) {
	    int copyWidth = src_w;
	    int copyHeight = src_h;

	       for (int i = 0; i < copyHeight; ++i) {
		   int srcIndex = i * src_w;
		   int dstIndex = (dstrect_y + i) * dst_w + dstrect_x;
		   memcpy(&base[dstIndex], &data[srcIndex], copyWidth * sizeof(uint32_t));
		   }
	       return;
	    }
	    else {
		 int srcrect_x = srcrect->x;
		 int srcrect_y = srcrect->y;
		 int width = srcrect->w;
		 int height = srcrect->h;
		 for (int i = 0; i < height; ++i) {
		   int srcIndex = (srcrect_y + i) * src_w + srcrect_x;
		   int dstIndex = (dstrect_y + i) * dst_w + dstrect_x;
		   memcpy(&base[dstIndex], &data[srcIndex], width * sizeof(uint32_t));
		 }
		 return;
	    }
    }else {
            int srcrect_x=0;int srcrect_y=0;int srcrect_w=src_w;int srcrect_h=src_h;
            int dstrect_x=0;int dstrect_y=0;int dstrect_w=dst_w;int dstrect_h=dst_h;
	    if(srcrect!=NULL){
	          srcrect_x = srcrect->x;
		  srcrect_y = srcrect->y;
		  srcrect_w = srcrect->w;
		  srcrect_h = srcrect->h;
		  }
	    if (dstrect != NULL) {
	          dstrect_x = dstrect->x;
		  dstrect_y = dstrect->y;
		  dstrect_w = dstrect->w; 
		  dstrect_h = dstrect->h;
		  }
            uint8_t *src_index=src->pixels+srcrect_y*src->pitch+srcrect_x*src->format->BytesPerPixel;
            uint8_t *dst_index=dst->pixels+dstrect_y*dst->pitch+dstrect_x*dst->format->BytesPerPixel;
		for (int i = 0; i < srcrect_h; ++i) {
                 memcpy(dst_index, src_index, srcrect_w*src->format->BytesPerPixel);
                 dst_index+=dst->pitch;
                 src_index+=src->pitch;
		}
		if (dstrect != NULL) {
		  dstrect->w = srcrect_w; 
		  dstrect->h = srcrect_h;
		  }
	}
  
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {
/*
   printf("FILL 8? %d\n",dst->format->palette==NULL);
  if(dst->format->palette==NULL)
    {
     uint32_t *dp=(uint32_t *)dst->pixels;
     if(dstrect==NULL) {
    for(int i=0;i<dst->h;i++){
      for (int j=0;j<dst->w;j++){
         dp[j+i*dst->w]=color;
      }
    }
  }
  else{
    int w = dstrect->w >= (dst->w - dstrect->x) ? (dst->w - dstrect->x):dstrect->w  ;
    int h = dstrect->h >= (dst->h - dstrect->y) ?  (dst->h - dstrect->y):dstrect->h ;
    for(int i=0;i<h;i++){
      for(int j=0;j<w;j++){
         dp[dstrect->x+j+dst->w*(i+dstrect->y)]=color;
      }
    }
  }
    }
  else
    {
    uint8_t *dp=(uint8_t *)dst->pixels;
    SDL_Color *colors=dst->format->palette->colors;
    if(dstrect==NULL) {
    for(int i=0;i<dst->h;i++){
      for (int j=0;j<dst->w;j++){
         colors[dp[j+i*dst->w]].a=(uint8_t)(color>>24);
         colors[dp[j+i*dst->w]].r=(uint8_t)(color>>16);
         colors[dp[j+i*dst->w]].g=(uint8_t)(color>>8);
         colors[dp[j+i*dst->w]].b=(uint8_t)(color);
      }
    }
  }
  else{
    int w = dstrect->w >= (dst->w - dstrect->x) ? (dst->w - dstrect->x):dstrect->w  ;
    int h = dstrect->h >= (dst->h - dstrect->y) ?  (dst->h - dstrect->y):dstrect->h ;
    for(int i=0;i<h;i++){
      for(int j=0;j<w;j++){
         colors[dp[dstrect->x+j+dst->w*(i+dstrect->y)]].a=(uint8_t)(color>>24);
         colors[dp[dstrect->x+j+dst->w*(i+dstrect->y)]].r=(uint8_t)(color>>16);
         colors[dp[dstrect->x+j+dst->w*(i+dstrect->y)]].g=(uint8_t)(color>>8);
         colors[dp[dstrect->x+j+dst->w*(i+dstrect->y)]].b=(uint8_t)(color);
      }
    }
  }
    }
 */
  int dst_w = dst->w;
    int dst_h = dst->h;
    int rect_x = 0, rect_y = 0, rect_w = dst_w, rect_h = dst_h;
    if (dstrect != NULL) {
      rect_x = dstrect->x;
      rect_y = dstrect->y;
      rect_w = dstrect->w;
      rect_h = dstrect->h;
    }
    if (dst->format->BytesPerPixel!=1){
      uint32_t *base = (uint32_t *)dst->pixels;
      for (int i = 0; i < rect_h; ++i) {
        int row_offset = (rect_y + i) * dst_w + rect_x;
        for (int j = 0; j < rect_w; ++j) {
            base[row_offset + j] = color;
        }
       }
    }
    else{
      uint8_t * pixels_index = (uint8_t *)dst->pixels;
      for (int i = 0; i < rect_h; ++i) {
        int row_offset = (rect_y + i) * dst_w + rect_x;
        for (int j = 0; j < rect_w; ++j) {
            pixels_index[row_offset+j]=color;
            }
      }
    }
 
}

void SDL_UpdateRect(SDL_Surface *s, int x, int y, int w, int h) {
/*
  printf("UP 8? %d\n",s->format->palette==NULL);
  if(s->format->palette!=NULL){
     if(s->format->palette->colors==NULL){printf("Palette and no color\n");assert(0);}
     int W=x==0&&y==0&&w==0&&h==0? s->w:w;
     int H=x==0&&y==0&&w==0&&h==0?s->h:h;
     printf("W %d H %d s->w %d s->h %d w %d h %d\n",W,H,s->w,s->h,w,h);
     printf("W*H %d\n",W*H);
        uint32_t  *pixels = (uint32_t *)malloc(W * H * sizeof(uint32_t ));
        for (int i = 0; i < W*H; i++){
            SDL_Color color = s->format->palette->colors[((uint8_t*)(s->pixels))[i]];
            pixels[i]=(color.a << 24) | (color.r << 16) | (color.g << 8) | (color.b << 0);
            
          
        }
        printf("here\n");
        NDL_DrawRect((uint32_t *)pixels, x, y, W, H);
        free(pixels);
  }
  else{
    if(x==0 &&y==0 &&w==0&&h==0){
    NDL_DrawRect((void *)(s->pixels),0,0,s->w,s->h);
    }
    else{
    NDL_DrawRect((void *)(s->pixels),x,y,w,h);
    }
  }*/
   if (x == 0 && y == 0 && w == 0 && h == 0 && s->format->BytesPerPixel!=1){
    w=s->w;h=s->h;NDL_DrawRect((uint32_t *)s->pixels, x, y, w, h);}  
    else if (s->format->BytesPerPixel!=1) {
      uint32_t * pixels = (uint32_t *)malloc(w * h * sizeof(uint32_t));
        for (int i = 0; i < h; i++) {
         for(int j = 0; j < w; j++) {
                pixels[i*w+j] = s->pixels[(y+i)*s->w+j+x];
            }
        }
        NDL_DrawRect((uint32_t *)pixels, x, y, w, h);
        free(pixels);
    }
    else{
        if (x == 0 && y == 0 && w == 0 && h == 0){w=s->w;h=s->h;}
      uint32_t * pixels = (uint32_t *)malloc(w * h * sizeof(uint32_t));
        for (int i = 0; i < h; ++i) {
         for(int j = 0; j < w; j++) {
                SDL_Color colors = s->format->palette->colors[s->pixels[(y+i)*s->w+j+x]];
                uint32_t temp = (colors.a << 24) | (colors.r << 16) | (colors.g << 8) | (colors.b << 0);
                pixels[i*w+j] = temp;
            }
        }
        NDL_DrawRect((uint32_t *)pixels, x, y, w, h);
        free(pixels);
    }
}

// APIs below are already implemented.

static inline int maskToShift(uint32_t mask) {
  switch (mask) {
    case 0x000000ff: return 0;
    case 0x0000ff00: return 8;
    case 0x00ff0000: return 16;
    case 0xff000000: return 24;
    case 0x00000000: return 24; // hack
    default: assert(0);
  }
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
    uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  assert(depth == 8 || depth == 32);
  SDL_Surface *s = malloc(sizeof(SDL_Surface));
  assert(s);
  s->flags = flags;
  s->format = malloc(sizeof(SDL_PixelFormat));
  assert(s->format);
  if (depth == 8) {
    s->format->palette = malloc(sizeof(SDL_Palette));
    assert(s->format->palette);
    s->format->palette->colors = malloc(sizeof(SDL_Color) * 256);
    assert(s->format->palette->colors);
    memset(s->format->palette->colors, 0, sizeof(SDL_Color) * 256);
    s->format->palette->ncolors = 256;
  } else {
    s->format->palette = NULL;
    s->format->Rmask = Rmask; s->format->Rshift = maskToShift(Rmask); s->format->Rloss = 0;
    s->format->Gmask = Gmask; s->format->Gshift = maskToShift(Gmask); s->format->Gloss = 0;
    s->format->Bmask = Bmask; s->format->Bshift = maskToShift(Bmask); s->format->Bloss = 0;
    s->format->Amask = Amask; s->format->Ashift = maskToShift(Amask); s->format->Aloss = 0;
  }

  s->format->BitsPerPixel = depth;
  s->format->BytesPerPixel = depth / 8;

  s->w = width;
  s->h = height;
  s->pitch = width * depth / 8;
  assert(s->pitch == width * s->format->BytesPerPixel);

  if (!(flags & SDL_PREALLOC)) {
    s->pixels = malloc(s->pitch * height);
    assert(s->pixels);
  }

  return s;
}

SDL_Surface* SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth,
    int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  SDL_Surface *s = SDL_CreateRGBSurface(SDL_PREALLOC, width, height, depth,
      Rmask, Gmask, Bmask, Amask);
  assert(pitch == s->pitch);
  s->pixels = pixels;
  return s;
}

void SDL_FreeSurface(SDL_Surface *s) {
  if (s != NULL) {
    if (s->format != NULL) {
      if (s->format->palette != NULL) {
        if (s->format->palette->colors != NULL) free(s->format->palette->colors);
        free(s->format->palette);
      }
      free(s->format);
    }
    if (s->pixels != NULL && !(s->flags & SDL_PREALLOC)) free(s->pixels);
    free(s);
  }
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
  if (flags & SDL_HWSURFACE) NDL_OpenCanvas(&width, &height);
  return SDL_CreateRGBSurface(flags, width, height, bpp,
      DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);
}

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
  assert(src && dst);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  assert(dst->format->BitsPerPixel == 8);

  int x = (srcrect == NULL ? 0 : srcrect->x);
  int y = (srcrect == NULL ? 0 : srcrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);

  assert(dstrect);
  if(w == dstrect->w && h == dstrect->h) {
    /* The source rectangle and the destination rectangle
     * are of the same size. If that is the case, there
     * is no need to stretch, just copy. */
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_BlitSurface(src, &rect, dst, dstrect);
  }
  else {
    assert(0);
  }
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, int firstcolor, int ncolors) {
  assert(s);
  assert(s->format);
  assert(s->format->palette);
  assert(firstcolor == 0);

  s->format->palette->ncolors = ncolors;
  memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

  if(s->flags & SDL_HWSURFACE) {
    assert(ncolors == 256);
    for (int i = 0; i < ncolors; i ++) {
      uint8_t r = colors[i].r;
      uint8_t g = colors[i].g;
      uint8_t b = colors[i].b;
    }
    SDL_UpdateRect(s, 0, 0, 0, 0);
  }
}

static void ConvertPixelsARGB_ABGR(void *dst, void *src, int len) {
  int i;
  uint8_t (*pdst)[4] = dst;
  uint8_t (*psrc)[4] = src;
  union {
    uint8_t val8[4];
    uint32_t val32;
  } tmp;
  int first = len & ~0xf;
  for (i = 0; i < first; i += 16) {
#define macro(i) \
    tmp.val32 = *((uint32_t *)psrc[i]); \
    *((uint32_t *)pdst[i]) = tmp.val32; \
    pdst[i][0] = tmp.val8[2]; \
    pdst[i][2] = tmp.val8[0];

    macro(i + 0); macro(i + 1); macro(i + 2); macro(i + 3);
    macro(i + 4); macro(i + 5); macro(i + 6); macro(i + 7);
    macro(i + 8); macro(i + 9); macro(i +10); macro(i +11);
    macro(i +12); macro(i +13); macro(i +14); macro(i +15);
  }

  for (; i < len; i ++) {
    macro(i);
  }
}

SDL_Surface *SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, uint32_t flags) {
  assert(src->format->BitsPerPixel == 32);
  assert(src->w * src->format->BytesPerPixel == src->pitch);
  assert(src->format->BitsPerPixel == fmt->BitsPerPixel);

  SDL_Surface* ret = SDL_CreateRGBSurface(flags, src->w, src->h, fmt->BitsPerPixel,
    fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

  assert(fmt->Gmask == src->format->Gmask);
  assert(fmt->Amask == 0 || src->format->Amask == 0 || (fmt->Amask == src->format->Amask));
  ConvertPixelsARGB_ABGR(ret->pixels, src->pixels, src->w * src->h);

  return ret;
}

uint32_t SDL_MapRGBA(SDL_PixelFormat *fmt, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  assert(fmt->BytesPerPixel == 4);
  uint32_t p = (r << fmt->Rshift) | (g << fmt->Gshift) | (b << fmt->Bshift);
  if (fmt->Amask) p |= (a << fmt->Ashift);
  return p;
}

int SDL_LockSurface(SDL_Surface *s) {
  return 0;
}

void SDL_UnlockSurface(SDL_Surface *s) {
}
