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

void sys_write(_Context *c) {
  int fd = c->GPR2;
  char *buf = (char*) c->GPR3;
  size_t len = c->GPR4; 
  if (fd == 1 || fd == 2){
	  for (size_t i = 0; i < len; i++){
		  _putc(buf[i]);
	  }
  }
  c->GPR1 = len;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  Log("SYSCALL_ID=0x%x", a[0]);
  switch (a[0]) {
	case SYS_yield: sys_yield(c); break; // 1
	case SYS_exit: sys_exit(c); break; // 0
	case SYS_write: sys_write(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
