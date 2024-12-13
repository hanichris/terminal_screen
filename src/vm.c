#include <stdio.h>

#include "include/debug.h"
#include "include/memory.h"
#include "include/screen.h"
#include "include/vm.h"

void initVM(VM* vm) {
	vm->screen = GROW_ARRAY(Screen, vm->screen, 0, 1);
	initScreen(vm->screen);
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
			case SS: {
					 [[maybe_unused]]uint8_t length = READ_BYTE();
					 uint8_t screenWidth = READ_BYTE();
					 uint8_t screenHeight = READ_BYTE();
					 ColourMode colourMode = MONOCHROME;
					 uint8_t mode = READ_BYTE();
					 if (mode == EIGHTBIT || mode == FOURBIT)
						 colourMode = mode;
					 screenSetup(
							 vm->screen, 
							 screenWidth, 
							 screenHeight, 
							 colourMode
					);
					 break;
				}
			case DC: {
					 if (vm->screen->pixels) {
						 [[maybe_unused]]uint8_t length = READ_BYTE();
						 uint8_t xCoord = READ_BYTE();
						 uint8_t yCoord = READ_BYTE();
						 uint8_t colourIndex = READ_BYTE();
						 uint8_t character = READ_BYTE();
						 drawCharacter(vm->screen, xCoord, yCoord, colourIndex, character);
						 printScreen(vm->screen);
					 }
					 break;
				 }
			case CM: {
					 if (vm->screen->pixels) {
						 [[maybe_unused]]uint8_t length = READ_BYTE();
						 uint8_t xCoord = READ_BYTE();
						 uint8_t yCoord = READ_BYTE();
						 setCursor(vm->screen, xCoord, yCoord);
					 }
					 break;
				 }
			case DAC: {
					  if (vm->screen->pixels) {
						  [[maybe_unused]]uint8_t length = READ_BYTE();
						  uint8_t character = READ_BYTE();
						  uint8_t colourIndex = READ_BYTE();
						  drawAtCursor(vm->screen, character, colourIndex);
						  printScreen(vm->screen);
					  }
					  break;
				  }
			case CLR: {
					  if (vm->screen->pixels) {
						  [[maybe_unused]]uint8_t length = READ_BYTE();
						  clearScreen(vm->screen);
						  printScreen(vm->screen);
					  }
					  break;
				  }
			case RT: {
					 if (vm->screen->pixels) {
						 uint8_t length = READ_BYTE();
						 uint8_t xCoord = READ_BYTE();
						 uint8_t yCoord = READ_BYTE();
						 uint8_t colourIndex = READ_BYTE();
						 uint8_t buffer[length - 3 + 1] = { };
						 for (uint8_t i = 0; i < length - 3; i++)
							 buffer[i] = READ_BYTE();
						 renderText(
							vm->screen,
							xCoord,
							yCoord,
							colourIndex,
							buffer,
							length - 3);
						 printScreen(vm->screen);
					 }
					 break;
				 }
			case DL: {
					 if (vm->screen->pixels) {
						 [[maybe_unused]]uint8_t length = READ_BYTE();
						 uint8_t x0 = READ_BYTE();
						 uint8_t y0 = READ_BYTE();
						 uint8_t x1 = READ_BYTE();
						 uint8_t y1 = READ_BYTE();
						 uint8_t colourIndex = READ_BYTE();
						 uint8_t character = READ_BYTE();
						 drawLine(vm->screen, x0, y0, x1, y1, colourIndex, character);
						 printScreen(vm->screen);
					 }
					 break;
				 }
			case END:
				  return INTERPRET_EOF;
			case EOS:
				  return INTERPRET_OK;
		}
	}
#undef READ_BYTE
}

InterpretResult interpret(VM *vm, Chunk *chunk) {
	vm->chunk = chunk;
	vm->ip = chunk->code;
	printf("\x1b[1J\x1b[H");
	return run(vm);
}
