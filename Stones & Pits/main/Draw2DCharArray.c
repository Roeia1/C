/*
 * Draw2DCharArray.c
 *
 *  Created on: Jul 29, 2014
 *      Author: roeia1
 */
#include <stdio.h>
#include "Draw2DCharArray.h"

/**
 * Initialize the given char array to be filled with space (' ') characters.
 */
void initializeArray(char arr[ROWS][COLS])
{
	int currRow;
	for (currRow = 0; currRow < ROWS; currRow++)
	{
		int currCol;
		for (currCol = 0; currCol < COLS; currCol++)
		{
			arr[currRow][currCol] = ' ';
		}
	}
}

/**
 * Draw the char array on the screen.
 * After each row go down a line ('\n').
 * Stop drawing when a nul character ('\0') is met.
 */
void drawArray(char arr[ROWS][COLS])
{
	char flag = 0;
	int currRow;
	for (currRow = 0; currRow < ROWS; currRow++)
	{
		int currCol;
		for (currCol = 0; currCol < COLS; currCol++)
		{
			if (arr[currRow][currCol] == '\0')
			{
				flag = 1;
				break;
			}
			printf("%c", arr[currRow][currCol]);
		}
		if (flag == 1)
		{
			break;
		}
		printf("\n");
	}
}

