#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "murmur.h"
#include "bloom.h"

#define ERROR_RATE .05
#define SIZE 100000

int main()
{

	BLOOM *bloom;

	if(!(bloom = bloom_create(SIZE, ERROR_RATE)))
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
  		bloom_insert(bloom, (char *) insert_words[i], strlen(insert_words[i]));
  	}
	
  	char * check_words[5];
	check_words[0] = "first";
  	check_words[1] = "second";
  	check_words[2] = "not";
  	check_words[3] = "in";
  	check_words[4] = "list";

  	for (i = 0; i < 5; ++i)
  	{
  		if(bloom_check(bloom, (char *) check_words[i], strlen(check_words[i])) != 1) 
  		{
  			fprintf(stderr, "%i\n", i);
			fprintf(stderr, "No match for word \"%s\"\n", check_words[i], strlen(check_words[i]));
		}
  	}

  	bloom_destroy(bloom);

	return EXIT_SUCCESS;
}