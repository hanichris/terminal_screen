#include <stdio.h>

#include "include/debug.h"


static unsigned simpleInstruction(char const* name, unsigned offset) {
	printf("%s\n", name);
	return offset + 1;
}

void disassembleChunk(Chunk* chunk, char const* name) {
	printf("== %s ==\n", name);

	for (unsigned offset = 0; offset < chunk->count;) {
		offset = disassembleInstruction(chunk, offset);
	}
}

unsigned disassembleInstruction(Chunk *chunk, unsigned offset) {
	printf("%04u ", offset);

	uint8_t instruction = chunk->code[offset];
	switch (instruction) {
		case EOS:
			return simpleInstruction("End of file", offset);
		default:
			printf("Unknown command %u\n", instruction);
			return offset + 1;
	}
}
