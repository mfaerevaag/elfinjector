#include "elf.h"

int elf_check_magic(void *data)
{
    if (!strncmp((char *) data, ELFMAG, SELFMAG)) {
        return -1;
    }

    return 0;
}

int elf_map(int fd, void **data, int *len)
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

int elf_patch_rel_jmp(void *mem, int len, long pat, long val)
{
    int i;
    long data;
    unsigned char *ptr;

    /* pointer to code */
    ptr = (unsigned char *) mem;

    for (i = 0; i < len; i++) {
        /* get value under pointer plus offset */
        data = *((long *) (ptr + i));

        /* check matching pattern */
        if (((int) data ^ (int) pat) == 0) {
            /* add size of code up to, and including, jmp instruction */
            *((long *) (ptr + i)) = val - i - 4;

            log_debugf("pattern found at offset 0x%x -> 0x%lx", i, val);

            return 0;
        }
    }

    return -1;
}

Elf64_Phdr *elf_find_gap(void *data, int fsize, int *gap_offset, int *gap_len)
{
    int         text_end, len;
    Elf64_Ehdr *hdr;
    Elf64_Phdr *next_seg, *text_seg;

    hdr = (Elf64_Ehdr *) data;
    text_seg = NULL;

    /* find .text */
    text_seg = elf_find_text(data);
    if (text_seg == NULL) {
        log_err("unable to find segment with .text section");
        return NULL;
    }

    /* end of .text section */
    text_end = text_seg->p_offset + text_seg->p_filesz;

    /* following segment */
    next_seg = (Elf64_Phdr *) ((unsigned char*) text_seg
                          + (unsigned int) hdr->e_phentsize);

    /* check for gap */
    if (next_seg->p_type == PT_LOAD &&
        (next_seg->p_offset - text_end) < (unsigned int) fsize) {
        len = next_seg->p_offset - text_end;

        log_infof("gap in .text segment at offset 0x%x (%d bytes available)",
                  text_end, len);

        *gap_offset = text_end;
        *gap_len = len;
    } else {
        log_err("not gab found in following segment");

        return NULL;
    }

    return text_seg;
}

Elf64_Phdr *elf_find_text(void *data)
{
    int         i;
    Elf64_Ehdr *hdr;
    Elf64_Phdr *seg;

    hdr = (Elf64_Ehdr *) data;
    seg = (Elf64_Phdr *) ((unsigned char*) hdr +
                          (unsigned int) hdr->e_phoff);

    /* iterate segments */
    for (i = 0; i < hdr->e_phnum; i++) {
        /* if PT_LOAD with read and exec permissions */
        if (seg->p_type == PT_LOAD && seg->p_flags & (PF_R | PF_X)) {
            log_debugf("found .text segment (#%d)", i);
            break;
        }

        seg = (Elf64_Phdr *) ((unsigned char *) seg +
                              (unsigned int) hdr->e_phentsize);
    }

    return seg;
}

Elf64_Shdr *elf_find_section(void *data, char *name)
{
    int        i;
    char       *sname;
    Elf64_Ehdr *hdr;
    Elf64_Shdr *shdr, *sh_strtab;
    char       *sh_strtab_p;

    hdr = (Elf64_Ehdr *) data;
    shdr = (Elf64_Shdr *)(data + hdr->e_shoff);
    sh_strtab = &shdr[hdr->e_shstrndx];
    sh_strtab_p = data + sh_strtab->sh_offset; /* store ptr to strtab */

    log_debugf("%d sections in file. looking for section '%s'",
               hdr->e_shnum, name);

    for (i = 0; i < hdr->e_shnum; i++) {
        sname = (char *) (sh_strtab_p + shdr[i].sh_name);

        if (!strcmp(sname, name)) {
            return &shdr[i];
        }
    }

    return NULL;
}

void elfi_dump_segments(void *data)
{
    int         i, n_seg;
    Elf64_Ehdr *hdr;
    Elf64_Phdr *seg;

    hdr = (Elf64_Ehdr *) data;
    n_seg = hdr->e_phnum;

    seg = (Elf64_Phdr *) ((unsigned char*) hdr
                          + (unsigned int) hdr->e_phoff);

    for (i = 0; i < n_seg; i++) {
        log_debugf("segment %d: type: %8x (%x) offset: %8x "
                   "fsize:%8x msize:%8x",
                   i, seg->p_type, seg->p_flags,
                   (unsigned int) seg->p_offset,
                   (unsigned int) seg->p_filesz,
                   (unsigned int) seg->p_memsz);

        seg = (Elf64_Phdr *) ((unsigned char*) seg
                              + (unsigned int) hdr->e_phentsize);
    }
}
