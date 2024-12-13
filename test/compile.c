#include "compiler.h"

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

static bool check(Parser* parser, TokenType type) {
	return parser->current.type == type;
}

static bool match(Parser* parser, Scanner* scanner, TokenType type) {
	if (!check(parser, type)) return false;
	advance(parser, scanner);
	return true;
}

static void emitByte(uint8_t byte) {
	writeChunk(currentChunk(), byte);
}

static void endCompiler() {
	emitByte(END);
}

static void expression(Parser* parser, Scanner* scanner);
static void statement(Parser* parser, Scanner* scanner);
static void declaration(Parser* parser, Scanner* scanner);
static ParseRule* getRule(TokenType type);
static void parsePrecedence(Parser* parser, Scanner* scanner, Precedence precedence);

static uint8_t argumentList(Parser* parser, Scanner* scanner) {
	uint8_t byteCount = 0;
	Scanner tempScanner = {
		.current = scanner->current,
		.start = scanner->start,
	};
	Parser tempParser = {
		.current = parser->current,
		.previous = parser->previous,
		.hadError = false,
		.panicMode = false,
	};

	if (!check(&tempParser, TOKEN_RIGHT_PAREN)) {
		do {
			if (check(&tempParser, TOKEN_STRING)){
				unsigned length = tempParser.current.length - 2;
				if (length > UINT8_MAX - 3) {
					errorAtCurrent(&tempParser, "String length exceeds 252 characters.");
				} else {
					byteCount += (uint8_t)length;
				}
			} else
				byteCount++;
			advance(&tempParser, &tempScanner);
		} while (match(&tempParser, &tempScanner, TOKEN_COMMA));
	}
	return byteCount;

}

static void call(Parser* parser, Scanner* scanner) {
	uint8_t argCount = argumentList(parser, scanner);
	emitByte(argCount);

	if (!check(parser, TOKEN_RIGHT_PAREN)) {
		do {
			expression(parser, scanner);
		} while (match(parser, scanner, TOKEN_COMMA));
	}
	consume(parser, scanner, TOKEN_RIGHT_PAREN, "Expect ')' after arguments");
}

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

static void screenStmt(Parser* parser, Scanner* scanner) {
	emitByte(SS);
	consume(parser, scanner, TOKEN_LEFT_PAREN, "Expect '(' after 'screen'.");
	call(parser, scanner);
	consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after expression");
}

static void moveStmt(Parser* parser, Scanner* scanner) {
	emitByte(CM);
	consume(parser, scanner, TOKEN_LEFT_PAREN, "Expect '(' after 'move'.");
	call(parser, scanner);
	consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after expression");
}
static void drawStmt(Parser* parser, Scanner* scanner) {
	emitByte(DC);
	consume(parser, scanner, TOKEN_LEFT_PAREN, "Expect '(' after 'draw'.");
	call(parser, scanner);
	consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after expression");
}
static void drawAtCursorStmt(Parser* parser, Scanner* scanner) {
	emitByte(DAC);
	consume(parser, scanner, TOKEN_LEFT_PAREN, "Expect '(' after 'dcur'.");
	call(parser, scanner);
	consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after expression");
}
static void lineStmt(Parser* parser, Scanner* scanner) {
	emitByte(DL);
	consume(parser, scanner, TOKEN_LEFT_PAREN, "Expect '(' after 'line'.");
	call(parser, scanner);
	consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after expression");
}
static void clearStmt(Parser* parser, Scanner* scanner) {
	emitByte(CLR);
	consume(parser, scanner, TOKEN_LEFT_PAREN, "Expect '(' after 'clear'.");
	call(parser, scanner);
	consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after expression");
}
static void renderStmt(Parser* parser, Scanner* scanner) {
	emitByte(RT);
	consume(parser, scanner, TOKEN_LEFT_PAREN, "Expect '(' after 'render'.");
	call(parser, scanner);
	consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after expression");
}
static void expression(Parser* parser, Scanner* scanner) {
	parsePrecedence(parser, scanner, PREC_BASE);
}

static void expressionStatement(Parser* parser, Scanner* scanner) {
	expression(parser, scanner);
	consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after expression.");
}

static void synchronise(Parser* parser, Scanner* scanner) {
	parser->panicMode = false;

	while (parser->current.type != TOKEN_EOF) {
		if (parser->previous.type == TOKEN_SEMICOLON) return;
		switch (parser->current.type) {
			case TOKEN_CLR_SCREEN:
			case TOKEN_SCREEN:
			case TOKEN_DRAW_AT_CURSOR:
			case TOKEN_DRAW_LINE:
			case TOKEN_RENDER_TEXT:
			case TOKEN_DRAW_CHARACTER:
			case TOKEN_MOVE_CUROR:
				return;
			default:
				;
		}
		advance(parser, scanner);
	}
}

static void declaration(Parser* parser, Scanner* scanner) {
	statement(parser, scanner);

	if (parser->panicMode) synchronise(parser, scanner);
}

static void statement(Parser* parser, Scanner* scanner) {
	if (match(parser, scanner, TOKEN_SCREEN)) {
		screenStmt(parser, scanner);
	} else if (match(parser, scanner, TOKEN_MOVE_CUROR)){
		moveStmt(parser, scanner);
	} else if (match(parser, scanner, TOKEN_DRAW_LINE)){
		lineStmt(parser, scanner);
	} else if (match(parser, scanner, TOKEN_CLR_SCREEN)){
		clearStmt(parser, scanner);
	} else if (match(parser, scanner, TOKEN_DRAW_CHARACTER)){
		drawStmt(parser, scanner);
	} else if (match(parser, scanner, TOKEN_RENDER_TEXT)){
		renderStmt(parser, scanner);
	} else if (match(parser, scanner, TOKEN_DRAW_AT_CURSOR)){
		drawAtCursorStmt(parser, scanner);
	} else {
		expressionStatement(parser, scanner);
	}
	
}

ParseRule rules[] = {
	[TOKEN_LEFT_PAREN]      = {grouping, NULL, PREC_NONE},
	[TOKEN_RIGHT_PAREN]     = {NULL, NULL, PREC_NONE},
	[TOKEN_COMMA]           = {NULL, NULL, PREC_NONE},
	[TOKEN_SEMICOLON]       = {NULL, NULL, PREC_NONE},
	[TOKEN_NUMBER]          = {number, NULL, PREC_NONE},
	[TOKEN_IDENTIFIER]      = {NULL, NULL, PREC_NONE},
	[TOKEN_STRING]          = {string, NULL, PREC_NONE},
	[TOKEN_SCREEN]          = {screenStmt, NULL, PREC_NONE},
	[TOKEN_MOVE_CUROR]      = {moveStmt, NULL, PREC_NONE},
	[TOKEN_DRAW_AT_CURSOR]  = {drawAtCursorStmt, NULL, PREC_NONE},
	[TOKEN_RENDER_TEXT]     = {renderStmt, NULL, PREC_NONE},
	[TOKEN_CLR_SCREEN]      = {clearStmt, NULL, PREC_NONE},
	[TOKEN_DRAW_LINE]       = {lineStmt, NULL, PREC_NONE},
	[TOKEN_DRAW_CHARACTER]  = {drawStmt, NULL, PREC_NONE},
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
	while (!match(&parser, &scanner, TOKEN_EOF))
		declaration(&parser, &scanner);
	endCompiler();
	return !parser.hadError;
}
