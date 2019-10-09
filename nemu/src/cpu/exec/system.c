#include "cpu/exec.h"

void difftest_skip_ref();
void difftest_skip_dut();
uint32_t pio_read_b(ioaddr_t addr);
uint32_t pio_read_w(ioaddr_t addr);
uint32_t pio_read_l(ioaddr_t addr);
void pio_write_b(ioaddr_t addr, uint32_t data);
void pio_write_w(ioaddr_t addr, uint32_t data);
void pio_write_l(ioaddr_t addr, uint32_t data);
void raise_intr(uint8_t NO, vaddr_t ret_addr);

make_EHelper(lidt) {
  rtl_mv(&t0, &id_dest->addr);
  // limit
  rtl_lm(&t1, &t0, 2);
  // base
  rtl_addi(&t0, &t0, 2);
  rtl_lm(&t2, &t0, 2);
  rtl_addi(&t0, &t0, 2);
  rtl_lm(&t3, &t0, 2);
  
  cpu.idtr.limit = t1;
  cpu.idtr.base = (t3 << 16) | t2;

  if (decoding.is_operand_size_16) {
	// 不使用高8位
    cpu.idtr.base &= 0xffffff;
  } 
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
  raise_intr(id_dest->val, *eip);

  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
  rtl_pop(&decoding.jmp_eip);
  rtl_pop(&t0);
  cpu.cs = t0;
  rtl_pop(&cpu.eflags.val);
  rtl_j(decoding.jmp_eip); 

  print_asm("iret");
}

make_EHelper(in) {
  switch (id_dest->width) {
    case 1: t0 = pio_read_b(id_src->val);
	    rtl_sr(id_dest->reg, &t0, 1);
	    break;
    case 2: t0 = pio_read_w(id_src->val);
	    rtl_sr(id_dest->reg, &t0, 2);
	    break;
    case 4: t0 = pio_read_l(id_src->val);
	    rtl_sr(id_dest->reg, &t0, 4);
	    break;
    default: assert(0);
  }

  print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(out) {
  switch (id_src->width) {
    case 1: rtl_lr(&t0, id_src->reg, 1);
	    pio_write_b(id_dest->val, t0);
	    break;
    case 2: rtl_lr(&t0, id_src->reg, 2);
	    pio_write_w(id_dest->val, t0);
	    break;
    case 4: rtl_lr(&t0, id_src->reg, 4);
	    pio_write_l(id_dest->val, t0);
	    break;
    default: assert(0);
  }

  print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
