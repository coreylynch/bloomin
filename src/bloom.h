#ifndef __BLOOM_H__
#define __BLOOM_H__

#include <stdint.h>
#include <stdlib.h>

typedef struct {

	uint32_t *filter;
	uint32_t size;
	uint32_t num_funcs;
	uint32_t *hashes;

} BLOOM;

BLOOM *bloom_create(uint32_t size, double error_rate);
int bloom_insert(BLOOM *bloom, const char *s, uint32_t len);
int bloom_check(BLOOM *bloom, const char *s, uint32_t len);
int bloom_delete(BLOOM *bloom, const char *s, uint32_t len);
int bloom_destroy(BLOOM *bloom);

#endif