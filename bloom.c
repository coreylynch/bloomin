#include <stdarg.h>
#include <stdio.h>
#include "bloom.h"

int bloom_create(BLOOM *bloom, size_t size, size_t num_funcs, ...)
{
	
	if ((bloom->funcs = (h *)malloc(num_funcs * sizeof(h))) == NULL)
		return -1;

	if ((bloom->filter = (size_t *)calloc(size, sizeof(size_t))) == NULL)
		return -1;

	va_list l;
	int i;
	va_start(l, num_funcs);
	for (i = 0; i < num_funcs; ++i)
	{
		bloom->funcs[i] = va_arg(l, h);	
	}
	va_end(l);

	bloom->num_funcs = num_funcs;
	bloom->size = size;
	
	return 0;
}

int bloom_insert(BLOOM *bloom, const char *s)
{
	size_t i;
	size_t idx;

	for (i = 0; i < bloom->num_funcs; ++i)
	{

		idx = bloom->funcs[i](s, bloom->size);
		bloom->filter[idx]++;
	}
	return 0;
}

int bloom_check(BLOOM *bloom, const char *s)
{
	
	size_t i;
	size_t idx;

	
	for (i = 0; i < bloom->num_funcs; ++i)
	{
		idx = bloom->funcs[i](s, bloom->size);
		if (bloom->filter[idx] == 0)
		{
			return 0;
		}
	}
	return 1;
}

int bloom_delete(BLOOM *bloom, const char *s)
{
	size_t i;
	size_t idx;
	for (i = 0; i < bloom->num_funcs; ++i)
	{
		idx = bloom->funcs[i](s, bloom->size);
		bloom->filter[idx]--;
		if (bloom->filter[idx] <= 0)
		{
			bloom->filter[idx] = 0;
		}
	}
	return 0;
}

int bloom_destroy(BLOOM *bloom)
{
	free(bloom->filter);
	free(bloom->funcs);
	free((void *)bloom->size);
	free((void *)bloom->num_funcs);
	free((void *)bloom);

	return 0;

}






