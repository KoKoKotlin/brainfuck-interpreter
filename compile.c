#include "file_io.h"
#include <unistd.h>

#define CODE_BUF_SIZE 2048 

//  TODO: make argument to keep the c file
//  TODO: fix indentation buf in c source file
//  TODO: make debuf raise sigint optional

int find_repetition(char *code, int codepointer) {
    char sym = code[codepointer];
    int count = 0;

    while(code[codepointer++] == sym) count++;

    return count;
}

void add_indentation(char *stream, int lvl) {
    for(int i = 0; i < lvl; i++) {
        strcat(stream, "\t");
    }
}

char* parseInst(char *code, int *codepointer, char *stream) {

    static int indentation_lvl = 1;
    char *inst = calloc(sizeof(char), 100);
    int rep = 1;
    
    add_indentation(inst, indentation_lvl);
    
    switch (code[*codepointer]) {
        
        case '>': {
            rep = find_repetition(code, *codepointer);
            strcat(inst, "mempointer += ");
            sprintf(inst + strlen(inst), "%d;", rep);
            break;
        }
        case '<': {
            rep = find_repetition(code, *codepointer);
            strcat(inst, "mempointer -= ");
            sprintf(inst + strlen(inst), "%d;", rep);
            break;
        }
        case '+': {
            rep = find_repetition(code, *codepointer);
            strcat(inst, "mem[mempointer] += ");
            sprintf(inst + strlen(inst), "%d;", rep);

            break;
        }
        case '-': {
            rep = find_repetition(code, *codepointer);
            strcat(inst, "mem[mempointer] -= ");
            sprintf(inst + strlen(inst), "%d;", rep);
            
            break;
        }
        case '[': {
            indentation_lvl++;
            strcat(inst, "while(mem[mempointer] != 0) {");
            break;
        }
        case ']': {
            indentation_lvl--;
            strcat(inst, "\n");
            add_indentation(inst, indentation_lvl);
            strcat(inst, "}");
            break;
        }
        case '.': {
            strcat(inst, "printf(\"%c\", mem[mempointer]);");
            break;
        }
        case ',': {
            strcat(inst, "mem[mempointer] = getchar();");
            break;
        }
        case '#': {
            // strcat(inst, "raise(SIGINT);");
            break;
        }
    }
    strcat(inst, "\n");
    
    *codepointer = *codepointer + rep;
    
    return inst;
}

void flush_buf(char *str, char *stream, FILE *out) {
    if(strlen(str) + strlen(stream) > CODE_BUF_SIZE) {
        fputs(stream, out);
        stream[0] = '\0';
    }
}

void parse(char *code, FILE *out) {
    char *code_output_buf = calloc(sizeof(char), CODE_BUF_SIZE);

    int codepointer = 0;
    while(codepointer < strlen(code)) {
        char *next_line = parseInst(code, &codepointer, code_output_buf);
        flush_buf(next_line, code_output_buf, out);

        strcat(code_output_buf, next_line);
    }

    fputs(code_output_buf, out);
}

void init(FILE *output) {
    fputs("#include <stdio.h>\n#include <stdlib.h>\n#include <signal.h>\n\n#define MEMSIZE 9000\n\nint main() {\n\n\tchar *mem = calloc(sizeof(char), MEMSIZE);", output);
    fputs("\n\tint mempointer = 0;", output);
}

void finish(FILE *output) {
    fputs("\n\treturn 0;\n}", output);
    fflush(output);
} 

int main(int argc, char* argv[]) {

    if(argc != 2) {
        printf("Usage: ./bint <path>");
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd < 0) {
        printf("Cannot open file %s!", argv[1]);
        return -1;
    }

    char *src_buf = malloc(get_file_size(fd) * sizeof(char));
    if(read_file(fd, src_buf) == 1) {
        return -1;
    }

    char *code = malloc((strlen(src_buf) + 1) * sizeof(char));
    prepareCode(src_buf, code);
    free(src_buf);
    close(fd);

    FILE *out = fopen("temp_out_.c", "wb+");
    init(out);

    parse(code, out);

    finish(out);
    fclose(out);

    system("gcc temp_out_.c -o bf -O4");
    // system("rm temp_out_.c");

    free(code);
 
    return 0;
}