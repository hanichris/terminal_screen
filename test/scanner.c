#include <string.h>

#include "scanner.h"

void initScanner(Scanner *scanner, const char *source) {
	scanner->current = source;
	scanner->start = source;
}

static bool isAlpha(char c) {
	return (c >= 'a' && c <= 'z') ||
		(c >= 'A' && c <= 'Z') ||
		c == '_';
}

static bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

static bool isAtEnd(Scanner* scanner) {
	return *scanner->current == '\0';
}

static char advance(Scanner* scanner) {
	scanner->current++;
	return scanner->current[-1];
}

static char peek(Scanner* scanner) {
	return *scanner->current;
}

static Token makeToken(Scanner* scanner, TokenType type) {
	Token token = (Token) {
		.type = type,
		.start = scanner->start,
		.length = (unsigned)(scanner->current - scanner->start)
	};
	return token;
}

static Token errorToken(Scanner* scanner, char const* message) {
	Token token = (Token) {
		.type = TOKEN_ERROR,
		.start = message,
		.length = (unsigned)strlen(message)
	};
	return token;
}

static void skipWhitespace(Scanner* scanner) {
	for (;;) {
		char c = peek(scanner);
		switch(c) {
			case ' ':
			case '\r':
			case '\n':
			case '\t':
				advance(scanner);
				break;
			default:
				return;
		}
	}
}

static TokenType checkKeyword(
		Scanner* scanner, unsigned start, unsigned length,
		char const* rest, TokenType type
) {
	if (scanner->current - scanner->start == start + length &&
	    memcmp(scanner->start + start, rest, length) == 0)
		return type;
	return TOKEN_IDENTIFIER;
}

static TokenType identifierType(Scanner* scanner) {
	switch (scanner->start[0]) {
		case 's': return checkKeyword(scanner, 1, 5, "creen", TOKEN_SCREEN);
		case 'm': return checkKeyword(scanner, 1, 3, "ove", TOKEN_MOVE_CUROR);
		case 'r': return checkKeyword(scanner, 1, 5, "ender", TOKEN_RENDER_TEXT);
		case 'l': return checkKeyword(scanner, 1, 3, "ine", TOKEN_DRAW_LINE);
		case 'c': return checkKeyword(scanner, 1, 4, "lear", TOKEN_CLR_SCREEN);
		case 'd':
			 if (scanner->current - scanner->start > 1) {
				 switch(scanner->start[1]) {
					case 'r': return checkKeyword(scanner, 2, 2, "aw", TOKEN_DRAW_CHARACTER);
					case 'c': return checkKeyword(scanner, 2, 2, "ur", TOKEN_DRAW_AT_CURSOR);
				 }
			 } 
	}
	return TOKEN_IDENTIFIER;
}

static Token identifier(Scanner* scanner) {
	while (isAlpha(peek(scanner)) || isDigit(peek(scanner)))
		advance(scanner);
	return makeToken(scanner, identifierType(scanner));
}
static Token number(Scanner* scanner) {
	while (isDigit(peek(scanner)))
		advance(scanner);
	return makeToken(scanner, TOKEN_NUMBER);
}

static Token string(Scanner* scanner) {
	while (peek(scanner) != '"' && !isAtEnd(scanner))
		advance(scanner);
	if (isAtEnd(scanner)) return errorToken(scanner, "Unterminated string.");

	advance(scanner);
	return makeToken(scanner, TOKEN_STRING);
}

Token scanToken(Scanner* scanner) {
	skipWhitespace(scanner);
	scanner->start = scanner->current;

	if (isAtEnd(scanner)) return makeToken(scanner, TOKEN_EOF);

	char c = advance(scanner);
	if (isDigit(c)) return number(scanner);
	if (isAlpha(c)) return identifier(scanner);
	switch (c) {
		case '(': return makeToken(scanner, TOKEN_LEFT_PAREN);
		case ')': return makeToken(scanner, TOKEN_RIGHT_PAREN);
		case ',': return makeToken(scanner, TOKEN_COMMA);
		case ';': return makeToken(scanner, TOKEN_SEMICOLON);
		case '"': return string(scanner);
	}

	return errorToken(scanner, "Unexpected character.");
}
