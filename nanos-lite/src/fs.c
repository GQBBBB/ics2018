#include "fs.h"

size_t ramdisk_read(void* buf, size_t offset, size_t len);
size_t ramdisk_write(const void* buf, size_t offset, size_t len);
size_t serial_write(const void* buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode){
  for (int i = 0; i < NR_FILES; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
	  Log("open %s success!", pathname); 
      file_table[i].open_offset = 0;
      return i;
    }
  }
  Log("open %s fail!", pathname);
  assert(0);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len){
  Finfo *file = &file_table[fd];
  
  if (file->open_offset + len > file->size)
	  len = file->size - file->open_offset;
  
  size_t return_len = ramdisk_read(buf, file->disk_offset + file->open_offset, len);
  file->open_offset += return_len;

  return return_len; 
}

size_t fs_write(int fd, const void *buf, size_t len){
  Finfo *file = &file_table[fd];
  
  if (file->open_offset + len > file->size)
	  len = file->size - file->open_offset;
  
  size_t return_len = ramdisk_write(buf, file->disk_offset + file->open_offset, len);
  file->open_offset += return_len;

  return return_len;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  Finfo *file = &file_table[fd];
  switch(whence) {
	  case SEEK_SET: file->open_offset = offset; break;
	  case SEEK_CUR: file->open_offset += offset; break;
	  case SEEK_END: file->open_offset = file->size + offset; break;
	  default: assert(0);
  }
  assert(file->open_offset >= 0 && file->open_offset <= file->size);
  return file->open_offset;
}

int fs_close(int fd){
  return 0;
}

size_t fs_filesz(int fd){
  return file_table[fd].size;
}
