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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  
  /* TODO: Add more members if necessary */
  char expr[50];
  uint32_t new_val;
  uint32_t old_val;
  bool use;

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].use=false;
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
/*
WP* new_wp(char *e){
  bool flag=false;
  uint32_t r=expr(e,&flag);
  if(flag==false){printf("wrong expression");return NULL;}
  if(head==NULL){
    head=&wp_pool[0];
    strcpy(head->expr,e);//一开始设置 char*expr，直接退出程序
    head->old_val=r;
    head->next=NULL; 
    head->use=true;
    return head;}
  WP* q=head;
  while(q->next!=NULL){
   q=q->next;
  }
  WP* p=free_;
  while(p->next!=NULL){
   if(p->use==false){
    q->next=p;
    q=q->next;
    q->NO=p->NO;
    q->next=NULL;
    strcpy(q->expr,e);
    q->old_val=r;
    //printf("%d %d",q->old_val,q->NO);
    q->use=true;
    return q;
   }
  }//会影响wp_pool的next
  printf("over the largest address");
  //assert(0);
  return NULL;
}
*/
WP* new_wp(char *e){
  bool flag=false;
  uint32_t r=expr(e,&flag);
  if(flag==false){printf("wrong expression");return NULL;}
  if(free_==NULL){printf("no watchpoint avalible");return NULL;}
  else{
  WP *new=free_;
  free_=free_->next;
  new->next=head;
  head=new;
  new->use=true;
  new->old_val=r;
  strcpy(new->expr,e);
  printf("%d %s %u\n",new->NO,new->expr,new->old_val);
  return new;
  }
  
}

void free_wp(WP *wp){
  WP *p=head;
  wp->use=false;
  if(head==NULL){printf("no watchpoint in use\n");}
  else if(head->NO==wp->NO){head->use=false;WP*f=head;head=head->next;f->next=free_;free_=f;}
  else{
    while(p->next->NO!=wp->NO){p=p->next;}
    WP* q=p->next->next;//wp.next
    WP* cur=p->next;//wp
    p->next=q; 
    cur->next=free_;
    free_=cur;
  }
  
}

void display_watchpoints(){
  for(WP* q=head;q!=NULL;q=q->next){
    printf("%d %s inuse\n",q->NO,q->expr);
  }
  for(WP* p=free_;p!=NULL;p=p->next){
    if(p->use==true){
      printf("%d %s inuse\n",p->NO,p->expr);}
    else{printf("%d not in use\n",p->NO);}
  }
}
void delete_wp(int NO){
  for(int i=0;i<NR_WP;i++){
    if(NO==wp_pool[i].NO){
      if(wp_pool[i].use==false){
        printf("already free\n");}
      else{free_wp(&wp_pool[i]);}
    break;
    }
  }
}
bool compare(){
  bool flag=false;
  for(int i=0;i<NR_WP;i++){
    if(wp_pool[i].use==true){
      uint32_t r=expr(wp_pool[i].expr,&flag);
      if(wp_pool[i].old_val!=r)
        {return false;}//状态信息 这两怎么和到一起呢
    }
  }
  return true;
}



