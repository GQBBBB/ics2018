#include "nemu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

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

uint32_t vaddr_read(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  paddr_write(addr, data, len);
}
