#include "util.h"

void* xmalloc(size_t size)
{
	void* ptr = malloc(size);
	if (ptr == NULL) {
		exit(EXIT_FAILURE);
	}

	return ptr;
}
