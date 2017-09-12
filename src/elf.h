#ifndef ELF_H
#define ELF_H

#include <elf.h>
#include <sys/mman.h>

#include "logger.h"
#include "file.h"

int elfi_map(int fd, void **data, int *len);
int elfi_mem_subst(void *mem, int len, long pat, long val);

Elf64_Phdr *elfi_find_text(void *data);
Elf64_Phdr *elfi_find_gap(void *data, int fsize, int *gap_offset, int *gap_len);
Elf64_Shdr *elfi_find_section(void *data, char *name);

void elfi_dump_segments(void *data);

#endif /* ELF_H */
