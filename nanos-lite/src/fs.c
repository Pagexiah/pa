#include <fs.h>
//pa3
#include<proc.h>
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;//pa3 cur 读写位置
} Finfo;
#define f_num 100  //
#define s_num 6
enum {FD_STDIN, FD_STDOUT, FD_STDERR,DEV_EVENTS,PROC_DISPINFO,FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [DEV_EVENTS] = {"/dev/events",0,0,events_read,invalid_write},
  [PROC_DISPINFO]={"/proc/dispinfo",0,0,dispinfo_read,invalid_write},
  [FD_FB]={"/dev/fb",0,0,invalid_read,fb_write},
#include "files.h"
};
size_t fs_open(const char *pathname,int flags,int mode){
  //简化实现，flags and mode 可忽视
  for(int i=3;i<f_num;i++){
    
    if(strcmp(file_table[i].name,pathname)==0){
      file_table[i].open_offset=0;
      //printf("%s %s\n",file_table[i].name,pathname);
      return i;
    }
  }
  panic("Not find file %s\n",pathname);
  }
  
  
  size_t fs_read(int fd,void* buf, size_t len){
    if(file_table[fd].read!=NULL ) return file_table[fd].read(buf,0,len);
    if(fd<3 || fd>=f_num) {
      printf("read wrong fd %d\n",fd);
      return 0;
    }
    if(file_table[fd].open_offset>file_table[fd].size){
      printf("open_offset larger than size\n");
      return 0;
    }
    size_t read_len=len;
    if(file_table[fd].open_offset+len>file_table[fd].size){
      read_len=file_table[fd].size-file_table[fd].open_offset;
    }
    //if(fd==4) file_table[fd].read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,read_len);
    //else 
    ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,read_len);
    file_table[fd].open_offset+=read_len;
    return read_len;
  }
  size_t fs_write(int fd,const void* buf,size_t len){
    if(file_table[fd].write!=NULL && fd!=5) return file_table[fd].write(buf,0,len);
    //keshenglue
    if(fd==0 || fd>=f_num) {
      printf("write wrong fd 0\n");
      return 0;
    }
    if(fd==1 ||fd==2){
      for(int i=0;i<len;i++)
      putch(*((char *)buf+i));
      return len;
    }
    //end
     if(file_table[fd].open_offset>file_table[fd].size){
      printf("open_offset larger than size\n");
      return 0;
    }
    size_t write_len=len;
    if(file_table[fd].open_offset+len>file_table[fd].size){
      write_len=file_table[fd].size-file_table[fd].open_offset;
    }
    if(fd==5) file_table[fd].write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,write_len);
    else ramdisk_write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,write_len);
    file_table[fd].open_offset+=write_len;
    return write_len;
  }
  size_t fs_close(int fd){return 0;}
  size_t fs_lseek(int fd,int offset, int whence){
    if(fd<3|| fd>=f_num){
      printf("lseek wrong fd %d\n",fd);
      return 0;
    }
    size_t new;
    //printf("%d\n",offset);
    if(whence==0){
      new=offset;
    }
    else if(whence==1){
      new=file_table[fd].open_offset+offset;
    }
    else if(whence==2){
      new=file_table[fd].size+offset;
    }
    else{
      printf("wrong whence %d\n",whence);
      return -1;
    }
    if(new<0 || new >file_table[fd].size){
      printf("wrong offset %d %d\n",new,file_table[fd].size);
      return -1;
    }
    file_table[fd].open_offset=new;
    return new;
  }
void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T ev = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = ev.width * ev.height * sizeof(uint32_t);
 
}
