#include "file.h"

int file_get_size(int fd)
{
    struct stat info;

    fstat(fd, &info);

    return info.st_size;
}
