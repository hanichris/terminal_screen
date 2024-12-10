#if !defined(screen_h)
#define screen_h

#include "common.h"

enum colourmode : uint8_t {
	MONOCHROME,
	FOURBIT,
	EIGHTBIT,
};

typedef enum colourmode ColourMode;

/**
 * struct screen_t - defines a screen object type.
 * @pixels: pointer to an array that defines the
 * screen.
 * @width: width of the screen.
 * @height: height of the screen.
 * @curX: x-coordinate of the cursor.
 * @curY: y-coordinate of the cursor.
 * @mode: colour mode of the screen. The screen's colour mode is either
 * monochromatic, supports 4-bit colour or supports 8-bit colour.
 */
typedef struct screen_t {
	uint16_t* pixels;
	uint8_t width;
	uint8_t height;
	uint8_t curX;
	uint8_t curY;
	ColourMode mode;
} Screen;

/**
 * initScreen - initialises a 'Screen' type to ensure it is in a valid state
 * before use.
 * @screen: pointer to a 'Screen' type.
 * @Returns: void.
 */
void initScreen(Screen *screen);
/**
 * screenSetup - sets up the screen for use by defining its dimensions and colour
 * mode.
 * @screen: pointer to a 'Screen' type.
 * @width: width of the screen object.
 * @height: height of the screen object.
 * @mode: colour mode of the screen object.
 * @Returns: void.
 */
void screenSetup(Screen *screen, uint8_t width, uint8_t height, ColourMode mode);
/**
 * freeScreen - frees up the memory used to create the array that represents a screen.
 * @screen: pointer to a 'Screen' type.
 * @Returns: void.
 */
void freeScreen(Screen *screen);
/**
 * clearScreen - clears the screen of any previous renderings readying it for new
 * use.
 * @screen: pointer to a 'Screen' type.
 * @Returns: void.
 */
void clearScreen(Screen *screen);
/**
 * drawCharacter - draws an ASCII character at the specified coordinates of the screen.
 * If the provided coordinates are out of bound, the character is not drawn on screen.
 * @screen: pointer to a 'Screen' type.
 * @x: x-coordinate of where the character will be drawn.
 * @y: y-coordinate of where the character will be drawn.
 * @colourIndex: the colour index to use during rendering of the character.
 * @character: the ASCII character to draw on the screen.
 * @Return: void.
 */
void drawCharacter(Screen *screen, uint8_t x, uint8_t y, uint8_t colourIndex, uint8_t character);
/**
 * setCursor - moves the cursor to a specifc location on the screen without drawing on it.
 * @screen: pointer to a 'Screen' type.
 * @x: x-coordinate of where the cursor will be moved to.
 * @y: y-coordinate of where the cursor will be moved to.
 * @Returns: void.
 */
void setCursor(Screen* screen, uint8_t x, uint8_t y);
/**
 * drawAtCursor - draws a character at the cursor position.
 * @screen: pointer to a 'Screen' type.
 * @character: the ASCII character to render at the cursor position.
 * @colourIndex: the colour index to use during rendering of the character.
 * @Returns: void.
 */
void drawAtCursor(Screen* screen, uint8_t character, uint8_t colourIndex);
#endif
