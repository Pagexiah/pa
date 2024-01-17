#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t n=0;
  while(s[n]!='\0')
  {n++;}
  return n;
  //panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
/*
  size_t n=0;
  if(dst==NULL || src==NULL)
    {return NULL;}
  while(src[n]!='\0')
  {
    dst[n]=src[n];
    n++;
  }
  dst[n]='\0';
  return dst;
 */
  assert(dst && src);
  char *ret=dst;
  while(*src!='\0'){
    *dst=*src;
    dst++;
    src++;
  }
  return ret;
  //如果目标数组 dest 不够大，而源字符串的长度又太长，可能会造成缓冲溢出的情况。
  //panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t m=0;
  if(dst==NULL || src==NULL)
    return NULL;
  while(m<n)
  { 
    dst[m]=src[m];
    m++;
    if (src[m]=='\0')
    {dst[m]='\0';break;}
  }
  return dst;
  //panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  size_t n=strlen(dst);
  size_t m=0;
  while(src[m]!='\0')
  {
    dst[n+m]=src[m];
    m++;
  }
  dst[n+m]='\0';
  return dst;
  //panic("Not implemented");
}
/*
int strcmp(const char *s1, const char *s2) {
  size_t m=0;
  while(1){
    if(*s1=='\0' && *s2=='\0'){return 0; }
    if(s1[m]=='\0' && *s2!='\0'){return -1;}
    if(s2[m]=='\0' && s1[m]!='\0'){return 1;}
    if(s1[m]>s2[m]){return 1;}
    if(s1[m]<s2[m]){return -1;}
    if(s1[m]==s2[m]){m++;} 
  }
  //panic("Not implemented");
}
*/
/*
int strcmp(const char *s1, const char *s2) {
  size_t m=0;
  while(1){
    if(*s1=='\0' && *s2=='\0'){return 0; }
    if(*s1=='\0' && *s2!='\0'){return -1;}
    if(*s2=='\0' && s1[m]!='\0'){return 1;}
    if(*s1>*s2){return 1;}
    if(*s1<*s2){return -1;}
    if(*s1==*s2){s1++;s2++;} 
  }
  //panic("Not implemented");
}
*/
int strcmp(const char*s1,const char*s2){
  //printf("s1:%ss2:%s\n",s1,s2);
 while((*s1!='\0')&&(*s1==*s2))
    {
        s1++;
        s2++;
    }
    int t;
    t=*s1-*s2;
    if(t>0){return 1;}
    if(t<0){return -1;}
    return 0;
}
int strncmp(const char *s1, const char *s2, size_t n) {
   size_t m=0;
  while(1){
    if(m==n){return 0;}
    if(s1[m]=='\0' && s2[m]=='\0'){return 0; }
    if(s1[m]=='\0' && s2[m]!='\0'){return -1;}
    if(s2[m]=='\0' && s1[m]!='\0'){return 1;}
    if(s1[m]>s2[m]){return 1;}
    if(s1[m]<s2[m]){return -1;}
    if(s1[m]==s2[m]){m++;}
    } 
  //panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  if (s == NULL || n < 0)
  {
	return NULL;
  }
  //char cc=(word_t)c;
  char *ps = (char *)s;
  while (n-->0)
  {
	*ps++ = c;
  }
  return s;
  //会溢出吗
  //panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
/*
    unsigned char tmp[n];
    memcpy(tmp,src,n);
    memcpy(dst,tmp,n);
    return dst;//效率较低，无所谓，诶，就是无所谓
  //panic("Not implemented");
  */
  assert(dst && src);
  void* ret=dst;
  if(dst<src){
    while(n--){
      *(char*)dst=*(char*)src;
      dst=(char*)dst+1;
      src=(char*)src+1;
    }
  }
  else{
    while(n--){
      *((char*)dst+n)=*((char*)src+n);
    }
  }
  return ret;
}
/*
void *memcpy(void *out, const void *in, size_t n) {
  assert(out && in);
  void* ret=out;
  if(out<in){
    while(n--){
      *(char*)out=*(char*)in;
      out=(char*)out+1;
      in=(char*)in+1;
    }
  }
  else{
    while(n--){
      *((char*)out+n)=*((char*)in+n);
    }
  }
  return ret;
}
*/
///*
void *memcpy(void *out, const void *in, size_t n) {
  if((out == NULL) || (in == NULL))     //memTo和memFrom必须有效
     return NULL;
  
  char *pout=(char *)out;
  char *pin=(char *)in;
  while(n--)
    *pout++=*pin++;
  return out;
  //panic("Not implemented");
  
  void* ret=out;
  while(n--){
    *(char*)out=*(char*)in;
    out=(char*)out+1;
    in=(char*)in+1;
  }
  return ret;
}
//*/
int memcmp(const void *s1, const void *s2, size_t n) {
  char *ps1=(char *)s1;
  char *ps2=(char *)s2;
  size_t m=0;
  while(1){
    if(m==n){return 0;}
    if(ps1[m]=='\0' && ps2[m]!='\0'){return -1;}
    if(ps2[m]=='\0' && ps1[m]!='\0'){return 1;}
    if(ps1[m]>ps2[m]){return 1;}
    if(ps1[m]<ps2[m]){return -1;}
    if(ps1[m]==ps2[m]){m++;}
    } 
  //panic("Not implemented");
}

#endif
