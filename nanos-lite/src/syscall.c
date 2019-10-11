#include "common.h"
#include "syscall.h"

int mm_brk(uintptr_t new_brk);
int fs_open(const char* pathname, int flags, int mode);
size_t fs_write(int fd, void* buf, size_t len);
size_t fs_read(int fd, void* buf, size_t len);
int fs_close(int fd);
size_t fs_lseek(int fd, size_t offset, int whence);

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
  char *buf = (char *) c->GPR3;
  size_t len = c->GPR4; 
  c->GPR1 = fs_read(fd, buf, len);
}

void sys_open(_Context* c) {
  const char* path = (const char*) c->GPR2;
  int flags = c->GPR3;
  int mode = c->GPR4;
  c->GPR1 = fs_open(path, flags, mode);
}

void sys_brk(_Context *c) {
  uintptr_t addr = c->GPR2;
  c->GPR1 = mm_brk(addr);
}

void sys_read(_Context *c) {
  int fd = c->GPR2;
  char *buf = (char *) c->GPR3;
  size_t len = c->GPR4;
  c->GPR1 = fs_read(fd, buf, len); 
}

void sys_close(_Context *c) {
  Log("closeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
  int fd = c->GPR2;
  c->GPR1 = fs_close(fd);
}

void sys_lseek(_Context *c) {
  int fd = c->GPR2;
  size_t offset = c->GPR3;
  int whence = c->GPR4;
  c->GPR1 = fs_lseek(fd, offset, whence);
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  Log("SYSCALL_ID=0x%x", a[0]);
  switch (a[0]) {
	case SYS_yield: sys_yield(c); break; // 1
	case SYS_exit: sys_exit(c); break; // 0
	case SYS_write: sys_write(c); break; // 4
	case SYS_brk: sys_brk(c); break; // 9
	case SYS_open: sys_open(c); break; // 2
	case SYS_read: sys_read(c); break;  // 3
	case SYS_close: sys_close(c); break; // 7
	case SYS_lseek: sys_lseek(c); break; // 8
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
