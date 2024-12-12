#include "compiler.h"
#include "scanner.h"

typedef struct {
	Token current;
	Token previous;
	bool hadError;
	bool panicMode;
} Parser;

typedef enum {
	PREC_NONE,
	PREC_BASE,
	PREC_CALL,    // ()
	PREC_PRIMARY
} Precedence;

typedef void (*ParseFn)(Parser* parser, Scanner* scanner);

typedef struct {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;

Chunk* compilingChunk;

static Chunk* currentChunk() {
	return compilingChunk;
}

static void errorAt(Parser* parser, Token* token, char const* message) {
	if (parser->panicMode) return;
	parser->panicMode = true;
	fprintf(stderr, "Error ");
	if (token->type == TOKEN_EOF) {
		fprintf(stderr, " at end");
	} else if (token->type == TOKEN_ERROR) {

	} else {
		fprintf(stderr, " at '%.*s'", token->length, token->start);
	}
	fprintf(stderr, ": %s\n", message);
	parser->hadError = true;
}

static void error(Parser* parser, char const* message) {
	errorAt(parser, &parser->previous, message);
}

static void errorAtCurrent(Parser* parser, char const* message) {
	errorAt(parser, &parser->current, message);
}

static void advance(Parser* parser, Scanner* scanner) {
	parser->previous = parser->current;

	for (;;) {
		parser->current = scanToken(scanner);
		if (parser->current.type != TOKEN_ERROR) break;

		errorAtCurrent(parser, parser->current.start);
	}
}

static void consume(Parser* parser, Scanner* scanner, TokenType type, char const* message) {
	if (parser->current.type == type) {
		advance(parser, scanner);
		return;
	}
	errorAtCurrent(parser, message);
}

static void emitByte(uint8_t byte) {
	writeChunk(currentChunk(), byte);
}

static void emitBytes(uint8_t byte1, uint8_t byte2) {
	emitByte(byte1);
	emitByte(byte2);
}

static void endCompiler() {
	emitByte(END);
}

static void expression(Parser* parser, Scanner* scanner);
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Parser* parser, Scanner* scanner, Precedence precedence);

static void grouping(Parser* parser, Scanner* scanner) {
	expression(parser, scanner);
	consume(parser, scanner, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number(Parser* parser, Scanner* scanner) {
	unsigned long value = strtoul(parser->previous.start, nullptr, 10);
	if (value > UINT8_MAX) {
		error(parser, "Numerical literal cannot be greater than 255.");
		return;
	}
	emitByte((uint8_t)value);
}

static void string(Parser* parser, Scanner* scanner) {
	unsigned size = parser->previous.length - 2;
	char const* ptr = parser->previous.start + 1;

	for (unsigned i = 0; i < size; i++) {
		emitByte(ptr[i]);
	}
}

static void expression(Parser* parser, Scanner* scanner) {
	parsePrecedence(parser, scanner, PREC_BASE);
}

ParseRule rules[] = {
	[TOKEN_LEFT_PAREN]      = {grouping, NULL, PREC_NONE},
	[TOKEN_RIGHT_PAREN]     = {NULL, NULL, PREC_NONE},
	[TOKEN_COMMA]           = {NULL, NULL, PREC_NONE},
	[TOKEN_SEMICOLON]       = {NULL, NULL, PREC_NONE},
	[TOKEN_NUMBER]          = {number, NULL, PREC_NONE},
	[TOKEN_IDENTIFIER]      = {NULL, NULL, PREC_NONE},
	[TOKEN_STRING]          = {string, NULL, PREC_NONE},
	[TOKEN_SCREEN]          = {NULL, NULL, PREC_NONE},
	[TOKEN_MOVE_CUROR]      = {NULL, NULL, PREC_NONE},
	[TOKEN_DRAW_AT_CURSOR]  = {NULL, NULL, PREC_NONE},
	[TOKEN_RENDER_TEXT]     = {NULL, NULL, PREC_NONE},
	[TOKEN_CLR_SCREEN]      = {NULL, NULL, PREC_NONE},
	[TOKEN_DRAW_LINE]       = {NULL, NULL, PREC_NONE},
	[TOKEN_DRAW_CHARACTER]  = {NULL, NULL, PREC_NONE},
	[TOKEN_ERROR]           = {NULL, NULL, PREC_NONE},
	[TOKEN_EOF]             = {NULL, NULL, PREC_NONE},
};

static void parsePrecedence(Parser* parser, Scanner* scanner, Precedence precedence) {
	advance(parser, scanner);
	ParseFn prefixRule = getRule(parser->previous.type)->prefix;

	if(!prefixRule) {
		error(parser, "Expect expression.");
		return;
	}
	prefixRule(parser, scanner);

	while (precedence <= getRule(parser->current.type)->precedence) {
		advance(parser, scanner);
		ParseFn infixRule = getRule(parser->previous.type)->infix;
		infixRule(parser, scanner);
	}
}

static ParseRule* getRule(TokenType type) {
	return &rules[type];
}

bool compile(char const* source, Chunk* chunk) {
	Scanner scanner = {
		.current = source,
		.start = source,
       	};
	
	Parser parser = {
		.hadError = false,
		.panicMode = false,
       	};
	
	compilingChunk = chunk;

	advance(&parser, &scanner);
	expression(&parser, &scanner);
	consume(&parser, &scanner, TOKEN_EOF, "Expect end of expression");
	endCompiler();
	return !parser.hadError;
}
