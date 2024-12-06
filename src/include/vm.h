#if !defined(vm_h)
#define vm_h

#include "chunk.h"

/**
 * struct vm_t - virtual machine type. Defines a VM type that will
 * interpret a stream of bytes.
 * @chunk: pointer to a 'Chunk' type that the VM needs to execute.
 * @ip: instruction pointer. Keeps track of the location of the
 * instruction currently being executed.
 */
typedef struct vm_t {
	Chunk* chunk;
	uint8_t* ip;
} VM;

/**
 * enum - Defines named constants that represent the possible outcomes
 * of the VM during its execution.
 */
typedef enum {
	INTERPRET_OK,
	INTERPRET_ERROR,
} InterpretResult;

/**
 * initVM - initialises a 'VM' type to ensure it is in a valid state.
 * @vm: pointer to a 'VM' type.
 * @Returns: void.
 */
void initVM(VM* vm);
void freeVM(VM* vm);

/**
 * interpret - executes the instructions that are encoded in the bytestream
 * using the provided vm object.
 * @chunk: pointer to a 'Chunk' type.
 * @vm: pointer to a 'VM' type.
 * @Returns: InterpretResult.
 */
InterpretResult interpret(VM* vm, Chunk* chunk);

#endif
