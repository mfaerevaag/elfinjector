#include "elf.h"

int elfi_map(int fd, void **data, int *len)
{
    int size;

    size = file_get_size(fd);

    *data = mmap(0, size, PROT_READ| PROT_WRITE| PROT_EXEC, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        log_perr("mmap");
        exit(1);
    }

    log_infof("file (%d bytes) mapped at %p", size, data);

    *len = size;

    return fd;
}

void elfi_dump_segments(Elf64_Ehdr *elf_hdr)
{
    Elf64_Phdr* elf_seg;
    int i, n_seg;

    n_seg = elf_hdr->e_phnum;

    elf_seg = (Elf64_Phdr *) ((unsigned char*) elf_hdr
                              + (unsigned int) elf_hdr->e_phoff);

    for (i = 0; i < n_seg; i++) {
        log_debugf("segment %d: type: %8x (%x) offset: %8x "
                   "fsize:%8x msize:%8x",
                   i, elf_seg->p_type, elf_seg->p_flags,
                   (unsigned int) elf_seg->p_offset,
                   (unsigned int) elf_seg->p_filesz,
                   (unsigned int) elf_seg->p_memsz);

        elf_seg = (Elf64_Phdr *) ((unsigned char*) elf_seg
                                  + (unsigned int) elf_hdr->e_phentsize);
    }
}

Elf64_Phdr *elfi_find_gap(void *data, int fsize, int *p, int *len)
{
  Elf64_Ehdr* elf_hdr = (Elf64_Ehdr *) data;
  Elf64_Phdr* elf_seg, *text_seg;
  int         n_seg = elf_hdr->e_phnum;
  int         i;
  int         text_end, gap = fsize;

  elf_seg = (Elf64_Phdr *) ((unsigned char*) elf_hdr
			    + (unsigned int) elf_hdr->e_phoff);

  /* iterate segments */
  for (i = 0; i < n_seg; i++) {
      if (elf_seg->p_type == PT_LOAD && elf_seg->p_flags & 0x011) {
          log_infof("found .text segment (#%d)", i);
          text_seg = elf_seg;
          text_end = elf_seg->p_offset + elf_seg->p_filesz;
      }
      else if (elf_seg->p_type == PT_LOAD && (elf_seg->p_offset - text_end) < gap) {
          log_infof("   found LOAD segment (#%d) close to .text (offset: 0x%x)",
                    i, (unsigned int) elf_seg->p_offset);
          gap = elf_seg->p_offset - text_end;
      }

      elf_seg = (Elf64_Phdr *) ((unsigned char*) elf_seg
                                + (unsigned int) elf_hdr->e_phentsize);
  }

  *p = text_end;
  *len = gap;

  log_infof("gap in .text segment at offset 0x%x (0x%x bytes available)", text_end, gap);

  return text_seg;
}
