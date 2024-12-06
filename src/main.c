#include "include/chunk.h"
#include "include/debug.h"

int main(void) {
	Chunk chunk = { };
	writeChunk(&chunk, EOS);
	disassembleChunk(&chunk, "test chunk");
	freeChunk(&chunk);
	return 0;
}
