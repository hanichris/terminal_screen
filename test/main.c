#include "compiler.h"

static void* serializeChunk(Chunk* chunk, size_t* bufSize) {
	size_t capacitySize = sizeof(chunk->capacity);
	size_t countSize = sizeof(chunk->count);
	*bufSize = capacitySize + countSize + (chunk->count * sizeof(uint8_t));
	void* buffer = malloc(*bufSize);
	if (!buffer) {
		fprintf(stderr, "Failed to allocate buffer memory of size %zu.\n", *bufSize);
		exit(EXIT_FAILURE);
	}
	size_t offset = 0;
	memcpy(buffer, &chunk->capacity, capacitySize);
	offset += capacitySize;
	memcpy(buffer + offset, &chunk->count, countSize);
	offset += countSize;
	memcpy(buffer + offset, chunk->code, chunk->count);
	return buffer;
}

static void repl() {
	char const* filePath = "/tmp/myfifo";
	if (access(filePath, F_OK) == -1)
		if (mkfifo(filePath, 0666)) {
			err(EX_OSERR, "Failed to create a named pipe at '%s'", filePath);
		}

	int fd = open(filePath, O_WRONLY);
	if (fd == -1)
		err(EX_IOERR, "Failed to open '%s' in write mode", filePath);

	Chunk chunk = { };
	void* buffer = nullptr;
	char* line = nullptr;
	size_t linecap = 0;
	for (;;) {
		printf(">>> ");
		if (getline(&line, &linecap, stdin) == -1) {
			if (feof(stdin)) {
				break;
			} else {
				perror("readline");
				break;
			}
		}
		if(!compile(line, &chunk)) {
			free(line);
			freeChunk(&chunk);
			line = nullptr;
			linecap = 0;
			continue;
		}
		size_t bufSize = 0;
		buffer = serializeChunk(&chunk, &bufSize);
		int byteCount = write(fd, &bufSize, sizeof(bufSize));
		if (byteCount == -1) {
			fprintf(
				stderr,
				"Error: %s. Failed to write the buffer size to the named pipe.",
				strerror(errno)
			);
			free(line);
			free(buffer);
			freeChunk(&chunk);
			break;
		}

		byteCount = write(fd, buffer, bufSize);
		if (byteCount == -1) {
			fprintf(
				stderr,
				"Error: %s. Failed to write the buffer to the named pipe.",
				strerror(errno)
			);
			free(line);
			free(buffer);
			freeChunk(&chunk);
			break;
		}

		free(line);
		free(buffer);
		freeChunk(&chunk);
		line = nullptr;
		linecap = 0;
	}
	if (close(fd) == -1) {
		err(EX_IOERR, "Failed to close '%s'", filePath);
	}
	if (unlink(filePath) == -1) {
		fprintf(stderr, "Error: %s. Could not unlink file '%s'", strerror(errno), filePath);
	}
	printf("\n");
}


int main(int argc, [[maybe_unused]]char* argv[argc]) {
	switch (argc) {
		case 1:
			repl();
			break;
		default:
			errx(EX_USAGE, "Does not accept command line arguments.");
	}
	exit(EXIT_SUCCESS);
}
