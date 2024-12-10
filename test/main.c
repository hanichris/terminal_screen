#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sysexits.h>

#include "compiler.h"


static void repl() {
	char* line = nullptr;
	size_t linecap = 0;

	for (;;) {
		printf("> ");
		if (getline(&line, &linecap, stdin) == -1) {
			if (feof(stdin)) {
				break;
			} else {
				perror("readline");
				exit(EXIT_FAILURE);
			}
		}
		compile(line);
		free(line);
		line = nullptr;
		linecap = 0;
	}
	printf("\n");
}


int main(int argc, [[maybe_unused]]char* argv[argc]) {
	switch (argc) {
		case 1:
			repl();
			break;
		default:
			errx(EX_USAGE, "Does not accept command line arguments.");
	}
	exit(EXIT_SUCCESS);
}
