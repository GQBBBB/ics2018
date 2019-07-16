#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
static bool wp_flag = false;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
  // 判断是否初始化过
  wp_flag = true;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(char *str){
    if(!wp_flag){
	    init_wp_pool();
		wp_flag = true;
		Log("init watchpoint pool...");
	}
    if(free_ == NULL){
	    panic("Error:no remaining watchpoints!");
	}

	WP *new = NULL;
	new = free_;
	free_ = free_->next;

	strncpy(new->expr, str, strlen(str) + 1);
	new->type = "watchpoint";
	bool success = true;
    uint32_t result = expr(str, &success);
	if (success)
		new->value = result;
	else
		panic("Error:表达式出错！\n");

	new->next = NULL;
	new->flag = true;
	if(head == NULL) 
		head = new;
	else{
		new->next = head;
	    head = new;
	}
	printf("No.%d %s %s at 0x%08x\n", new->NO, new->type, new->expr, new->value);
	return new;
}

void free_wp(int n){
    WP *p1 = head->next;
	WP *p2 = head;
    if(head == NULL){
	    panic("Error:no watchpoint!");
    }else if(head->NO == n){
	    head = head->next;
		p2->expr = NULL;
        p2->value = 0;
        p2->flag = false; 
		p2->type = NULL;
        p2->next = free_;
        free_ = p2;
		return;
	}else{
        while(p1 != NULL){
           if(p1->NO == n){
		       p1->expr = NULL;
               p1->value = 0;
               p1->flag = false;
			   p1->type = NULL;
			   p2->next = p1->next;
               p1->next = free_;
			   free_ = p1;
               printf("监视点 %d 已删除!\n", n);
               return;
           }else{
			   p2 = p1;
               p1 = p1->next;
           }
	   }
   }
   printf("监视点 %d 不存在!\n",n);
   return;
}

void print_wp(){
     if(head == NULL){
		  printf("不存在监视点!\n");
		  return;
	  }
	  WP *p = head;
	  while(p != NULL){
		  printf("No.%d %s %s at 0x%08x\n", p->NO, p->type, p->expr, p->value);
		  p = p->next;
	  }
	  return;
}
