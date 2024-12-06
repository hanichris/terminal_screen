#if !defined (chunk_h)
#define chunk_h

#include "common.h"

constexpr uint8_t EOS = 0xff;  // End of file. Marks the end of binary stream.
constexpr uint8_t CLR = 0x7;   // Clear Screen.
constexpr uint8_t DAC = 0x6;   // Draw At Cursor. Draws a character at the cursor location.
constexpr uint8_t CM = 0x5;    // Cursor Movement. Move the cursor to specified location.
constexpr uint8_t RT = 0x4;    // Render Text. Renders a string starting from a specified location.
constexpr uint8_t DL = 0x3;    // Draw Line. Draws a line from one coordinate of the screen to another.
constexpr uint8_t DC = 0x2;    // Draw Character. Places a character at a given coordinate.
constexpr uint8_t SS = 0x1;    // Screen Setup. Defines the dimensions and colour settings of the screen.

/**
 * struct chunk - dynamic array type. Defines a new dynamic array type
 * that will hold the array of bytes received.
 * @count: the number of slots in the array that are currently occupied.
 * @capacity: the capacity of the array.
 * @code: pointer to the array.
 */
typedef struct chunk_t {
	unsigned count;
	unsigned capacity;
	uint8_t* code;
} Chunk;

/**
 * initChunk - Initialises a chunk type to ensure it is in a valid
 * state.
 * @chunk: Pointer to a 'Chunk' type.
 * @Returns: void.
 **/
void initChunk(Chunk* chunk);
/**
 * freeChunk - frees up the memory used to allocate the array to hold
 * the stream of bytes.
 * @chunk: pointer to a 'Chunk' type.
 * @Returns: void.
 */
void freeChunk(Chunk* chunk);
/**
 * writeChunk - Appends a byte to the end of a chunk. If the array to
 * store the bytes has capacity for a new byte, the byte is appended
 * to the end of the array. If not, the internal array's capacity is increased
 * before a new byte can be appended to it.
 * @chunk: pointer to a 'Chunk' type.
 * @byte: the byte that is to be appended to the array.
 * @Returns: void.
 */
void writeChunk(Chunk* chunk, uint8_t byte);


#endif
