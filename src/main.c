
#include "include/chunk.h"
#include "include/debug.h"
#include "include/vm.h"

int main(void) {
	VM vm = {};
	Chunk chunk = {};
	writeChunk(&chunk, SS);
	writeChunk(&chunk, 3);
	writeChunk(&chunk, 3);
	writeChunk(&chunk, 4);
	writeChunk(&chunk, 1);
	writeChunk(&chunk, DC);
	writeChunk(&chunk, 4);
	writeChunk(&chunk, 2);
	writeChunk(&chunk, 3);
	writeChunk(&chunk, 31);
	writeChunk(&chunk, '*');
	writeChunk(&chunk, CM);
	writeChunk(&chunk, 2);
	writeChunk(&chunk, 1);
	writeChunk(&chunk, 1);
	writeChunk(&chunk, DAC);
	writeChunk(&chunk, 2);
	writeChunk(&chunk, '$');
	writeChunk(&chunk, 92);
	writeChunk(&chunk, EOS);
	//disassembleChunk(&chunk, "test chunk");
	interpret(&vm, &chunk);
	
	freeVM(&vm);
	freeChunk(&chunk);
	return 0;
}
