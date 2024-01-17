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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include<math.h>
#include<memory/paddr.h>
enum {
  TK_NOTYPE = 256, //TK_EQ=0,

  /* TODO: Add more token types */
  //'+'=1,
  //'-'=2,
  //'*'=3,
  //'/'=4,
  and=1,
  eq=2,
  neq=3,
  reg=4,
  left=5,
  right=6,
  TK_INT=7,
  hex=8,

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  //{"==", TK_EQ},        // equal
  {"\\-",'-'},		//minus
  {"\\*",'*'},		//multiple
  {"\\/",'/'},		//division
  	//int in shi jin zhi
  {"\\(",left},
  {"\\)",right},
  {"\\=\\=",eq},
  {"\\!\\=",neq},
  {"\\&\\&",and},
  {"\\$+[a-zA-Z]*[0-9]*", reg},
  {"0[xX][0-9a-fA-F]+", hex},
 // point
  {"[0-9]*",TK_INT},//last or wrong;目前这个正则表达式的局限性
  
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];// yi chu??
} Token;

static Token tokens[100] __attribute__((used)) = {};//32 to 100
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        
	Token curtk;
        switch (rules[i].token_type) {
          case '+':
            curtk.type='+';
            //curtk.str='+';
            tokens[nr_token]=curtk;
            break;
          case '-':
            curtk.type='-';
            tokens[nr_token]=curtk;
            break;
          case '*':
            curtk.type='*';
            tokens[nr_token]=curtk;
            break;
          case '/':
            curtk.type='/';
            tokens[nr_token]=curtk;
            break;
          case 5:
            curtk.type=5;
            tokens[nr_token]=curtk;
            break;
          case 6:
            curtk.type=6;
            tokens[nr_token]=curtk;
            break;
          case 256:
            nr_token--;
            break;
          case 7:
            curtk.type=TK_INT;
            tokens[nr_token].type=TK_INT;
            //strcpy(curtk.str,e.substr(position-substr_len,substr_len)); //xian jie qu zai ti huan
            strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len);
            tokens[nr_token].str[substr_len]='\0';
            break;
          case 1:
            curtk.type=1;
            tokens[nr_token]=curtk;
            break;
          case 2:
            curtk.type=2;
            tokens[nr_token]=curtk;
            break;
          case 3:
            curtk.type=3;  
            tokens[nr_token]=curtk;
            break;
          case 4:
            tokens[nr_token].type=4;
            //strcpy(curtk.str,e.substr(position-substr_len,substr_len)); //xian jie qu zai ti huan
            strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len);break;
            tokens[nr_token].str[substr_len]='\0';
          case 8:
            tokens[nr_token].type=8;
            //strcpy(curtk.str,e.substr(position-substr_len,substr_len)); //xian jie qu zai ti huan
            strncpy(tokens[nr_token].str, &e[position - substr_len], substr_len);break;
            tokens[nr_token].str[substr_len]='\0';
          default: printf("no rules\n");break;
        }
	nr_token++;  //a new token is recognized.
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  nr_token--;
  return true;
}

bool check(int p,int q){
  if(tokens[p].type!=5 || tokens[q].type!=6){
    return false;
  }
  else{
    int a=0;
    for(int i=p+1;i<=q-1;i++){
      if(tokens[i].type==5){a++;}
      else if(tokens[i].type==6){
        if(a==0){
        return false;
        printf("wrong expr\n");
        assert(0);}
        else{a--;}
      }else{continue;}
      
    }
  }
  return true;
}
// type worried!!

uint32_t getreg(char str[]){
  bool flag=false;
  return isa_reg_str2val(str,&flag);
}



//uint32_t eval(int p,int q){
sword_t eval(int p,int q){
  if(p>q){
    printf("wrong expr\n");
    assert(0);
  
  }else if(p==q){
    //printf("str %s\n",tokens[p].str);
    if(tokens[p].type==7) {return atoi(tokens[p].str);}
    else if(tokens[p].type==8){
    uint32_t hex=0;
    sscanf(tokens[p].str,"%X",&hex);return hex;}
    else if(tokens[p].type==4){return getreg(tokens[p].str);}
  }
  else if(check(p,q)==true){
    //printf("check true\n");
    return eval(p+1,q-1); 
  }
  else{
    int op=p;
    int flag=11;
    int count=0;
    for(int i=p;i<=q;i++){
      if(tokens[i].type==5){
        count++;
      }else if(tokens[i].type==6){
        count--;
      }else if(tokens[i].type==1 && count==0){
        if(flag>=1){flag=1;op=i;}
      }else if((tokens[i].type==2 || tokens[i].type==3) && count==0){
        if(flag>=2){flag=3;op=i;}
      }else if((tokens[i].type=='+'||tokens[i].type=='-')&& count==0){
        if (flag>=4){flag=5;op=i;}
      }else if((tokens[i].type=='*'||tokens[i].type=='/') && count==0){
        if (flag>=6){flag=7;op=i;}
      }else if((tokens[i].type==9||tokens[i].type==10)&& count==0){
        if (flag==11){flag=10;op=i;}//yuan if(flag>=9){flag=9;op=i;}
      }else{continue;}
      
    }
    printf("op %d op_type %d\n",op,tokens[op].type);
    if (tokens[op].type==9){
      //uint32_t num = paddr_read(eval(op + 1,q), 4); printf("point %u\n",num);return num;
     sword_t num = paddr_read(eval(op + 1,q), 4); printf("point %u\n",num);return num;
      }// point 
    //!!!!
    if(tokens[op].type==10){sword_t num=0-eval(p+1,q);printf("neg %d\n",num);return num;}
    //uint32_t val1 = eval(p, op - 1);
    //uint32_t val2 = eval(op + 1, q);
    sword_t val1 = eval(p, op - 1);
    sword_t val2 = eval(op + 1, q);
    printf("val1 %u val2 %u\n",val1,val2);
    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1-val2;
      case '*': return val1*val2;
      case '/': if(val2==0){printf("division zero");assert(0);}else{return val1/val2;}
      case 1: return val1&&val2;
      case 2:return val1==val2;
      case 3:return val1!=val2;
      default: printf(" type recognized fail");//assert(0);
  
    }
  }
  return 0;
}




//word_t expr(char *e, bool *success) {
sword_t expr(char *e,bool *success){
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type <= 3 || tokens[i-1].type==5 || tokens[i-1].type =='+'|| tokens[i-1].type =='-'|| tokens[i-1].type =='*'|| tokens[i-1].type =='/'||tokens[i-1].type==10) ) {//未考虑多重指针
      tokens[i].type = 9;
    }
     if (tokens[i].type == '-' && (i == 0 || tokens[i - 1].type <= 3 || tokens[i-1].type==5 || tokens[i-1].type =='+'|| tokens[i-1].type =='-'|| tokens[i-1].type =='*'|| tokens[i-1].type =='/'||tokens[i-1].type==9||tokens[i-1].type==10) ) {
      tokens[i].type = 10;
    }
  }
   for (int i = 0; i <=nr_token; i ++) {printf("NO %d type %d str %s\n",i,tokens[i].type,tokens[i].str);}
  //get tokens and do somthing to reg and hex and point;
  /* TODO: Insert codes to evaluate the expression. */
  //uint32_t r=eval(0,nr_token);
  uint32_t r=eval(0,nr_token);//pa1 没有最终结果为负数的样例
  *success=true;
  printf("result %u\n",r);
  return r;
  
  //return 0;
}



