#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "bloom.h"

unsigned int sax_hash(const char *key, unsigned int size)
{
	unsigned int h=0;

	while(*key) h^=(h<<5)+(h>>2)+(unsigned char)*key++;
	return h % size;
}

unsigned int sdbm_hash(const char *key, unsigned int size)
{
	unsigned int h=0;
	while(*key) h=(unsigned char)*key++ + (h<<6) + (h<<16) - h;
	return h % size;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	char line[1024];
	char *p;

	size_t size = 2500000;

	BLOOM *bloom;

	if(!(bloom = malloc(sizeof(BLOOM)))) 
		return -1;

	if((bloom_create(bloom, size, 2, sdbm_hash, sax_hash)) != 0)
	{
		fprintf(stderr, "ERROR: Could not create bloom filter\n");
		return EXIT_FAILURE;
	}

	
	char * insert_words[5];
	insert_words[0] = "first";
  	insert_words[1] = "first";
  	insert_words[2] = "third";
  	insert_words[3] = "fourth";
  	insert_words[4] = "fifth";

  	int i;
  	for (i = 0; i < 5; ++i)
  	{
  		bloom_insert(bloom, (char *) insert_words[i]);
  	}
	
  	char * check_words[5];
	check_words[0] = "first";
  	check_words[1] = "second";
  	check_words[2] = "not";
  	check_words[3] = "in";
  	check_words[4] = "list";

  	for (i = 0; i < 5; ++i)
  	{
  		if(bloom_check(bloom, (char *) check_words[i]) != 1) {
			fprintf(stderr, "No match for word \"%s\"\n", p);
		}
  	}

	bloom_destroy(bloom);

	return EXIT_SUCCESS;
}