#if !defined (scanner_h)
#define scanner_h

#include <stdbool.h>

/**
 * enum tokenType - all the possible tokens.
 */
typedef enum tokenType{
	// single character tokens
	TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
	TOKEN_COMMA, TOKEN_SEMICOLON,
	// literals
	TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
	// keywords
	TOKEN_SCREEN, TOKEN_MOVE_CUROR, TOKEN_DRAW_AT_CURSOR,
	TOKEN_RENDER_TEXT, TOKEN_CLR_SCREEN, TOKEN_DRAW_LINE,
	TOKEN_DRAW_CHARACTER,

	TOKEN_EOF, TOKEN_ERROR
} TokenType;

/**
 * struct scanner - declares a new 'Scanner' type which processes
 * a string of input.
 * @start: pointer to the beginning of the current lexeme being scanned.
 * @current: pointer to the current character of the lexeme being scanned.
 */
typedef struct scanner {
	char const* start;
	char const* current;
} Scanner;

/**
 * struct token - declares a new 'Token' type which represents
 * the properties of a token.
 * @type: the type of token.
 * @length: the length of the underlying lexeme that constitutes the token.
 * @start: the beginning of the underlying lexeme that constitutes the token.
 */
typedef struct token {
	TokenType type;
	unsigned length;
	char const* start;
} Token;

/**
 * initScanner - initialises the state of a 'Scanner' type.
 * @scanner: pointer to a 'Scanner' type.
 * @source: pointer to a string of input to being scanned.
 * @Returns: void.
 */
void initScanner(Scanner* scanner, char const* source);
/**
 * scanToken - generates tokens based off of the lexeme that
 * is currently being processed.
 * @scanner: pointer to a 'Scanner' type.
 * @Returns: Token.
 */
Token scanToken(Scanner* scanner);

#endif
