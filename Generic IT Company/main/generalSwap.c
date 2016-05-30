/**
 * A swap functions that works with everything
 * Need to get also the size of the elements that are being swapped
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef void* Element;

void generalSwap(Element a, Element b, int sizeOfElement);

int main()
{
	double a = 4., b = 5.;
	int c = 44, d = 55;
	generalSwap(&a, &b, sizeof(double));
	generalSwap(&c, &d, sizeof(int));
	printf("a=%f b=%f\n", a, b); /* should print a=5 b=4 */
	printf("a=%d b=%d\n", c, d); /* should print a=55 b=44 */
	return 0;
}

/**
 * The swap function
 * Swap the two given elements of the given size
 */
void generalSwap(Element a, Element b, int sizeOfElements)
{
	Element temp = malloc(sizeOfElements);
	if (temp == NULL)
	{
		return;
	}
	else
	{
		memcpy(temp, a, sizeOfElements);
		memcpy(a, b, sizeOfElements);
		memcpy(b, temp, sizeOfElements);
		free(temp);
		temp = NULL;
	}
}
