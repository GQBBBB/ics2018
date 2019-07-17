#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[32];
  int value;
  bool flag;
  char type[11];
} WP;

WP* new_wp(char *str);
void free_wp(int n);
void print_wp();
int check_wp();

#endif
