#if !defined(debug_h)
#define debug_h

#include "chunk.h"

/**
 * disassembleChunk - Disassembles the stream of bytes stored
 * internally in a chunk.
 * @chunk: a pointer to a 'Chunk' type.
 * @name: header for the disassembly operation.
 * @Returns: void.
 */
void disassembleChunk(Chunk* chunk, char const* name);
/**
 * disassembleInstruction - Disassembles each instruction in the
 * stream of bytes.
 * @chunk: a pointer to a 'Chunk' type.
 * @offset: iterator.
 * Returns: the 'offset' of the next instruction.
 */
unsigned disassembleInstruction(Chunk* chunk, unsigned offset);

#endif
