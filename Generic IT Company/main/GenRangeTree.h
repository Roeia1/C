/**
 * A general binary search tree where the nodes are sorted according to their keys
 * Each node have the functions for comparing, copy, printing and free
 */
#ifndef GEN_RANGE_TREE_H
#define GEN_RANGE_TREE_H

/** Pointer to an element in the range tree */
typedef void* Element;
typedef const void* ConstElement;

/** The compare function of the range tree */
typedef int (*lmCmpFunc)(ConstElement, ConstElement);

/** The copy function of the range tree */
typedef Element (*lmCpyFunc)(ConstElement);

/** The print function of the range tree */
typedef char* (*lmLblFunc)(ConstElement);

/** The free function of the range tree */
typedef void (*lmFreFunc)(Element);

typedef enum
{
	FALSE,
	TRUE
} Boolean;

/** Pointer at a range tree */
typedef struct GenRangeTreeRec *RangeTreeP;
typedef const struct GenRangeTreeRec *ConstRangeTreeP; 

/**
 * create a new range tree, Returns a pointer to it. 
 * The nodes of the tree will contain the participateWorkers workers from the array.
 * In addition, receive 4 pointers to functions:
 * - cmp - compare between two elements, return negative number if the first is smaller than the second, zero
 *   if the items are equal or positive number if the first element is larger than the second element.
 * - cpy - duplicate an element. Return NULL in case of memory out.
 * - lbl - turn an element into a string (so we can print it). Allocate memory for the string - it's our
 *   responsibility to free the memory after using the string. In case of out-of-memory event, return NULL.
 * - fre - a function that free the memory allocate for the element.
 * Note that the tree is a static tree - once the tree was created, we can't add / remove elements
 * from it.	
 * Same error handling as in the SimpleRangeTree.c file.
*/
RangeTreeP createNewRangeTree(Element participateWorkers[], int arrsize,
							  int cmp(ConstElement, ConstElement), 
							  Element cpy(ConstElement), 
							  char *lbl(ConstElement), 
							  void fre(Element));

/**
 * Free the range tree from the memory (should be called when the user doesn't need the range tree anymore).
 */
void destroyRangeTree(RangeTreeP tree);

/**
 * Return the number of workers in the range tree.
 */
int size(ConstRangeTreeP tree);

/**
 * Print the tree according to a range quory - print all the workers that
 * are paid at least as p1, and at most as p2.
 */
void printRange(ConstRangeTreeP tree, ConstElement p1, ConstElement p2);

/**
 * Debbuging function - you don't have to use it but you may find it helpful.
 */
void debugStableCheck(ConstRangeTreeP tree);

#endif
