#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*pgalloc_usr)(size_t);
static void (*pgfree_usr)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);

  return 0;
}

int _protect(_Protect *p) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  p->pgsize = 4096;
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
  return 0;
}

void _unprotect(_Protect *p) {
}

static _Protect *cur_as = NULL;
void get_cur_as(_Context *c) {
  c->prot = cur_as;
}

void _switch(_Context *c) {
  set_cr3(c->prot->ptr);
  cur_as = c->prot;
}

int _map(_Protect *p, void *va, void *pa, int mode) {
  // 获取页目录表基址
  PDE *updir = (PDE *) (p->ptr);
  intptr_t vaddr = (intptr_t) va;
  // 获取va对应页目录项
  PDE pde = updir[PDX(vaddr)];

  // 判断页目录项pde对应物理页是否可用
  if ((pde & PTE_P) == 0) {// 不可用
	// 申请新的物理页
    PTE *new = (PTE *)(pgalloc_usr(1));
	// 把该物理页赋给该页目录项
    pde = ((PDE)new & 0xfffff000) | PTE_P;
	// 更新页目录项
    updir[PDX(vaddr)] = pde;
  }

  // 获取页表基址
  PTE *upt = (PTE *)(((pde >> 12) & 0xfffff) << 12);
  // 获取页表项
  PTE pte = upt[PTX(vaddr)];
  // 判断页表项pte对应物理页是否可用
  if ((pte & PTE_P) == 0) {// 不可用
    // 使用物理页pa更新页表项
    upt[PTX(vaddr)] = ((PTE)pa & 0xfffff000) | PTE_P;
  }

  return 0;
}

_Context *_ucontext(_Protect *p, _Area ustack, _Area kstack, void *entry, void *args) {
  typedef struct {
    int argc;
    char** argv;
    char** envp;
  } StackFrame;

  _Context *cp = (_Context*) (ustack.end - sizeof(StackFrame) - sizeof(_Context));
  StackFrame *sf = (StackFrame*) (ustack.end - sizeof(StackFrame)); 

  sf->argc = 0;
  sf->argv = NULL;
  sf->envp = NULL;

  cp->prot = p;
  cp->eip = (uintptr_t) entry;
  cp->cs = 0x8; 
  cp->esp = (uintptr_t)((void*)cp + sizeof(struct _Protect*) + 3 * sizeof(uintptr_t));
  cp->eflags |= 0x200;

  return cp; 
}
