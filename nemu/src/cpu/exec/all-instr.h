#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

// PA2.1
make_EHelper(call); // control.c
make_EHelper(push); // data-mov.c
make_EHelper(sub); // arith.c
make_EHelper(xor); // logic.c
make_EHelper(pop); // data-mov.c
make_EHelper(ret); // control.c

// PA2.3
make_EHelper(nop); // special.c
make_EHelper(jmp); // control.c
make_EHelper(test); // logic.c
make_EHelper(jcc); // control.c
make_EHelper(leave); // data-mov.c
make_EHelper(add); // arith.c
make_EHelper(cmp); // arith.c
make_EHelper(inc); // arith.c
make_EHelper(dec); // arith.c
make_EHelper(neg); // arith.c
make_EHelper(adc); // arith.c
make_EHelper(sbb); // arith.c
make_EHelper(mul); // arith.c
make_EHelper(imul1); // arith.c
make_EHelper(imul2); // arith.c
make_EHelper(imul3); // arith.c
make_EHelper(div); // arith.c
make_EHelper(idiv); // arith.c
make_EHelper(and); // logic.c
make_EHelper(or); // logic.c
make_EHelper(sar); // logic.c
make_EHelper(shl); // logic.c
make_EHelper(shr); // logic.c
make_EHelper(setcc); // logic.c
make_EHelper(not); // logic.c
make_EHelper(jmp_rm); // control.c
make_EHelper(call_rm); // control.c
make_EHelper(pusha); // data-mov.c
make_EHelper(popa); // data-mov.c
make_EHelper(lea); // data-mov.c
make_EHelper(movsx); // data-mov.c
make_EHelper(movzx); // data-mov.c
make_EHelper(cltd); // data-mov.c
make_EHelper(cwtl); // data-mov.c
make_EHelper(out); // system.c
make_EHelper(in); // system.c
make_EHelper(int); // system.c
make_EHelper(lidt); // system.c
make_EHelper(iret); // system.c
make_EHelper(mov_cr2r); // system.c
make_EHelper(mov_r2cr); // system.c

