#include "debug.h"
#include <fs.h>
#include <stdio.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

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
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode) {
  int n = sizeof(file_table) / sizeof(Finfo);
  for (int i = 0; i < n; ++i) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = file_table[i].disk_offset;
      return i;
    }
  }
  panic("can not open file!, %s",pathname);
}

size_t fs_read(int fd, void *buf, size_t len) {
  Finfo *f = &file_table[fd];
  size_t ret;
  if (fd <= 2) {
    return 0;
  }

  ret = ramdisk_read(buf, f->open_offset, len);
  f->open_offset += ret;
  return ret;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  Finfo *f = &file_table[fd];
  size_t ret;

  if (fd == FD_STDIN) {
    return 0;
  }

  // stdout stderr
  if (fd == FD_STDOUT || fd == FD_STDERR) {
    char *p = (char *)buf;
    for (size_t i = 0; i < len; ++i) {
      putch(p[i]);
    }
    return len;
  }

  // normal file
  ret = ramdisk_write(buf, f->open_offset, len);
  f->open_offset += ret;
  return ret;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  Finfo *f = &file_table[fd];
  size_t rsz = get_ramdisk_size();
  if (fd <= 2) {
    return 0;
  }

  switch (whence) {
    case SEEK_SET :
      if (f->open_offset + offset > rsz || offset < 0) return -1;
      f->open_offset = f->disk_offset + offset;
      break;
    case SEEK_CUR :
      if (f->open_offset + offset > rsz || f->open_offset + offset < f->disk_offset) return -1;
      f->open_offset += offset;
      break;
    case SEEK_END :
      if (f->open_offset + offset > rsz || f->open_offset + offset < f->disk_offset) return -1;
      f->open_offset = f->disk_offset + f->size + offset;
      break;
    default: panic("fs_lseek!");
  }
  return f->open_offset - f->disk_offset;
}

int fs_close(int fd) {
  return 0;
}

char *fs_filename(int fd) {
  return file_table[fd].name;
}