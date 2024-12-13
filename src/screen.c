#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "include/screen.h"
#include "include/memory.h"

void initScreen(Screen* screen) {
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

void printScreen(Screen* screen) {
	//printf("\x1b[%uA\x1b[%uD", screen->height, screen->width);
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

void screenSetup(Screen* screen, uint8_t width, uint8_t height, ColourMode mode) {
	size_t oldSize = screen->width * screen->height;
	bool initialise = true;
	if (screen->pixels) {
		initialise = false;
	}
	screen->pixels = GROW_ARRAY(uint16_t, screen->pixels, oldSize, width * height);
	screen->mode = mode;
	screen->width = width;
	screen->height = height;
	if (initialise)
		memset(screen->pixels, ' ', width * height * sizeof(uint16_t));
}

void drawCharacter(Screen* screen, uint8_t x, uint8_t y, uint8_t colourIndex, uint8_t character) {
	if (x >= 0 && x < screen->width && y >= 0 && y < screen->height) {
		uint16_t pixel = (colourIndex << 8) | character;
		screen->pixels[y * screen->width + x] = pixel;
	}
}

void clearScreen(Screen* screen) {
	memset(screen->pixels, ' ', screen->width * screen->height * sizeof(uint16_t));
}

void setCursor(Screen* screen, uint8_t x, uint8_t y) {
	if (x >= 0 && x < screen->width && y >= 0 && y < screen->height) {
		screen->curX = x;
		screen->curY = y;
	}
}

void drawAtCursor(Screen* screen, uint8_t character, uint8_t colourIndex) {
	size_t index = screen->curY * screen->width + screen->curX;
	uint16_t pixel = (colourIndex << 8) | character;
	screen->pixels[index] = pixel;
}

void renderText(Screen* screen, uint8_t x, uint8_t y, uint8_t colourIndex, uint8_t* str, uint8_t len) {
	if (x >= 0 && x < screen->width && y >= 0 && y < screen->height) {
		size_t startIndex = y * screen->width + x;
		int overflows = (startIndex + len) - (screen->width * screen->height);
		if (overflows > 0)
			for (size_t i = startIndex, j = 0; i < (screen->width * screen->height); i++) {
				uint16_t pixel = (colourIndex << 8) | str[j++];
				screen->pixels[i] = pixel;
			}
		else
			for (size_t i = startIndex, j = 0; j < len; i++) {
				uint16_t pixel = (colourIndex << 8) | str[j++];
				screen->pixels[i] = pixel;
			}
	}
}

void drawLine(Screen *screen, uint8_t x_0, uint8_t y_0, uint8_t x_1, uint8_t y_1, uint8_t colourIndex, uint8_t character) {
	int dx = abs(x_1 - x_0);
	int sx = x_0 < x_1 ? 1: -1;
	int dy = -abs(y_1 - y_0);
	int sy = y_0 < y_1 ? 1: -1;
	int e = dx + dy;

	for (;;) {
		drawCharacter(screen, x_0, y_0, colourIndex, character);
		if (x_0 == x_1 && y_0 == y_1) break;
		int e_2 = 2 * e;
		if (e_2 >= dy) {
			if (x_0 == x_1) break;
			e += dy;
			x_0 += sx;
		}
		if (e_2 <= dx) {
			if (y_0 == y_1) break;
			e += dx;
			y_0 += sy;
		}
	}
}
