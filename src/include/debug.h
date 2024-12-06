#if !defined(debug_h)
#define debug_h

#include "chunk.h"

void disassembleChunk(Chunk* chunk, char const* name);
unsigned disassembleInstruction(Chunk* chunk, unsigned offset);

#endif
