#include "include/debug.h"
#include "include/vm.h"

void initVM(VM* vm) {
	vm->ip = nullptr;
	initChunk(vm->chunk);
	initScreen(vm->screen);
}

void freeVM(VM* vm) {

}

static InterpretResult run(VM* vm) {
#define READ_BYTE() (*vm->ip++)

	for (;;) {
#if defined(DEBUG_TRACE_EXECUTION)
		disassembleInstruction(vm->chunk, (unsigned)(vm->ip - vm->chunk->code));
#endif
		uint8_t instruction = READ_BYTE();
		switch (instruction) {
			case EOS:
				return INTERPRET_OK;
		}
	}
#undef READ_BYTE
}

InterpretResult interpret(VM *vm, Chunk *chunk) {
	vm->chunk = chunk;
	vm->ip = chunk->code;
	return run(vm);
}
