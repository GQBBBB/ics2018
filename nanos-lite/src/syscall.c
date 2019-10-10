#include "common.h"
#include "syscall.h"

void sys_yield(_Context *c) {
  _yield();
  c->GPR1 = 0;
}

void sys_exit(_Context *c) {
  _halt(0);
  c->GPR1 = 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  Log("0x%x, 0x%x, 0x%x, 0x%x", c->GPR1 , c->GPR2, c->GPR3, c->GPR4);
  switch (a[0]) {
	case SYS_yield: sys_yield(c); break;
	case SYS_exit: sys_exit(c); break; 
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
