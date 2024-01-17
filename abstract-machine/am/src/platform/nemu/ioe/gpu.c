#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t screen=inl(VGACTL_ADDR);
  uint32_t h=screen & 0xffff;
  uint32_t w=screen >>16;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x=ctl->x,y=ctl->y,h=ctl->h,w=ctl->w;
  if(!ctl->sync && (w==0||h==0))return ;
  uint32_t *pix=ctl->pixels;
  uint32_t *fb=(uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t sw=inl(VGACTL_ADDR)>>16;
  for(int i=y;i<y+h;i++){
    for(int j=x;j<x+w;j++){
      fb[sw*i+j]=pix[w*(i-y)+j-x];
      }
    }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
