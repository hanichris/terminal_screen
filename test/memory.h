#if !defined (memory_h)
#define memory_h

#include "common.h"

#define GROW_CAPACITY(capacity) \
	((capacity) < 8 ? 8 : (capacity) * 2)

#define FREEARRAY(type, pointer, oldCount) \
	(type*)reallocate(pointer, sizeof(type) * (oldCount), 0)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
	(type*)reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))
/**
 * reallocate - performs the dynamic memory management. This includes
 * allocating memory, freeing the allocated memory, and changing the size
 * of an existing allocation. The two size parameters control which operation
 * to perform.
 *  oldSize    |   newSize       |   Operation
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  0          |   Non-zero      |   Allocate a new block.
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Non-zero   |   0             |   Free allocation.
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Non-zero   |   < oldSize     |   Shrink existing allocation.
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Non-zero   |   > oldSize     |   Grow existing allocation.
 */
void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif
