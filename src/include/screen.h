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
 * printScreen - prints out the current buffer of the screen to the terminal.
 * @screen: pointer to a 'Screen' type.
 * @Returns: void.
 */
void printScreen(Screen* screen);

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
/**
 * renderText = renders a text of string whose length is not greater than 252 characters
 * on the screen starting at the coordinates (x,y).
 * @screen: pointer to a 'Screen' type.
 * @x: x-coordinate of the starting point of the string to be rendered.
 * @y: y-coordinate of the starting point of the string to be rendered.
 * @colourIndex:the colour index to use during rendering of the string.
 * @str: the string to be rendered (only ASCII characters are supported).
 * @len: length of the string to be rendered.
 * @Returns: void.
 * */
void renderText(Screen* screen, uint8_t x, uint8_t y, uint8_t colourIndex, uint8_t* str, uint8_t len);
/**
 * drawLine - draws a line on the screen starting at coordinate (x0, y0) to coordinate (x1, y1) in the
 * desired colour using the provided character as a line.
 * @screen: pointer to a 'Screen' type.
 * @x_0: x-coordinate of the starting point of the line.
 * @y_0: y-coordinate of the starting point of the line.
 * @x_1: x-coordinate of the ending point of the line.
 * @y_1: y-coordinate of the ending point of the line.
 * @colourIndex: the colour index to use when rendering the line.
 * @character: the character to draw the line using.
 * @Returns: void.
 */
void drawLine(Screen* screen, uint8_t x_0, uint8_t y_0, uint8_t x_1, uint8_t y_1, uint8_t colourIndex, uint8_t character);
#endif
