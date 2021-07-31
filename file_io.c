#include "file_io.h"

size_t get_file_size(int fd) {
    struct stat st;
    fstat(fd, &st);

    return st.st_size;
}

int read_file(int src_fd, char *buf) {
    size_t size_read = read(src_fd, buf, get_file_size(src_fd));

    if(size_read < 0) {
        printf("Cannot read from file!");
        return 1;
    }

    return 0;
}

int char_in_str(char c, const char *str) {
    for(int i = 0; i < strlen(str); i++) {
        if(c == str[i]) return 1;
    }

    return 0;
}

void prepareCode(const char* code, char *buf) {
    const char *allowedSymbold = "<>[]+-,.#";
    int pointer_dest = 0;

    for(int i = 0; i < strlen(code); i++) {
        if(char_in_str(code[i], allowedSymbold)) {
            buf[pointer_dest] = code[i];
            pointer_dest++;
        }
    }

    buf[pointer_dest + 1] = '\0';
}

