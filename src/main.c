#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

//#include "include/debug.h"
#include "include/vm.h"

static void deserializeChunk(void* buffer, Chunk* chunk) {
	size_t capacitySize = sizeof(chunk->capacity);
	size_t countSize = sizeof(chunk->count);

	size_t offset = 0;
	memcpy(&chunk->capacity, buffer, capacitySize);
	offset += capacitySize;
	memcpy(&chunk->count, buffer + offset, countSize);
	offset += countSize;

	size_t arSize = (chunk->count * sizeof(uint8_t));
	chunk->code = malloc(arSize);
	if (!chunk->code) {
		fprintf(stderr, "Failed to allocate block of memory of size %zu.\n", arSize);
		chunk->count = 0;
		chunk->capacity = 0;
		return;
	}

	memcpy(chunk->code, buffer + offset, chunk->count);	
}

int main(void) {
	//disassembleChunk(&chunk, "test chunk");
	
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
	VM vm = {};
	initVM(&vm);
	for (;;) {
		size_t bufSize = 0;
		int byteCount = read(fd, &bufSize, sizeof(bufSize));
		if (byteCount < 0) {
			fprintf(stderr, "Failed to read the expected buffer size.\n");
			break;
		} else if (byteCount == 0) {
			printf("EOS.\n");
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
			interpret(&vm, &chunk);
		}

		free(buffer);
		freeChunk(&chunk);
	}
	freeVM(&vm);
	if (close(fd) == -1) {
		err(EX_IOERR, "Failed to close '%s'", filePath);
	}
	exit(EXIT_SUCCESS);
}
