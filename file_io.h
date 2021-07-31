#pragma once

#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

size_t get_file_size(int fd);
int read_file(int src_fd, char *buf);
int char_in_str(char c, const char *str);
void prepareCode(const char* code, char *buf);
