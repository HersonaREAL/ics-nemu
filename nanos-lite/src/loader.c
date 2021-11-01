#include <proc.h>
#include <elf.h>
#include <fs.h>
#include <common.h>
#include <stdio.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr header;
  Elf_Phdr seg;
  int ret, n, fd;

  //clean screen
  int fb_fd = fs_open("/dev/fb",0,0);
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  int zero[4096] = {0};
  fs_lseek(fb_fd, 0, 0);
  for (int i = 0; i < h ; ++i)
    fs_write(fb_fd, &zero, w * 4);
  fs_lseek(fb_fd, 0, 0);

  //open file
  char *p = (char *)filename;
  while(*p) {
    if (*p == '\n'|| *p == ' ') {
      *p = '\0';
      break;
    }
    ++p;
  }
  fd = fs_open(filename, 0, 0);

  // check header
  ret = fs_read(fd, &header, sizeof(Elf_Ehdr));
  assert(ret == sizeof(Elf_Ehdr));
  assert(memcmp(header.e_ident,ELFMAG,SELFMAG) == 0);

  //read seg
  n = header.e_phnum;
  // printf("n: %d\n",n);
  for (int i = 0; i < n; ++i) {
    fs_lseek(fd, header.e_phoff + i * sizeof(Elf_Phdr), SEEK_SET);
    fs_read(fd, &seg, sizeof(Elf_Phdr));
    if (seg.p_type == PT_LOAD) {
      //printf("p_offset: 0xd, p_vaddr: 0x%016lx, p_memsz: %lu, p_filesz: %lu\n",seg.p_offset,seg.p_vaddr,seg.p_memsz,seg.p_filesz);
      fs_lseek(fd, seg.p_offset, SEEK_SET);
      fs_read(fd,(void *)seg.p_vaddr, seg.p_memsz);
      memset((char *)(seg.p_vaddr + seg.p_filesz),0,seg.p_memsz - seg.p_filesz);
    }
  }
  return header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

