#include <stdio.h>
#include <string.h>

#include "include/screen.h"
#include "include/memory.h"

void initScreen(Screen *screen) {
	screen->curX = 0;
	screen->curY = 0;
	screen->pixels = nullptr;
	screen->width = 0;
	screen->height = 0;
	screen->mode = MONOCHROME;
}

void freeScreen(Screen* screen) {
	FREEARRAY(uint16_t, screen->pixels, screen->width * screen->height);
	initScreen(screen);
}

static void printScreen(Screen* screen) {
	printf("\x1b[H");

	for (uint8_t r = 0; r < screen->height; r++) {
		for (uint8_t c = 0; c < screen->width; c++){
			uint16_t pixel = screen->pixels[r * screen->width + c];
			uint8_t character = (pixel & 0xff);
			uint8_t colourIndex = (pixel & 0xff00) >> 8;
			switch (screen->mode) {
				case FOURBIT: {
						 printf("\x1b[%um%c\x1b[39m", colourIndex, character);
						 break;
					      }
				case EIGHTBIT: {
						 printf("\x1b[38;5;%um%c\x1b[39m", colourIndex, character);
						 break;
					       }
				case MONOCHROME:
				default:
						 putchar(character);
						 break;
			}
		}
		putchar('\n');
	}
}

void screenSetup(Screen *screen, uint8_t width, uint8_t height, ColourMode mode) {
	uint8_t oldSize = screen->width * screen->height;
	screen->pixels = GROW_ARRAY(uint16_t, screen->pixels, oldSize, width * height);
	screen->mode = mode;
	screen->width = width;
	screen->height = height;
	memset(screen->pixels, '0', width * height * sizeof(uint16_t));
}

void drawCharacter(Screen *screen, uint8_t x, uint8_t y, uint8_t colourIndex, uint8_t character) {
	if (x >= 0 && x < screen->width && y >= 0 && y < screen->height) {
		uint16_t pixel = (colourIndex << 8) | character;
		screen->pixels[y * screen->width + x] = pixel;
		printScreen(screen);
	}
}

void clearScreen(Screen *screen) {
	memset(screen->pixels, ' ', screen->width * screen->height * sizeof(uint16_t));
	printScreen(screen);
}

void setCursor(Screen *screen, uint8_t x, uint8_t y) {
	if (x >= 0 && x < screen->width && y >= 0 && y < screen->height) {
		screen->curX = x;
		screen->curY = y;
	}
}

void drawAtCursor(Screen *screen, uint8_t character, uint8_t colourIndex) {
	uint8_t index = screen->curY * screen->width + screen->curX;
	uint16_t pixel = (colourIndex << 8) | character;
	screen->pixels[index] = pixel;
	printScreen(screen);
}
