#ifndef __BLOOM_H__
#define __BLOOM_H__

#include <stdlib.h>

typedef unsigned int (*h)(const char *, const unsigned int size);

typedef struct {

	size_t *filter;
	size_t size;
	size_t num_funcs;
	h *funcs;

} BLOOM;

int bloom_create(BLOOM *bloom, size_t size, size_t num_funcs, ...);
int bloom_insert(BLOOM *bloom, const char *s);
int bloom_check(BLOOM *bloom, const char *s);
int bloom_delete(BLOOM *bloom, const char *s);
int bloom_destroy(BLOOM *bloom);

#endif