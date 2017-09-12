#include "elf.h"

int elfi_map(int fd, void **data, int *len)
{
    int size;

    size = file_get_size(fd);

    *data = mmap(0, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        log_perr("mmap");
        exit(1);
    }

    log_debugf("file (%d bytes) mapped at %p", size, data);

    *len = size;

    return fd;
}

int elfi_mem_subst(void *mem, int len, long pat, long val)
{
    unsigned char *p;
    long           v;
    int            i, r;

    p = (unsigned char *) mem;

    for (i = 0; i < len; i++) {
        v = *((long *) (p+i));
        r = v ^ pat;

        if (r == 0) {
            *((long *) (p + i)) = val;

            log_debugf("pattern %lx at offset %d -> %lx", pat, i, val);

            return 0;
        }
    }

    return -1;
}

Elf64_Phdr *elfi_find_gap(void *data, int fsize, int *gap_offset, int *gap_len)
{
    Elf64_Ehdr *elf_hdr;
    Elf64_Phdr *elf_seg, *text_seg;
    int         n_seg;
    int         i;
    int         text_end, gap;

    elf_hdr = (Elf64_Ehdr *) data;
    n_seg = elf_hdr->e_phnum;
    gap = fsize;

    elf_seg = (Elf64_Phdr *) ((unsigned char*) elf_hdr
                              + (unsigned int) elf_hdr->e_phoff);

    /* TODO: refactor */
    for (i = 0; i < n_seg; i++) {
        /* found .text */
        if (elf_seg->p_type == PT_LOAD && elf_seg->p_flags & 0x11) {
            log_debugf("found .text segment (#%d)", i);

            text_seg = elf_seg;
            text_end = elf_seg->p_offset + elf_seg->p_filesz;
        }
        /* found PT_LOAD segment after */
        else if (elf_seg->p_type == PT_LOAD && (elf_seg->p_offset - text_end) < gap) {
            log_debugf("\tfound LOAD segment (#%d) close to .text (offset: 0x%x)",
                      i, (unsigned int) elf_seg->p_offset);

            gap = elf_seg->p_offset - text_end;
            /* TODO: break? find larger? */
        }

        elf_seg = (Elf64_Phdr *) ((unsigned char*) elf_seg
                                  + (unsigned int) elf_hdr->e_phentsize);
    }

    *gap_offset = text_end;
    *gap_len = gap;

    log_infof("gap in .text segment at offset 0x%x (0x%x bytes available)", text_end, gap);

    return text_seg;
}

Elf64_Shdr *elfi_find_section(void *data, char *name)
{
    char       *sname;
    int        i;
    Elf64_Ehdr *elf_hdr;
    Elf64_Shdr *shdr;
    Elf64_Shdr *sh_strtab;
    char       *sh_strtab_p;

    elf_hdr = (Elf64_Ehdr *) data;
    shdr = (Elf64_Shdr *)(data + elf_hdr->e_shoff);
    sh_strtab = &shdr[elf_hdr->e_shstrndx];
    sh_strtab_p = data + sh_strtab->sh_offset;

    log_debugf("%d sections in file. looking for section '%s'",
               elf_hdr->e_shnum, name);

    for (i = 0; i < elf_hdr->e_shnum; i++) {
        sname = (char*) (sh_strtab_p + shdr[i].sh_name);

        if (!strcmp (sname, name)) {
            return &shdr[i];
        }
    }

    return NULL;
}

void elfi_dump_segments(void *data)
{
    Elf64_Ehdr *elf_hdr;
    Elf64_Phdr *elf_seg;
    int         i, n_seg;

    elf_hdr = (Elf64_Ehdr *) data;
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
