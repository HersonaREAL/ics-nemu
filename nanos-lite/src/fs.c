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

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_SB, FD_SBCTL,FD_EVENT};

size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t sb_write(const void *buf, size_t offset, size_t len);
size_t sbctl_write(const void *buf, size_t offset, size_t len);
size_t sbctl_read(void *buf, size_t offset, size_t len);

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
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_FB]     = {"/dev/fb", 0, 0, invalid_read, fb_write},
  [FD_SB]     = {"/dev/sb", 0, 0, invalid_read, sb_write},
  [FD_SBCTL]  = {"/dev/sbctl",0, 0, sbctl_read, sbctl_write},
  [FD_EVENT]  = {"/dev/events", 0, 0, events_read, serial_write},
#include "files.h"
  {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
    int w = io_read(AM_GPU_CONFIG).width;
    int h = io_read(AM_GPU_CONFIG).height;
    file_table[FD_FB].size = w * h * 4;
}

int fs_open(const char *pathname, int flags, int mode) {
  int n = sizeof(file_table) / sizeof(Finfo);
  for (int i = 0; i < n; ++i) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = file_table[i].disk_offset;
      //printf("open_offset: %d\n",file_table[i].open_offset);
      return i;
    }
  }
  panic("can not open file!, %s",pathname);
}

size_t fs_read(int fd, void *buf, size_t len) {
  Finfo *f = &file_table[fd];
  size_t ret;

  if (f->size != 0 && f->open_offset - f->disk_offset + len > f->size) {
    len = f->size - (f->open_offset - f->disk_offset);
  }

  if (f->read) {
    ret = f->read(buf,f->open_offset,len);
  } else {
    ret = ramdisk_read(buf, f->open_offset, len);
  }

  f->open_offset += ret;
  return ret;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  Finfo *f = &file_table[fd];
  size_t ret;

  if (f->size != 0 && f->open_offset - f->disk_offset + len > f->size) {
    len = f->size - (f->open_offset - f->disk_offset);
  }

  // stdout stderr
  if (f->write) {
    ret = f->write(buf,f->open_offset,len);
  } else {
    // normal file
    ret = ramdisk_write(buf, f->open_offset, len);
  }

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
      if (f->disk_offset + offset > rsz || offset < 0) panic("SEEK_SET,f->open_offset + offset: %lu, rsz: %lu",f->disk_offset + offset, rsz);
      f->open_offset = f->disk_offset + offset;
      break;
    case SEEK_CUR :
      if (f->open_offset + offset > rsz || f->open_offset + offset < f->disk_offset) panic("SEEK_CUR");
      f->open_offset += offset;
      break;
    case SEEK_END :
      if (f->open_offset + offset > rsz || f->open_offset + offset < f->disk_offset) panic("SEEK_END");
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