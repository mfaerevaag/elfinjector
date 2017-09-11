#include <stdio.h>
#include <stdlib.h>

#include <elf.h>
#include <sys/mman.h>

#include "logger.h"
#include "file.h"

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

int main(int argc, char *argv[])
{
    void *target_data;
    char *target_path;
    int target_fd;
    int target_fsize;

    Elf64_Ehdr* elf_hdr;
    Elf64_Addr elf_ep;

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

    /* clean up */
    close(target_fd);

    return 0;
}
