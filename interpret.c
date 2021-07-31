#include <ncurses.h>
#include "file_io.h"

#define SOLID 1
#define POINTER 2

#define EXEC_SI 0
#define EXEC_DEBUG 1
#define EXEC_RUN 2

#define MEMSIZE 90000

void initAll() {
    initscr();
    start_color();

    init_pair(SOLID, COLOR_BLACK, COLOR_WHITE);
    init_pair(POINTER, COLOR_RED, COLOR_WHITE);
}

void drawCode(const char *code, int size, int pointer) {
    const int offset = 10;

    char *pre = "Code: ";
    mvaddstr(1, 2, pre);
    if(strlen(code) + 6 <= size) {
        for(int i = 0; i < strlen(code); i++) {
            if(i == pointer) {
                attron(COLOR_PAIR(POINTER));
            }
            mvaddstr(1, i + 2 + strlen(pre), &code[i]);
            attroff(COLOR_PAIR(POINTER));    
        }
    } else {
        int start = pointer - offset;
        int pointTo = start + 10;
        if(pointer - offset < 0) {
            pointTo = pointer;
            start = 0;
        } else if(pointer - offset + size > strlen(code)) {
            pointTo = pointer - start;
            start = strlen(code) - size - 1;
        }
        
        for(int i = 0; i < size; i++) {
            int currIndex = i + start;
            if(currIndex == pointTo) {
                attron(COLOR_PAIR(POINTER));
            }
            mvaddch(1, i + 2 + strlen(pre), code[currIndex]);
            attroff(COLOR_PAIR(POINTER));    
        }
    }

}

void drawState(int code_pointer, int mem_pointer, const char *buf, const char *code) {
    mvprintw(1, 90, "Code Pointer: %d", code_pointer);
    mvprintw(2, 90, "Mem Pointer : %d", mem_pointer);

    // TODO: make case where mem pointer isnt on screen
    for(int i = 0; i < MEMSIZE; i++) {
        if(i == mem_pointer) {
            attron(COLOR_PAIR(POINTER));
        }
        mvprintw(i / 20 + 3, (i % 20 * 4) + 2, "%i", buf[i]);
        attroff(COLOR_PAIR(POINTER));
    }

    mvprintw(4, 90, "Commands: ");

    mvprintw(5, 94, "d: run the program and stop at debug points");
    mvprintw(6, 94, "r: run the program");
    mvprintw(7, 94, "q: quit the program");
    mvprintw(8, 94, "any other key: single step instructions");
    
}

void output(char c, char *buf, int mode) {
    static int offset = 0;
    if(mode == 1) {
        buf[offset] = c;
        offset++;    
    } else {
        putchar(c);
    }
}

int executeOne(const char *code, int *code_poiner, int *mem_pointer, char *buf, char *output_buf, int mode) { 
    char curr = code[*code_poiner];
    
    switch (curr) {
    case '<':
        *mem_pointer = *mem_pointer - 1;
        break;
    
    case '>':
        *mem_pointer = *mem_pointer + 1;
        break;
    
    case '+':
        buf[*mem_pointer] = buf[*mem_pointer] + 1;
        break;
    
    case '-':
        buf[*mem_pointer] = buf[*mem_pointer] - 1;
        break;
    
    case '[':
        if(buf[*mem_pointer] == 0) {
            int openingCount = 0;
            while(true) {
                *code_poiner = *code_poiner + 1;
                if(*code_poiner >= strlen(code)) break;
                
                if(code[*code_poiner] == ']') {
                    if(openingCount == 0) break;
                    else openingCount--;
                    
                } else if(code[*code_poiner] == '[') {
                    openingCount++;
                } 
            }
        }
        break;
    
    case ']':
        if(buf[*mem_pointer] != 0) {
            int closingCount = 0;
            while(true) {
                *code_poiner = *code_poiner - 1;

                if(*code_poiner < 0) break;

                if(code[*code_poiner] == '[') {
                    if(closingCount == 0) break;
                    else closingCount--;
                } else if(code[*code_poiner] == ']') {
                    closingCount++;
                }
            }
        }
        break;
    
    case '.':
        output((char)buf[*mem_pointer], output_buf, mode);
        break;

    case ',':
        buf[*mem_pointer] = getchar();
        break;
    
    case '#':
        *code_poiner = *code_poiner + 1;
        return 1;
    }

    *code_poiner = *code_poiner + 1;

    return 0;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage: ./bint <path> <mode>");
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

    int code_pointer = 0;
    int mem_pointer = 0;
    char *buf = calloc(MEMSIZE, sizeof(int));
    char *output_buf = calloc(200, sizeof(char));

    char *code = malloc((strlen(src_buf) + 1) * sizeof(char));
    prepareCode(src_buf, code);
    free(src_buf);

    close(fd);

    if (argv[2][0] == '1') {
        initAll();
    }

    int execMode = EXEC_SI;
    while(code_pointer < strlen(code)) {
        if (argv[2][0] == '1') {

            clear();

            drawCode(code, 75, code_pointer);
            drawState(code_pointer, mem_pointer, buf, code);
            mvprintw(2, 2, "Output: %s", output_buf);

            int execCode = executeOne(code, &code_pointer, &mem_pointer, buf, output_buf, 1);

            int end = 0;

            if(execMode == EXEC_DEBUG && execCode == 1) {
                execMode = EXEC_SI;
            }

            if(execMode == EXEC_SI) {
                switch(getch()) {
                    
                    case 'q':
                        end = 1;
                        break;
                    
                    case 'd':
                        execMode = EXEC_DEBUG;
                        break;

                    case 'r':
                        execMode = EXEC_RUN;
                        break;
                }
            }

             if(end) break;           
            
            refresh();
        } else {
            executeOne(code, &code_pointer, &mem_pointer, buf, output_buf, 0);
        }
    }

    if (argv[2][0] == '1') {
        endwin();
    }

    free(code);
    free(output_buf);

    return 0;
}