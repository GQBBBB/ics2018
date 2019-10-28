#include "common.h"
#include <amdev.h>
#include "proc.h"

size_t serial_write(const void *buf, size_t offset, size_t len) {
  //_yield();
  const char *b = (const char *) buf;
  for (size_t i = 0; i < len; i++){
	  _putc(b[i]);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  //_yield();
  int key = read_key();
  int return_len = 0;
  
  if ((key & 0x1ff) == _KEY_NONE) {
    return_len = sprintf(buf, "t %d\n", uptime());
  } else {
	// 判断是否keydown
	if (key & 0x8000){
	  return_len = sprintf(buf, "kd %s\n", keyname[key & 0x1ff]);

      if ((key & 0xfff) == _KEY_F1) {
        fg_pcb = &pcb[0];
      } else if ((key & 0xfff) == _KEY_F2) { 
        fg_pcb = &pcb[1];
      }else if ((key & 0xfff) == _KEY_F3) { 
        fg_pcb = &pcb[2];
      }

	} else {
	  return_len = sprintf(buf, "ku %s\n", keyname[key & 0x1ff]);
	}
  }

  return return_len;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  if (offset + len > strlen(dispinfo)) {
    len = strlen(dispinfo) - offset;
  }
  char *b = (char *) buf;
  for (size_t i = 0; i < len; i++) {
	b[i] = dispinfo[offset + i];
  }
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  //_yield();
  int x = (offset / sizeof(uint32_t)) % screen_width();
  int y = (offset / sizeof(uint32_t)) / screen_width();
  int w = len / sizeof(uint32_t);
  int h = 1;
  draw_rect((uint32_t *) buf, x, y, w, h);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
