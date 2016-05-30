/*
 * RadiusReader.c
 *
 *  Created on: Jul 29, 2014
 *      Author: roeia1
 */
#include <stdio.h>
#include "RadiusReader.h"

/**
 * Read a vector of unsigned ints from a given FILE
 * each line contains a single unsigned integer
 * @return number of lines read
 */
unsigned int readVector(FILE* file, unsigned int vec[MAX_DEPTH])
{
	int currLine = 0;
	while ((readSingleUInt(file, &vec[currLine]) == SUCC_CODE) && (currLine < MAX_DEPTH))
	{
		currLine++;
	}
	return currLine;
}

/**
 * Read a single unsigned int from a given FILE
 * each line contains a single unsigned integer
 * @return SUCC_CODE iff successful, FAIL_CODE otherwise
 */
int readSingleUInt(FILE* file, unsigned int* val)
{
	if (fscanf(file, "%u", val) == 1)
	{
		return SUCC_CODE;
	}
	return FAIL_CODE;
}
