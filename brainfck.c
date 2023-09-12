#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 30000
#define CAP 10

typedef struct {
	int cap;
	int size;
	int *items;
} STACK;

typedef STACK* Stack;

Stack createStack(void);
void push(int, Stack);
void overflow(Stack);
int pop(Stack);
int top(Stack);

int main(int argc, char **argv)
{
	char mem[BUFSIZE];
	FILE *program;
	Stack loopStarts;
	int i, ch, memptr, sp;
	
	if (argc != 2) {
		fprintf(stderr, "[ERROR] Usage: %s <.bf_filename>\n", argv[0]);
		return 1;
	}
	program = fopen(argv[1], "r");
	if (program == NULL) {
		fprintf(stderr, "[ERROR]: Failed to open .bf file\n");
		return 1;
	}

	for (i = 0; i < BUFSIZE; i++)
		mem[i] = 0;
	
	loopStarts = createStack();
	memptr = 0;		

	while (1) {
		ch = fgetc(program);
		if (feof(program))
			break;

		switch (ch) {
		case '+':
			mem[memptr]++;
			break;

		case '-':
			mem[memptr]--;
			break;

		case '>':
			memptr = (memptr + 1) % BUFSIZE;
			break;

		case '<':
			memptr = (memptr > 0) ? memptr - 1 : BUFSIZE - 1;
			break;

		case '.':
			putchar(mem[memptr]);
			break;

		case ',':
			mem[memptr] = getchar();
			break;

		case '[':
			if (mem[memptr] == 0)
				while ((ch = fgetc(program)) != ']')
					;
			else
				push(ftell(program), loopStarts);
			
			break;

		case ']':
			if (mem[memptr] == 0)
				pop(loopStarts);
			else 
				fseek(program, top(loopStarts), SEEK_SET);
			break;

		default:
			break;
		}
	}
	puts("");
	if (loopStarts->size != 0) {
		fprintf(stderr, "[ERROR]: Unclosed loop\n");
		return 1;
	}

	return 0;
}

Stack createStack(void)
{
	Stack st;
	st = malloc(sizeof *st);
	if (st == NULL) {
		fprintf(stderr, "[ERROR]: Out of memory\n");
		exit(1);
	}
	st->cap = CAP;
	st->size = 0;
	st->items = malloc(sizeof *st->items * st->cap);
	
	if (st->items == NULL) {
		fprintf(stderr, "[ERROR]: Out of memory\n");
		exit(1);
	}
	
	return st;
}

void push(int k, Stack st)
{
	if (st->cap == st->size) {
		overflow(st);
	}
	st->items[st->size++] = k;
	return;
}

void overflow(Stack st)
{
	int *newVec;
	st->cap *= 2;
	newVec = malloc(sizeof *st->items * st->cap);
	free(st->items);
	st->items = newVec;

	return;
}

int pop(Stack st)
{
	if (st->size <= 0) {
		fprintf(stderr, "[ERROR]: Closing unopened loop\n");
		exit(1);
	}
	return st->items[--st->size];
}

int top(Stack st)
{
	if (st->size <= 0) {
		fprintf(stderr, "[ERROR]: Closing unopened loop\n");
		exit(1);
	}
	return st->items[st->size - 1];
}
