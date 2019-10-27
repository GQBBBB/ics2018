#include "nemu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

// Control Register flags
#define CR0_PG    0x80000000  // Paging

// Page table/directory entry flags
#define PTE_P     0x001     // Present

// Page directory and page table constants
#define PTXSHFT   12      // Offset of PTX in a linear address
#define PDXSHFT   22      // Offset of PDX in a linear address


// +--------10------+-------10-------+---------12----------+
// | Page Directory |   Page Table   | Offset within Page  |
// |      Index     |      Index     |                     |
// +----------------+----------------+---------------------+
//  \--- PDX(va) --/ \--- PTX(va) --/ \----- OFF(va) ------/
typedef uint32_t PTE;
typedef uint32_t PDE;
#define PDX(va)     (((uint32_t)(va) >> PDXSHFT) & 0x3ff)
#define PTX(va)     (((uint32_t)(va) >> PTXSHFT) & 0x3ff)
#define OFF(va)     ((uint32_t)(va) & 0xfff)

// construct virtual address from indexes and offset
#define PGADDR(d, t, o) ((uint32_t)((d) << PDXSHFT | (t) << PTXSHFT | (o)))

// Address in page table or page directory entry
#define PTE_ADDR(pte)   ((uint32_t)(pte) & ~0xfff)

// page size
#define PGSIZE 4096

uint8_t pmem[PMEM_SIZE];

int is_mmio(paddr_t addr);
uint32_t mmio_read(paddr_t addr, int len, int mmio_id);
void mmio_write(paddr_t addr, int len, uint32_t data, int mmio_id);

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  // 判断该物理地址是否被映射到I/O地址空间
  int mmio_id = is_mmio(addr);
  if (mmio_id != -1)
	return mmio_read(addr, len, mmio_id);

  return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
  // 判断该物理地址是否被映射到I/O地址空间
  int mmio_id = is_mmio(addr);
  if (mmio_id != -1)
	mmio_write(addr, len, data, mmio_id);
  
  memcpy(guest_to_host(addr), &data, len);
}

paddr_t page_translate(vaddr_t vaddr){
  //Log("Page translate vaddr: 0x%08x", vaddr);
  paddr_t dir = PTE_ADDR(cpu.CR3);
  assert(paddr_read(dir + sizeof(paddr_t) * PDX(vaddr), sizeof(paddr_t)) & PTE_P);
  
  paddr_t pg = PTE_ADDR(paddr_read(dir + sizeof(paddr_t) * PDX(vaddr), sizeof(paddr_t)));
  assert(paddr_read(pg + sizeof(paddr_t) * PTX(vaddr), sizeof(paddr_t)) & PTE_P);
  
  //Log("Page translate paddr: 0x%08x", PTE_ADDR(paddr_read(pg + sizeof(paddr_t) * PTX(vaddr), sizeof(paddr_t))) | OFF(vaddr));
  return (PTE_ADDR(paddr_read(pg + sizeof(paddr_t) * PTX(vaddr), sizeof(paddr_t))) | OFF(vaddr)); 
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  // 当CR0的PG位为1则开启分页模式
  if (cpu.CR0 & CR0_PG){
	// 数据跨越虚拟页边界, 如果这两个虚拟页被映射到两个不连续的物理页, 就需要进行两次页级地址转换
    if (OFF(addr) + len > PGSIZE) {
      int prev_len = PGSIZE - OFF(addr);
      int last_len = len - prev_len;
      uint32_t prev = paddr_read(page_translate(addr), prev_len);
      uint32_t last = paddr_read(page_translate(addr + prev_len), last_len);
      return (last << (8 * prev_len)) | prev;
    } else {
      return paddr_read(page_translate(addr), len);
    }
  }

  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if (cpu.CR0 & CR0_PG){
    if (OFF(addr) + len > PGSIZE) {
      assert(0);
    } else {
      paddr_write(page_translate(addr), data, len);
    }
  }else 
    paddr_write(addr, data, len);
}
