#include "filereader.h"

#include <stdio.h>
#include <stdlib.h>

char* read_text_file(const char* path)
{
	FILE* f = fopen(path, "rb");
	if (f == NULL) {
		exit(EXIT_FAILURE);
	}

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	rewind(f);

	char* buf = malloc(size + 1);
	if (buf == NULL) {
		exit(EXIT_FAILURE);
	}

	if (fread(buf, sizeof(*buf), size, f) == 0) {
		exit(EXIT_FAILURE);
	}
	buf[size] = 0;

	return buf;
}
