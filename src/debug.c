#include <stdio.h>

#include "include/debug.h"

void disassembleChunk(Chunk* chunk, char const* name) {
	printf("== %s ==\n", name);

	for (unsigned offset = 0; offset < chunk->count;) {
		offset = disassembleInstruction(chunk, offset);
	}
}

static void printArguments(Chunk* chunk, unsigned offset, unsigned length) {
	for (unsigned i = offset, j = 0; j < length; i++, j++) {
		printf("%u ", chunk->code[i]);
	}
}

static unsigned constantInstruction(char const* name, Chunk* chunk, unsigned offset) {
	unsigned length = chunk->code[offset + 1];
	printf("%-16s %4u '", name, length);
	printArguments(chunk, offset + 2, length);
	printf("'\n");
	return offset + 2 + length;
}

static unsigned simpleInstruction(char const* name, unsigned offset) {
	printf("%s\n", name);
	return offset + 1;
}


unsigned disassembleInstruction(Chunk *chunk, unsigned offset) {
	printf("%04u ", offset);

	uint8_t instruction = chunk->code[offset];
	switch (instruction) {
		case EOS:
			return simpleInstruction("END_OF_FILE", offset);
		case SS:
			return constantInstruction("SCREEN_SETUP", chunk, offset);
		case DC:
			return constantInstruction("DRAW_CHARACTER", chunk, offset);
		case DL:
			return constantInstruction("DRAW_LINE", chunk, offset);
		case RT:
			return constantInstruction("RENDER_TEXT", chunk, offset);
		case CM:
			return constantInstruction("CURSOR_MOVEMENT", chunk, offset);
		case DAC:
			return constantInstruction("DRAW_AT_CURSOR", chunk, offset);
		case CLR:
			return simpleInstruction("CLEAR_SCREEN", offset);
		default:
			printf("Unknown command %u\n", instruction);
			return offset + 1;
	}
}
