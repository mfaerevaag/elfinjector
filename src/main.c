#include <stdio.h>
#include <stdlib.h>

#include <elf.h>

#include "logger.h"
#include "elf.h"

int main(int argc, char *argv[])
{
    void *target_data;
    char *target_path;
    int target_fd;
    int target_fsize;

    Elf64_Ehdr* elf_hdr;
    Elf64_Addr elf_ep;
    Elf64_Phdr *elf_text;

    int gap_offset;
    int gap_len;

    if (argc != 2) {
        fprintf(stderr, "usage: %s target\n", argv[0]);
        return 1;
    }

    /* parse args */
    target_path = argv[1];

    /* open file */
    target_fd = open(target_path, O_APPEND | O_RDWR, 0);
    if (target_fd < 0) {
        log_perr("open");
        exit(1);
    }

    target_fd  = elfi_map(target_fd, &target_data, &target_fsize);

    /* get binary entry point */
    elf_hdr = (Elf64_Ehdr *) target_data;
    elf_ep = elf_hdr->e_entry;

    log_infof("target entry point: %p", (void *) elf_ep);

    elfi_dump_segments(elf_hdr);

    /* Find executable segment and obtain offset and gap size */
    elf_text = elfi_find_gap(target_data, target_fsize, &gap_offset, &gap_len);
    /* base = t_text_seg->p_vaddr; */

    log_infof("target .text seg: %p", (void *) elf_text);

    /* clean up */
    close(target_fd);

    return 0;
}
