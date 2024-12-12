#include "memory.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
	if (newSize == 0) {
		free(pointer);
		return nullptr;
	}

	void* result = realloc(pointer, newSize);
	if (!result) err(EX_OSERR, "Cannot reallocate memory.");
	return result;
}
