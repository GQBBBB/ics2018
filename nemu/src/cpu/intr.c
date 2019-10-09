#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  Log("NO=%d", NO);
  rtl_push(&cpu.eflags.val);
  cpu.eflags.IF = 0;
  t0 = cpu.cs;
  rtl_push(&t0);
  rtl_push(&ret_addr);

  GateDesc gatedesc;
  vaddr_t addr = cpu.idtr.base + sizeof(GateDesc) * NO;
  //gatedesc.offset_15_0 = vaddr_read(addr, 2);
  //gatedesc.dont_care0 = vaddr_read(addr + 2, 2);
  //gatedesc.dont_care1 = vaddr_read(addr + 4, 2) & 0x7fff;
  //gatedesc.present = vaddr_read(addr + 4, 2) & 0x8000;
  //gatedesc.offset_31_16 = vaddr_read(addr + 6, 2);
  gatedesc.val = vaddr_read(addr, 8);
  Assert(gatedesc.present, "invalid gate descriptor!");

  decoding.jmp_eip = (gatedesc.offset_31_16 << 16) | gatedesc.offset_15_0;
  rtl_j(decoding.jmp_eip);
}

void dev_raise_intr() {
}
