/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>
#include <monitor/sdb.h>//pa1加的
//#include"src/monitor/sdb/sdb.h"//src/cpu/cpu-exec.c
/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 10

CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;


/*
// iringbuf start
typedef struct Buf{
  char p[128];
  struct Buf *next;
}buf;
static buf *head=NULL;
static int count_p=0;
static bool f=0;


void iringbuf(Decode *s){ 
  buf *new=(buf *)malloc(sizeof(buf));
  strcpy(new->p,s->logbuf);
  new->next=NULL;
  //printf("%s\n",new->p);
  //printf("%s\n",s->logbuf);
  if (head==NULL){head=new;count_p++;}
  else if(count_p<16 && f==0)
  {
    buf* cur=head;
    while(cur->next)
    {
      cur=cur->next;
    }
    cur->next=new;
    count_p++;
    if(count_p==16){count_p=1;f=1;}
  }
  else if(count_p<=16 && f==1)
  {
    buf* cur=head;
    int i=1;
    while(i<count_p)
    {
      cur=cur->next;
      i++;
    }
    strcpy(cur->p,s->logbuf);
    if(count_p<16){count_p++;}else{count_p=1;}
    
   }
   else{}
}

static void putiringbuf(){
  int i=1;
  if(count_p>1){count_p--;}
  else{count_p=16;}
  for(buf *q=head;q!=NULL;q=q->next)
  { 
    
    if(i!=count_p){
      printf("   %s\n",q->p);}
    else{
      printf("-->%s\n",q->p);}
    i++;
  }
}

*/
//iringbuf end  pa2.2


void device_update();

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
  if (ITRACE_COND) { log_write("%s\n", _this->logbuf); }
#endif
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
  if(compare()==false)
  {nemu_state.state=NEMU_STOP;
  printf("invoke a watcpoint\n");}
  //void iringbuf(Decode *s);
  //iringbuf(_this);
}

static void exec_once(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  isa_exec_once(s);
  cpu.pc = s->dnpc;
#ifdef CONFIG_ITRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  for (i = ilen - 1; i >= 0; i --) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;
  /*
  buf *new=(buf *)malloc(sizeof(buf));
  strcpy(new->p,s->logbuf);
  new->next=NULL;
  //printf("%s\n",new->p);
  //printf("%s\n",s->logbuf);
  if (head==NULL){head=new;count_p++;}
  else if(count_p<16 && f==0)
  {
    buf* cur=head;
    while(cur->next)
    {
      cur=cur->next;
    }
    cur->next=new;
    count_p++;
    if(count_p==16){count_p=1;f=1;}
  }
  else if(count_p<=16 && f==1)
  {
    buf* cur=head;
    int i=1;
    while(i<count_p)
    {
      cur=cur->next;
      i++;
    }
    strcpy(cur->p,s->logbuf);
    if(count_p<16){count_p++;}else{count_p=1;}
    
   }
   else{}
*/

#ifndef CONFIG_ISA_loongarch32r
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);
#else
  p[0] = '\0'; // the upstream llvm does not support loongarch32r
#endif
#endif
  
}

//iringbuf implement pa 2,2

//putftrace
#ifdef CONFIG_FTRACE
#include"isa.h"
void putftrace(){
  Funcs *p=func_head;
  int i=0;
  printf("\n");
  bool flag=0;
  for(;p!=NULL;p=p->next){
    char qq[1024];
    char *q=&qq[0];
    q+=snprintf(q,1024,FMT_WORD ":",p->pc_addr);
    if(p->type==Ftrace_call){
      if(flag==0){i=i+1;}
      for(int t=0;t<i;t++){q+=snprintf(q,1024," ");}
      q+=snprintf(q,1024, "call[%s@%x]",p->fut_ad->func_name,p->fut_ad->func_start);
      flag=0;
    }
    else{
      if(flag==1){i=i-1;}
      for(int t=0;t<i;t++){q+=snprintf(q,1024," ");}
      q+=snprintf(q,1024, "ret[%s]",p->cur_ad->func_name);
      flag=1;
    }
    printf("%s\n",qq);
  }
}
#endif

static void execute(uint64_t n) {
  //printf("%ld",n);
  Decode s;
  for (;n > 0; n --) {
    exec_once(&s, cpu.pc);
    g_nr_guest_inst ++;
    trace_and_difftest(&s, cpu.pc);
    //iringtrace
    if (nemu_state.state != NEMU_RUNNING) {
      //void putiringbuf(); 
      //putiringbuf();
#ifdef CONFIG_FTRACE
      putftrace();
#endif
      break;}
    IFDEF(CONFIG_DEVICE, device_update());
  }
}


static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

void assert_fail_msg() {
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();

  execute(n);

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;

  switch (nemu_state.state) {
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT:
      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          nemu_state.halt_pc);
      // fall through
    case NEMU_QUIT: statistic();
  }
}
