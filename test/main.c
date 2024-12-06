#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sysexits.h>

static void repl() {
	printf("RUNNING A REPL\n");
}


int main(int argc, [[maybe_unused]]char* argv[argc + 1]) {
	switch (argc) {
		case 1:
			repl();
			break;
		default:
			errx(EX_USAGE, "Does not accept command line arguments.");
	}
	exit(EXIT_SUCCESS);
}
