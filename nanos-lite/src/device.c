#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
//offset ignored
  for(int i=0;i<len;i++){
    putch(*((char*)buf+i));
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) {
    *(char*)buf = '\0';
    return 0;
  }
  int ret = sprintf(buf, "%s %s\n", ev.keydown?"kd":"ku", keyname[ev.keycode]);
  if(ret>=len) *((char *)buf+len)='\0';
  //printf("%s\n", buf);
  return ret;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
   AM_GPU_CONFIG_T ev = io_read(AM_GPU_CONFIG);
   int ret = sprintf(buf, "WIDTH : %d\nHEIGHT : %d", ev.width, ev.height);
   if(ret>=len) *((char *)buf+len)='\0';
  //printf("%s\n", buf);
    return ret;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T ev = io_read(AM_GPU_CONFIG);
  offset=offset/4;
  len=len/4;
  io_write(AM_GPU_FBDRAW,(int)(offset-offset/ev.width*ev.width),(int)(offset/ev.width),(void *)buf,len,1,true);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
