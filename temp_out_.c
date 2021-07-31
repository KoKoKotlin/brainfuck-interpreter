#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#define MEMSIZE 9000

int main() {

	char *mem = calloc(sizeof(char), MEMSIZE);
	int mempointer = 0;	mem[mempointer] += 1;
	while(mem[mempointer] != 0) {
		mem[mempointer] -= 1;
		while(mem[mempointer] != 0) {
			mempointer -= 2;
			while(mem[mempointer] != 0) {
				mem[mempointer] += 1;
				while(mem[mempointer] != 0) {
					mem[mempointer] -= 3;
					mempointer += 1;
					
				}
				mem[mempointer] -= 1;
				while(mem[mempointer] != 0) {
					mempointer -= 3;
					
				}
				
			}
			
		}
		mempointer += 3;
		mem[mempointer] -= 1;
		
	}
	mempointer += 1;
	mem[mempointer] -= 1;
	printf("%c", mem[mempointer]);
	mem[mempointer] -= 3;
	printf("%c", mem[mempointer]);
	mempointer += 1;
	printf("%c", mem[mempointer]);
	printf("%c", mem[mempointer]);
	mempointer += 1;
	printf("%c", mem[mempointer]);
	mempointer -= 4;
	mem[mempointer] -= 1;
	printf("%c", mem[mempointer]);
	mempointer -= 1;
	mem[mempointer] += 1;
	printf("%c", mem[mempointer]);
	mempointer += 5;
	printf("%c", mem[mempointer]);
	mempointer += 1;
	printf("%c", mem[mempointer]);
	mempointer -= 2;
	printf("%c", mem[mempointer]);
	mempointer -= 1;
	mem[mempointer] -= 1;
	printf("%c", mem[mempointer]);

	return 0;
}