#include <am.h>
#include <x86.h>
#include <amdev.h>

#define I8042_DATA_PORT 0x60
#define KEYDOWN_MASK 0x8000

size_t input_read(uintptr_t reg, void *buf, size_t size) {
  uint32_t key = inl(I8042_DATA_PORT);
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      kbd->keydown = (key & KEYDOWN_MASK) != 0;
      kbd->keycode = kbd->keydown ? (key ^ KEYDOWN_MASK) : key;
      return sizeof(_KbdReg);
    }
  }
  return 0;
}
