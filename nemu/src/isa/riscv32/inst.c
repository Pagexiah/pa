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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
  TYPE_I, TYPE_U, TYPE_S, TYPE_J, TYPE_R,TYPE_B,
  TYPE_N, // none
  
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)

//new immtype
#define immJ() do { *imm = SEXT(((BITS(i,31,31)<<20)+(BITS(i,19,12)<<13>>1)+(BITS(i,20,20)<<20>>9)+(BITS(i,30,21)<<11>>10)),21); } while(0)
#define immB() do { *imm = SEXT((BITS(i,31,31)<<12)+(BITS(i,7,7)<<12>>1)+(BITS(i,30,25)<<7>>2)+(BITS(i,11,8)<<9>>8),13);} while(0)
//end

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J: 		   immJ(); break;
    case TYPE_R: src1R(); src2R();       ; break;
    case TYPE_B: src1R(); src2R(); immB(); break;
  }
}
//sheng ming han shu
void record_func(Decode*);

static int decode_exec(Decode *s) {
  int rd = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;//需要维护，不一定是snpc.pa2 可以在INSTPAT里实现
  
 //new define
#define jal(s) do {s->dnpc=s->pc+imm;R(rd)=s->pc+4;} while(0)
#define jalr(s) do {R(rd)=s->pc+4;s->dnpc=src1+imm;} while(0)
#define beq(s) do {if(src1==src2){s->dnpc=s->pc+imm;};/*printf("src1:%x, src2:%x\n",src1,src2);*/} while(0)
#define bne(s) do {if(src1!=src2){s->dnpc=s->pc+imm;};/* printf("src1:%x, src2:%x\n",src1,src2);printf("%x\n",s->dnpc);*/} while(0) 
#define sltu() do {if((uint32_t)src1<(uint32_t)src2){R(rd)=1;}else{R(rd)=0;} } while(0)//有隐患，因为强制转换了类型。要改。
#define bge(s) do {if((int32_t)src1>=(int32_t)src2){s->dnpc=s->pc+imm;};/*printf("src1:%d, src2:%d\n",src1,src2);*/} while(0)//大于等于！！！！！！！！！
//end

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm;/*printf("pc:%x, rd:%x, imm:%x\n",s->pc,R(rd),imm)*/);
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));

  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  //from this new add
  //dummy.c
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm;/*printf("src1:%x, imm:%d, src1+imm:%x\n",src1,imm,R(rd));*/);//src1?
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, jal(s););//record_func(s));//dnpc?
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1+imm,4,src2);/*printf("src1:%x, src2:%x, imm:%d \n",src1,src2,imm)*/);//sb是存入低八位
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, jalr(s););//record_func(s));
  //add.c
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd)=Mr(src1+imm,4);/*printf("src1:%x, imm:%d, rd:%x\n",src1,imm,R(rd));*/);
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R, R(rd)=src1+src2);
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R, R(rd)=src1-src2);
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, R(rd)=(uint32_t)src1<imm?1:0);//要考虑无符号数吗？？
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, beq(s));//目前跳转正确
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, bne(s));//目前跳转正确
  //add-longlong
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, sltu());
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R, R(rd)=src1 ^ src2);//异或
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R, R(rd)=src1 | src2);//或
  //bit.c
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1+imm,2,src2));
  INSTPAT("0100000 ????? ????? 101 ????? 00100 11", srai   , I, R(rd)=(int32_t)src1>>imm);//算术移位//在运行movsx时debug 添加了(int32_t)
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd)=src1 & imm);
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R, R(rd)=src1<<src2);
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R, R(rd)=src1 & src2);
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd)=src1 ^ imm);
  //bubble-sort.c //无限循环了？  解决了bne 写错了
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, bge(s));
  //crc32.c
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd)=imm);
  INSTPAT("0000000 ????? ????? 101 ????? 00100 11", srli   , I, R(rd)=(uint32_t)src1>>imm);//逻辑移位 ，强制转换会有影响吗？
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, if((uint32_t)src1>=(uint32_t)src2){s->dnpc=s->pc+imm;});
  INSTPAT("0000000 ????? ????? 001 ????? 00100 11", slli   , I, R(rd)=(uint32_t)src1<<imm);//逻辑左移 与 算术左移 同  //11.7 uint32_t
  //div.c
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R, R(rd)=src1*src2);
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R, R(rd)=(int32_t)src1/(int32_t)src2);//R(rd)=SEXT(src1,32)/SEXT(src2,32));//HIT GOOD 不知道后面会不会爆雷。主要是SEXT的使用。
  //goldbach.c
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R, R(rd)=(int32_t)src1%(int32_t)src2);//SEXT(src1,32)%SEXT(src2,32));//HIT GOOD
  //if-else.c
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, if((int32_t)src1<(int32_t)src2){s->dnpc=s->pc+imm;});//小于
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, if((int32_t)src1<(int32_t)src2){R(rd)=1;}else{R(rd)=0;});
  //load-store.c
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", lh     , I, R(rd)=SEXT(Mr(src1+imm,2),16));//但是默认Mr的结果是无符号啊，或许SEXT 比我们想象中强大。
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", lhu    , I, R(rd)=Mr(src1+imm,2));
  //mersenne.c
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   , R, int64_t h=(((int64_t)src1)<<32>>32)*(((int64_t)src2)<<32>>32);R(rd)=h>>32;/*printf("src1:%lx, src2:%lx, rd:%x, h:%lx \n",((int64_t)src1)<<32>>32,(int64_t)src2,R(rd),h)*/);//单纯的int64_t会直接零扩展
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   , R, R(rd)=(uint32_t)src1%(uint32_t)src2);//M指令集的符号扩展这方面都稀里糊涂的！！！！！！！！！要再看看！！！！！
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   , R, R(rd)=(uint32_t)src1/(uint32_t)src2);
  //mul-longlong.c
    //debug mulh
  //shift.c
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R, R(rd)=(int32_t)src1>>src2);
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", srl    , R, R(rd)=(uint32_t)src1>>src2);
  //switch.c
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, if((uint32_t)src1<(uint32_t)src2){s->dnpc=s->pc+imm;});
  //to-lower-case.c
    //after finish above ok! smile
  //alu-test
  INSTPAT("0000001 ????? ????? 011 ????? 01100 11", mulhu  , R, uint64_t h=(((uint64_t)src1)<<32>>32)*(((uint64_t)src2)<<32>>32);R(rd)=h>>32;);  
  INSTPAT("??????? ????? ????? 000 ????? 00000 11", lb     , I, R(rd)=SEXT(Mr(src1+imm,1),8));
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(rd)=src1|imm);
  INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti   , I, if((int32_t)src1<(int32_t)imm){R(rd)=1;}else{R(rd)=0;});
  //INSTPAT("??????? ????? ????? 001 ????? 11100 11", csrw  , I, );
  //end
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();
  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}


#ifdef CONFIG_FTRACE
#include<isa.h>

//ftrace record the func
Func* find_func_by_pc(uint32_t pc_start){
  for(int i=0;i<funcs_num;i++){
    if(elf_func[i].func_start<=pc_start && pc_start<=elf_func[i].func_end)
    {
      //printf("find func when running, func_name:%s\n",elf_func[i].func_name);
      return &elf_func[i]; 
    }
  }
  return NULL;
}



void record_func(Decode *s){
  uint32_t i = s->isa.inst.val;
  int rd=BITS(i,11,7);
  int rs1 = BITS(i, 19, 15);
  Func *cur=find_func_by_pc(s->pc);
  Func *fut=find_func_by_pc(s->dnpc);
  //if(cur==NULL){printf("pc:%x  Not func call or ret\n",s->pc);}
  //if(fut==NULL){printf("dnpc:%x  Not func call or ret\n",s->dnpc);}
  //if(find_func_by_pc(s->pc)==find_func_by_pc(s->dnpc) && rd==1){return;}
  Funcs *new=(Funcs *)malloc(sizeof(Funcs));
  new->cur_ad=cur;
  new->fut_ad=fut;
  new->pc_addr=s->pc;
  if(rd==0 && rs1==1){
    new->type=Ftrace_ret;
    //Func *fut2=find_func_by_pc(R(rs1));
    //new->fut_ad=fut2;
    }
  if(rd==1){new->type=Ftrace_call;}
  new->next=NULL;
  if(func_head==NULL){func_head=new;}
  else{
    Funcs *p=func_head;
    while(p->next!=NULL){
      p=p->next;
    }
    p->next=new;
  }
  return;
}
#endif







