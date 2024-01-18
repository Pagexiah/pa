#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>


#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  //panic("Not implemented");
  char out[9216]={'\0'};
  //va_list ap;
  //sprintf(out,fmt,...);
  va_list ap;
  int ret=-1;
  va_start(ap,fmt);
  ret=vsprintf(out,fmt,ap);
  va_end(ap);
  int i=0;
  while(out[i]!='\0')
  {
    putch(out[i]);
    i++;
  }
  return ret;
}
/*
int vsprintf(char *out, const char *fmt, va_list ap) {
  size_t n=0;
  size_t m=0;
  int d;
  while(1){
    if (fmt[n]=='\0'){out[m]='\0';break;return m;}
    if (fmt[n]!='%'){
      out[m]=fmt[n];
      m++;n++;}
    if (fmt[n]=='%'){
      n++; 
      switch(fmt[n]){
        case 's': 
          char *s = va_arg(ap, char *);
          //for(int i=0;i<strlen(ap);i++){
            //out[m]=s[i];m++;}
          //ap++;
          memcpy(&out[m],s,strlen(s));
          m+=strlen(s);
          break;
        case 'd':        
          d=va_arg(ap,int);
          if(d<0){out[m]='-';d=-d;m++;}
          int a[100]={0};
          int b=0;
          while(d>0)
          { 
            a[b]=d%10;
            d=d/10;
            b++;
          }
          b--;
          while(b>=0){
            out[m]='0'+a[b];
            b--;
            m++;}
         
          break;
        //int 返回值?  
        case'%':
            out[m]='%';
            m++;
        case 'c':
          int c = va_arg(ap, int);
          //for(int i=0;i<strlen(ap);i++){
            //out[m]=s[i];m++;}
          //ap++;
          memcpy(&out[m],(char *)c,1);
          m++;
          break;   
    }
    n++;
  }
  //panic("Not implemented");
  //out[m+1]='\0';
  //return 1;
 
  } 
return m;
}
*/
///*
void reverse(char* s,int i){
  char *e=s+i-1;
  char t;
  while(s<e){
    t=*s;
    *s=*e;
    *e=t;
    ++s;
    --e;
    //putch(*e);
  }
}
static int itoa(int n, char *s, int base){
  int i=0;
  //int sign=n;
  unsigned long int nn=0;
  if(n<0) {nn=(unsigned long int)-n;}else{nn=n;}
  
  while(nn>0){
    s[i++]='0'+nn%base;
    nn=nn/base;
  }
  if(n<0)s[i++]='-';
  if(n==0){s[i++]='0';}
  s[i]='\0';
  if(i==1){return i;}
  reverse(s,i);
  return i;
}

int vsprintf(char *out,const char *fmt,va_list ap){
  char *start=out;
  while(*fmt!='\0'){
    if(*fmt !='%'){*out=*fmt;++out;}
    else{
      switch(*(++fmt)){
        case 's': 
          char *s = va_arg(ap, char *);
          //memcpy(&out,s,strlen(s));
          strcpy(out,s);
          out+=strlen(out);
          break;
        case 'c':
          char c=va_arg(ap,int);
          *out=c;
          out+=1;
          break;
        case 'd':
          out+=itoa(va_arg(ap,int),out,10);
          break;
          
      }
    }
    ++fmt;
  }
  *out='\0';
  return out-start;
  

}
//*/

int sprintf(char *out, const char *fmt, ...) {
   va_list ap;
   int ret=-1;
   va_start(ap,fmt);
   ret=vsprintf(out,fmt,ap);
   va_end(ap);
   return ret;
  //panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
