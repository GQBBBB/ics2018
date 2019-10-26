#include "proc.h"

#define DEFAULT_ENTRY 0x8048000

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
int fs_open(const char* pathname, int flags, int mode);
size_t fs_read(int fd, void* buf, size_t len);
int fs_close(int fd);
size_t fs_filesz(int fd);
void* new_page(size_t nr_page);

static uintptr_t loader(PCB *pcb, const char *filename) { 
  int fd = fs_open(filename, 0, 0);
  int len = fs_filesz(fd);
  int pgsize = pcb->as.pgsize; 
  int end = DEFAULT_ENTRY + len;
  Log("%s len:0x%x", filename, len);

  // 一页一页的copy
  for (uintptr_t st = DEFAULT_ENTRY; st < end; st += pgsize) {
	// 申请一页空闲的物理页
    void* newpage = new_page(1);
	Log("0x%08x", newpage);
	// 通过_map()把这一物理页映射到用户进程的虚拟地址空间中
	Log("Map va to pa: 0x%08x to 0x%08x", st, newpage);
    _map(&pcb->as, (void *)st, newpage, 0); 
	// 从文件中读入一页的内容到这一物理页上
    fs_read(fd, newpage, (end - st) < pgsize ? (end - st) : pgsize);
  }
  fs_close(fd);

  return DEFAULT_ENTRY;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) { 
  _protect(&pcb->as);
  
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
