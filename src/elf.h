#ifndef ELF_H
#define ELF_H

#include <string.h>

#include <elf.h>
#include <sys/mman.h>

#include "logger.h"
#include "file.h"

int elf_check_magic(void *data);

int elf_map(int fd, void **data, int *len);

int elf_patch_rel_jmp(void *mem, int len, long pat, long val);

Elf64_Phdr *elf_find_text_sec(void *data);
Elf64_Phdr *elf_find_gap(void *data, int fsize, int *gap_offset, int *gap_len);
Elf64_Shdr *elf_find_section(void *data, char *name);

void elf_dump_segments(void *data);

#endif /* ELF_H */
