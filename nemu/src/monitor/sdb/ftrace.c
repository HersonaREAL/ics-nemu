#include <elf.h>
#include <stdint.h>
#include <stdio.h>
#include "ftrace.h"
#include "debug.h"
#include "isa.h"

typedef struct FunctionTable {
    char func_name[1024];
    uint64_t value;
    uint64_t size;
}funcTab;

static uint64_t ftSize = 0;
static funcTab *ft;

static bool s_isFtraceEnable = false;

inline bool isFtraceEnable() {
    return s_isFtraceEnable;
}

static char * read_section64(FILE *fp, Elf64_Shdr sh)
{
	char* buff = malloc(sh.sh_size);
    int ret;
	if(!buff) {
		printf("%s:Failed to allocate %ldbytes\n",
				__func__, sh.sh_size);
	}

	assert(buff != NULL);
	assert(fseek(fp,sh.sh_offset, SEEK_SET) == 0);
    ret = fread(buff, sh.sh_size,1,fp);
	Assert(ret == 1,"fread fail, ret = %d, expect read: %ld",ret,sh.sh_size);

	return buff;
}

void load_elf(char *elf_file) {
    Elf64_Ehdr header;

    if (elf_file == NULL) {
        Log("elf file is not given, ftrace disable!");
        s_isFtraceEnable = false;
        return;
    }

    int ret;
    FILE *fp = fopen(elf_file, "rb");
    Assert(fp, "Can not open '%s'", elf_file);

    ret = fread(&header,sizeof(header),1, fp);
    Assert(ret != 0,"can not read header!");

    // check elf vaild
    if (memcmp(header.e_ident,ELFMAG,SELFMAG) != 0) {
        Log("This is not vaild elf file!");
        s_isFtraceEnable = false;
        return;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    Log("The elf file is %s, size = %ld", elf_file, size);

    //get section table
    Elf64_Shdr *shdr = (Elf64_Shdr *)malloc(sizeof(Elf64_Shdr) * header.e_shnum);
    Assert(shdr,"alloc shdr fail!");

    ret = fseek(fp,header.e_shoff,SEEK_SET);
    Assert(ret == 0, "can not seek file,offset = %lu",header.e_shoff);
    
    ret = fread(shdr,sizeof(Elf64_Shdr) * header.e_shnum,1,fp);
    Assert(ret != 0,"fail to read section");

    //get symt index
    int symt_index = 0;
    for (int i = 0; i < header.e_shnum; ++i) {
        if (shdr[i].sh_type == SHT_SYMTAB || shdr[i].sh_type == SHT_DYNSYM) {
            symt_index = i;
            break;
        }
    }

    //get sym table and str table
    Elf64_Sym *symt_buf = (Elf64_Sym *)read_section64(fp, shdr[symt_index]);
    char *str_tab = read_section64(fp, shdr[shdr[symt_index].sh_link]);
    int sym_cnt = (shdr[symt_index].sh_size / sizeof(Elf64_Sym));

    //get func cnt
    for (int i = 0; i < sym_cnt; ++i) {
        if (ELF64_ST_TYPE( symt_buf[i].st_info) == STT_FUNC) {
            ++ftSize;
        }
    }

    //save sym info
    ft = (funcTab *)malloc(sizeof(funcTab) * ftSize);
    assert(ft);
    for (int i = 0,ft_idx = 0; i < sym_cnt; ++i) {
        if (ELF64_ST_TYPE( symt_buf[i].st_info) == STT_FUNC) {
            assert(ft_idx < ftSize);

            strcpy(ft[ft_idx].func_name, str_tab + symt_buf[i].st_name);
            ft[ft_idx].value = symt_buf[i].st_value;
            ft[ft_idx].size = symt_buf[i].st_size;
            ++ft_idx;
        }
    }

    // release resource
    fclose(fp);
    free(symt_buf);
    free(str_tab);
    free(shdr);
    s_isFtraceEnable = true;
}

void printJmpInfo(uint64_t addr, bool isCall) {
    static int space = 0;
    
    char buf[4096] = {0};
    char *p = buf;
    p += snprintf(p,sizeof(buf),"\033[35m0x%016lx\033[0m: ",cpu.pc);   
    
    if (!isCall)
        space += -2;
    if (space < 0)
        space = 0;

    memset(p,' ', space);

    p += space;

    if (isCall)
        space += 2;
    
    p += snprintf(p, sizeof(buf) - (p - buf), isCall ? "\033[36mCall\033[0m " : "\033[33mret\033[0m  ");

    for (int i = 0; i < ftSize; ++i) {
        if (addr >= ft[i].value && addr < ft[i].value + ft[i].size) {
            p += snprintf(p, sizeof(buf) - (p - buf), "[\033[32m%s\033[0m @0x%016lx]\n", ft[i].func_name,addr);
            printf("%s",buf);
            return;
        }
    }

    //not found
    p += snprintf(p, sizeof(buf) - (p - buf), "[%s@%016lx]\n", "???",addr);
    printf("%s",buf);
}