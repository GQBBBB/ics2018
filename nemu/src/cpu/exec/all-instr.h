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
