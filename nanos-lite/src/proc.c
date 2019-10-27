#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used));
static PCB pcb_boot;
PCB *current;

void naive_uload(PCB *pcb, const char *filename);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  char *filename = "/bin/dummy";
  naive_uload(&pcb[0], filename);
  switch_boot_pcb(); 
}

_Context* schedule(_Context *prev) {
  // save the context pointer
current->tf = prev;
Log("2:0x%x", current->tf);
// always select pcb[0] as the new process
current = &pcb[0];

// then return the new context
Log("3:0x%x", current->tf);
_yield();
return current->tf;
  //return NULL;
}
