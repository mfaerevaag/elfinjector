#ifndef ELF_H
#define ELF_H

#include <elf.h>
#include <sys/mman.h>

#include "logger.h"
#include "file.h"

int elfi_map(int fd, void **data, int *len);
void elfi_dump_segments(Elf64_Ehdr *elf_hdr);
Elf64_Phdr *elfi_find_gap(void *data, int fsize, int *p, int *len);

#endif /* ELF_H */
