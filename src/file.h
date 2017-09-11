#ifndef FILE_H
#define FILE_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int file_get_size(int fd);

#endif /* FILE_H */
