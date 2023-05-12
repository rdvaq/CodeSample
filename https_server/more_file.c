#include "more_file.h"

#include <fcntl.h>
#include <sys/stat.h>

#include "errors.h"

int checked_open_for_reading(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        errno_err("could not open file for reading");
    }
    return fd;
}

int checked_open_for_writing(char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        errno_err("could not open file for writing");
    }
    return fd;
}