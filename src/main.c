#include "include/chunk.h"
#include "include/debug.h"
#include "include/vm.h"

int main(void) {
	VM vm = { };
	Chunk chunk = { };
	writeChunk(&chunk, EOS);
	disassembleChunk(&chunk, "test chunk");
	interpret(&vm, &chunk);
	freeVM(&vm);
	freeChunk(&chunk);
	return 0;
}
