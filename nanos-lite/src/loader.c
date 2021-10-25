#include <proc.h>
#include <elf.h>
#include <common.h>

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
  int ret, n;

  // check header
  ret = ramdisk_read(&header, 0, sizeof(Elf_Ehdr));
  assert(ret == sizeof(Elf_Ehdr));
  assert(memcmp(header.e_ident,ELFMAG,SELFMAG) == 0);
  
  //read seg
  n = header.e_phnum;
  for (int i = 0; i < n; ++i) {
    ramdisk_read(&seg, header.e_phoff + i * sizeof(Elf_Phdr), sizeof(Elf_Phdr));
    if (seg.p_type == PT_LOAD) {
      ramdisk_read((void *)seg.p_vaddr, seg.p_offset, seg.p_memsz);
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

