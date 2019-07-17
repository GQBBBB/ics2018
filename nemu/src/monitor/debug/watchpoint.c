#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "nemu.h"
#include <regex.h>

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
}

/* TODO: Implement the functionality of watchpoint */

bool matchRegex(char* userString)
{
	    const char* pattern = "\\$eip==0x[0-9]{1,8}";
	    bool result = false;
		regex_t regex;
	    int regexInit = regcomp(&regex, pattern, REG_EXTENDED);
	    if(regexInit){
		    //Error print : Compile regex failed
		}else{
			int reti = regexec(&regex, userString, 0, NULL, 0);
			if(REG_NOERROR != reti){
				//Error print: match failed! 
			}else{
				result = true;
	        }
		}
        regfree(&regex);
		return result;
}

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

    strcpy(new->expr, str);
//	printf("str address:%08x\n",*str);
    if(matchRegex(str)){
		strcpy(new->type, "breakpoint");
		char p[11] = "\0";
		strcpy(p, str + 6);
		int nvalue = 0;          
		sscanf(p, "%x", &nvalue);
		cpu.eip = nvalue;
		new->result = nvalue;
		new->value = vaddr_read(nvalue, 1);
		printf("No.%d %s %s: %d at 0x%08x\n", new->NO, new->type, new->expr, new->value, new->result);
	}else{
	    strcpy(new->type, "watchpoint");
	    bool success = true;
        uint32_t result = expr(str, &success);
	    if (success){
			new->result = result;
		    new->value = vaddr_read(result, 1);
		}else
		    panic("Error:表达式出错！\n");
        printf("No.%d %s %s: %d at 0x%08x\n", new->NO, new->type, new->expr, new->value, new->result);
	}

	new->next = NULL;
	if(head == NULL) 
		head = new;
	else{
		new->next = head;
	    head = new;
	}
	
	return new;
}

void free_wp(int n){
    WP *p1 = head->next;
	WP *p2 = head;
    if(head == NULL){
	    panic("Error:no watchpoint!");
    }else if(head->NO == n){
	    head = head->next;
//		p2->expr = "\0";
        p2->value = 0;
		p2->result = 0; 
//		p2->type = NULL;
        p2->next = free_;
        free_ = p2;
		printf("监视点 %d 已删除!\n", n);
		return;
	}else{
        while(p1 != NULL){
           if(p1->NO == n){
//		       p1->expr = "\0";
               p1->value = 0;
               p1->result = 0;
//			   p1->type = NULL;
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
		  printf("不存在监视点/断点!\n");
		  return;
	  }
	  WP *p = head;
	  while(p != NULL){
		  printf("No.%d %s %s: %d at 0x%08x\n", p->NO, p->type, p->expr, p->value, p->result);
		  p = p->next;
	  }
	  return;
}

int check_wp(){
    if(head != NULL){
	    WP *p = head;
		char des[] = "watchpoint";
	    while(p != NULL && strcmp(p->type, des) == 0){
            int new_value = vaddr_read(p->result, 1);
	        if (new_value != p->value){
				printf("触发监视点 %d :值由 %d 变为 %d \n", p->NO, p->value, new_value);
				return 1;
			}
		    p = p->next;
	    }
	}
	return 0;
}
