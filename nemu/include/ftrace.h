#ifndef FTRACE_H
#define FTRACE_H
#include <stdbool.h>
#include <stdint.h>

bool isFtraceEnable();

void load_elf(char *elf_file);

void printJmpInfo(uint64_t addr, bool isCall);

#endif