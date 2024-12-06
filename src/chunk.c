#include "include/chunk.h"
#include "include/memory.h"

void initChunk(Chunk *chunk) {
	chunk->capacity = 0;
	chunk->count = 0;
	chunk->code = nullptr;
}

void writeChunk(Chunk *chunk, uint8_t byte) {
	if (chunk->capacity < chunk->count + 1) {
		unsigned oldCapacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(oldCapacity);
		chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
	}

	chunk->code[chunk->count] = byte;
	chunk->count++;
}

void freeChunk(Chunk *chunk) {
	FREEARRAY(uint8_t, chunk->code, chunk->capacity);
	initChunk(chunk);
}