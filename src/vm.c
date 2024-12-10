#include <stdio.h>

#include "include/debug.h"
#include "include/memory.h"
#include "include/screen.h"
#include "include/vm.h"

void initVM(VM* vm) {

}

void freeVM(VM* vm) {
	freeScreen(vm->screen);
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
			case SS: {
					 [[maybe_unused]]uint8_t length = READ_BYTE();
					 uint8_t screenWidth = READ_BYTE();
					 uint8_t screenHeight = READ_BYTE();
					 ColourMode colourMode = READ_BYTE();
					 screenSetup(
							 vm->screen, 
							 screenWidth, 
							 screenHeight, 
							 colourMode
					);
					 break;
				}
			case DC: {
					 if (vm->screen) {
						 [[maybe_unused]]uint8_t length = READ_BYTE();
						 uint8_t xCoord = READ_BYTE();
						 uint8_t yCoord = READ_BYTE();
						 uint8_t colourIndex = READ_BYTE();
						 uint8_t character = READ_BYTE();
						 drawCharacter(vm->screen, xCoord, yCoord, colourIndex, character);
					 }
					 break;
				 }
			case CM: {
					 if (vm->screen) {
						 [[maybe_unused]]uint8_t length = READ_BYTE();
						 uint8_t xCoord = READ_BYTE();
						 uint8_t yCoord = READ_BYTE();
						 setCursor(vm->screen, xCoord, yCoord);
					 }
					 break;
				 }
			case DAC: {
					  if (vm->screen) {
						  [[maybe_unused]]uint8_t length = READ_BYTE();
						  uint8_t character = READ_BYTE();
						  uint8_t colourIndex = READ_BYTE();
						  drawAtCursor(vm->screen, character, colourIndex);
					  }
					  break;
				  }
		}
	}
#undef READ_BYTE
}

InterpretResult interpret(VM *vm, Chunk *chunk) {
	vm->chunk = chunk;
	vm->ip = chunk->code;
	vm->screen = GROW_ARRAY(Screen, vm->screen, 0, 1);
	initScreen(vm->screen);
	printf("\x1b[1J\x1b[H");
	return run(vm);
}
