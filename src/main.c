/*
#include "include/chunk.h"
#include "include/debug.h"
#include "include/vm.h"
*/
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#include "include/chunk.h"

static void deserializeChunk(void* buffer, Chunk* chunk) {
	size_t capacitySize = sizeof(chunk->capacity);
	size_t countSize = sizeof(chunk->count);
	chunk->code = malloc(countSize);
	if (!chunk->code) {
		fprintf(stderr, "Failed to allocate memory for code chunk of size %zu.\n", countSize);
		exit(EXIT_FAILURE);
	}

	size_t offset = 0;
	memcpy(&chunk->capacity, buffer, capacitySize);
	offset += capacitySize;
	memcpy(&chunk->count, buffer + offset, countSize);
	offset += countSize;
	memcpy(chunk->code, buffer + offset, chunk->count);
}

int main(void) {
	/*VM vm = {};
	Chunk chunk = {};
	//disassembleChunk(&chunk, "test chunk");
	interpret(&vm, &chunk);
	
	freeVM(&vm);
	freeChunk(&chunk);
	*/
	char const* filePath = "/tmp/myfifo";
	if (access(filePath, F_OK) == -1) {
		if (mkfifo(filePath, 0666) == -1){
			err(EX_OSERR, "Failed to create a named pipe at '%s'", filePath);
		}
	}

	int fd = open(filePath, O_RDONLY);
	if (fd == -1) {
		err(EX_IOERR, "Failed to open '%s' in read mode", filePath);
	}

	Chunk chunk = { };
	for (;;) {
		size_t bufSize = 0;
		int byteCount = read(fd, &bufSize, sizeof(bufSize));
		if (byteCount < 0) {
			fprintf(stderr, "Failed to read the expected buffer size.\n");
			break;
		} else if (byteCount == 0) {
			printf("EOF.\n");
			break;
		}

		void* buffer = malloc(bufSize);
		if (!buffer) {
			fprintf(stderr, "Failed to allocate memory for buffer of size %zu.\n", bufSize);
			break;
		}

		byteCount = read(fd, buffer, bufSize);
		if (byteCount < 0)
			fprintf(stderr, "Failed to read in the buffer from the named pipe.\n");
		else if (byteCount == 0)
			printf("EOF.\n");
		else {
			deserializeChunk(buffer, &chunk);
			
			printf("Chunk capacity: %u\n", chunk.capacity);
			printf("Chunk filled slots: %u\n", chunk.count);
			for (unsigned i = 0; i < chunk.count; i++){
				printf("[%u]: %u\n", i, chunk.code[i]);
			}
		}

		free(buffer);
		freeChunk(&chunk);
	}
	if (close(fd) == -1) {
		err(EX_IOERR, "Failed to close '%s'", filePath);
	}
	exit(EXIT_SUCCESS);
}
