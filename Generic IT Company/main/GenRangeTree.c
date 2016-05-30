/**
 * The implementation of the binary tree for sorting workers according to their salary
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "GenRangeTree.h"

/**********************************************************************************
 A tree node definitions and functions
 **********************************************************************************/

typedef struct Node *NodeP;
typedef const struct Node *ConstNodeP;
typedef enum
{
	LEFT,
	RIGHT
} Side;
typedef enum
{
	OUT_OF_MEMORY,
	NULL_INPUT,
	SET_A_ROOT_WHEN_EXISTS,
	GENERAL_ERROR,
	ELEMENT_ADD_TWICE,
	BAD_RANGE
} ErrorTypes;

static void reporterrorMessage(ErrorTypes theErr, int currLineNumber)
{
	fprintf(stderr, "ERROR in line %d: ", currLineNumber);
	if (theErr == OUT_OF_MEMORY)
	{
		fprintf(stderr, "Out of memory!!!\n");
	}
	else if (theErr == NULL_INPUT)
	{
		fprintf(stderr, "Function received an illegal input (NULL Pointer)!!!\n");
	}
	else if (theErr == SET_A_ROOT_WHEN_EXISTS)
	{
		fprintf(stderr, "The root of the tree isn't empty, but you're trying to set it!!!\n");
	}
	else if (theErr == ELEMENT_ADD_TWICE)
	{
		fprintf(stderr, "The array contain two elements with the same data!!!\n");
	}
	else if (theErr == BAD_RANGE)
	{
		fprintf(stderr, "Bad input range for printRange: p1 is bigger than p2!!!\n");
	}
	else
	{
		fprintf(stderr, "General error.\n");
	}
	exit(1);
}

#define ERROR_MESSAGE(x) reporterrorMessage(x, __LINE__)

/**
 * A node in the tree contains a pointer to the two sons, to the parent an to the key
 */
struct Node
{
	NodeP _left;
	NodeP _right;
	NodeP _parent;
	Element _key;	// Points to data about the worker
};

static NodeP getNewNode(ConstElement key, NodeP left, NodeP right, NodeP parent, lmCpyFunc lmCpy)
{
	assert(lmCpy != NULL);
	NodeP retVal = (NodeP) malloc(sizeof(struct Node));
	if (retVal == NULL)
	{
		ERROR_MESSAGE(OUT_OF_MEMORY);
	}
	if (key == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	retVal->_left = left;
	retVal->_right = right;
	retVal->_parent = parent;
	retVal->_key = lmCpy(key);
	assert(retVal->_key != NULL);
	return retVal;
}

static void freeNode(NodeP node, lmFreFunc lmFre)
{
	assert(lmFre != NULL);
	if (node == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	lmFre(node->_key);
	free(node);
	node = NULL;
}

static NodeP getChildren(ConstNodeP node, Side side)
{
	if (node == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	return (side == LEFT) ? node->_left : node->_right;
}

static NodeP getParent(NodeP node)
{
	if (node == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	return node->_parent;
}

static Element getNodeKey(NodeP node)
{
	return node->_key;
}

static void setChild(NodeP node, Side side, NodeP child)
{
	if (node == NULL || child == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	if (side == LEFT)
	{
		assert(node->_left == NULL);
		node->_left = child;
	}
	else
	{
		assert(side == RIGHT);
		assert(node->_right == NULL);
		node->_right = child;
	}
}

static Side whichChild(ConstNodeP node, ConstNodeP child)
{
	if (node == NULL || child == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	if (node->_right == child)
	{
		return RIGHT;
	}
	assert(node->_left == child);
	return LEFT;
}


/**********************************************************************************
 The range tree definitions and functions
 **********************************************************************************/

/**
 * A struct that contains the tree of Workers.
 * Including the root, the maximal node and the number of leafs in the tree
 */
struct GenRangeTreeRec
{
	/* The tree root, contains NULL for an empty tree */
	NodeP _root;

	/* A pointer to the node with the maximum value in the tree (useful for the successor function).
	   We have to update this field in the Add/Remove element functions. */
	NodeP _maxNode;

	/* Number of nodes in the tree */
	int _size;

	/* Duplicate an element. Return NULL in case of memory out. */
	lmCpyFunc lmCpy;

	/* Compare between two elements, return negative number if the first is smaller than the
	 * second, zero if the items are equal or positive number if the first element is larger than
	 * the second element. */
	lmCmpFunc lmCmp;

	/* turn an element into a string (so we can print it). Allocate memory for the string - it's
	 * our responsibility to free the memory after using the string. In case of out-of-memory event,
	 * return NULL. */
	lmLblFunc lmLbl;

	/* a function that free the memory allocate for the element. */
	lmFreFunc lmFre;
};

static NodeP getRoot(ConstRangeTreeP tree)
{
	if (tree == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	return tree->_root;
}

/* For save setRoot, the root must be NULL in order to set it */
static void setRoot(RangeTreeP tree, NodeP node, Boolean safe)
{
	if (tree == NULL || node == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	if (getRoot(tree) != NULL && safe)
	{
		ERROR_MESSAGE(SET_A_ROOT_WHEN_EXISTS);
	}
	tree->_root = node;
}

/*
	Search for keyToSearchFor in the SubTree. Helper function of subTreeSearch (see below).
*/
static NodeP subTreeSearchRec(NodeP root, ConstElement keyToSearchFor, lmCmpFunc lmCmp)
{
	assert(lmCmp != NULL);
	int cmpRetVal;
	assert(keyToSearchFor != NULL);
	if (root == NULL)
	{
		return NULL;
	}
	cmpRetVal = lmCmp(root->_key, keyToSearchFor);
	if (cmpRetVal == 0)
	{
		return root;
	}
	if (cmpRetVal > 0)
	{
		if (getChildren(root, LEFT) == NULL)
		{
			return root;
		}
		return subTreeSearchRec(getChildren(root, LEFT), keyToSearchFor, lmCmp);
	}
	if (getChildren(root, RIGHT) == NULL)
	{
		return root;
	}
	return subTreeSearchRec(getChildren(root, RIGHT), keyToSearchFor, lmCmp);
}

/* Search for keyToSearchFor in the range tree. Will return NULL for an empty range tree,
   a pointer to the node if the node exists in the tree or a pointer to the last
   node in the search path otherwise.											*/
static NodeP subTreeSearch(ConstRangeTreeP tree, ConstElement keyToSearchFor)
{
	if (tree == NULL || keyToSearchFor == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	return subTreeSearchRec(tree->_root, keyToSearchFor, tree->lmCmp);
}

static void addElement(RangeTreeP tree, ConstElement keyToSearchFor)
{
	int direct;
	NodeP parent;
	debugStableCheck(tree);
	if (tree == NULL || keyToSearchFor == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	parent = subTreeSearch(tree, keyToSearchFor);
	if (parent == NULL)
	{
		/* An empty tree - the new node will be the root (special case) */
		NodeP newRoot = getNewNode(keyToSearchFor, NULL, NULL, NULL, tree->lmCpy);
		assert(tree->_size == 0);
		assert(newRoot != NULL);
		setRoot(tree, newRoot, TRUE);
		tree->_maxNode = newRoot;
		++tree->_size;
		return;
	}
	direct = tree->lmCmp(getNodeKey(parent), keyToSearchFor);
	if (direct == 0)
	{
		/* The element is already in the tree */
		ERROR_MESSAGE(ELEMENT_ADD_TWICE);
	}
	++tree->_size;
	if (direct > 0)
	{
		NodeP newNode = getNewNode(keyToSearchFor, NULL, NULL, parent, tree->lmCpy);
		assert(newNode != NULL);
		setChild(parent, LEFT, newNode);
	}
	else
	{
		NodeP newNode = getNewNode(keyToSearchFor, NULL, NULL, parent, tree->lmCpy);
		assert(newNode != NULL);
		if (tree->lmCmp(getNodeKey(tree->_maxNode), getNodeKey(newNode)) < 0)
		{
			tree->_maxNode = newNode;
		}
		setChild(parent, RIGHT, newNode);
	}
}

/*
 * Initializes the random number seed.
 *
 * The seed is initialized from the environment variable SRAND_SEED, or,
 * if SRAND_SEED is undefined, uses the system time as the seed.
 */
static void initializeSeed()
{
	char *seedStr = getenv("SRAND_SEED");
	unsigned int seed;

	if (seedStr != NULL)
	{
		/* read seed from the environment variable and convert to an integer */
		seed = atoi(seedStr);
	}
	else
	{
		/* use the system time as a seed. it changes every second and never repeats. */
		seed = time(NULL);
	}

	srand(seed);
}

/*
 * Returns a random integer from the range [low,high].
 */
static int chooseRandomNumber(int low, int high)
{
	/* In Numerical Recipes in C: The Art of Scientific Computing
	   (William H. Press, Brian P. Flannery, Saul A. Teukolsky, William T. Vetterling; New  York:  Cambridge
	   University Press, 1992 (2nd ed., p. 277)), the following comments are made:
			  "If you want to generate a random integer between 1 and 10, you should always do it
			   by using high-order bits, as in

					 j = 1 + (int) (10.0 * (rand() / (RAND_MAX + 1.0)));

	   (cited by rand(3) man page) */
	int num = low + (int) ( ((double)(high - low + 1)) * (rand() / (RAND_MAX + 1.0)));

	return num;
}


/* Get the inserted order entered by the user and "mix" the array to create a "random" insertion order.
   There exists better algorithm for randomness, but the following algorithm is good enough
   for our purpose. */
static void generateRandomPermutation(Element participateElements[], int arrsize)
{
	int it;
	if (arrsize < 2)
	{
		return;
	}
	initializeSeed();
	for (it = 0 ; it < arrsize ; ++it)
	{
		Element tempElement;
		int f1 = chooseRandomNumber(0, arrsize-1);
		int f2 = chooseRandomNumber(0, arrsize-1);
		if (f1 == f2)
		{
			continue;
		}
		tempElement = participateElements[f1];
		participateElements[f1] = participateElements[f2];
		participateElements[f2] = tempElement;
	}
}

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
RangeTreeP createNewRangeTree(Element lmArray[], int arrSize, lmCmpFunc cmp,
							  lmCpyFunc cpy, lmLblFunc lbl, lmFreFunc fre)
{
	if (cmp == NULL || cpy == NULL || lbl == NULL || fre == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	int it;
	RangeTreeP retVal = (RangeTreeP) malloc(sizeof(struct GenRangeTreeRec));
	if (retVal == NULL)
	{
		ERROR_MESSAGE(OUT_OF_MEMORY);
	}
	generateRandomPermutation(lmArray, arrSize);
	retVal->_root = NULL;
	retVal->_maxNode = NULL;
	retVal->_size = 0;
	retVal->lmCpy = cpy;
	retVal->lmCmp = cmp;
	retVal->lmLbl = lbl;
	retVal->lmFre = fre;
	for (it = 0 ; it < arrSize ; ++it)
	{
		addElement(retVal, lmArray[it]);
	}
	return retVal;
}

static void freeNodeRec(NodeP node, lmFreFunc lmFre)
{
	if (node == NULL)
	{
		return;
	}
	freeNodeRec(getChildren(node, RIGHT), lmFre);
	freeNodeRec(getChildren(node, LEFT), lmFre);
	freeNode(node, lmFre);
}

/**
 * Call this function if you want to clear all the elements in the node.
*/
static void clearTree(RangeTreeP tree)
{
	if (tree == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	freeNodeRec(getRoot(tree), tree->lmFre);
	tree->_size = 0;
	tree->_maxNode = NULL;
}

/**
 * Free the range tree from the memory (should be called when the user doesn't need the range tree anymore).
 */
void destroyRangeTree(RangeTreeP tree)
{
	if (tree == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	clearTree(tree);
	free(tree);
	tree = NULL;
}

/**
 * Return the number of workers in the range tree.
 */
int size(ConstRangeTreeP tree)
{
	if (tree == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	return tree->_size;
}

static NodeP getMinimum(NodeP n)
{
	while(1)
	{
		NodeP tempN;
		assert(n != NULL);
		tempN = getChildren(n, LEFT);
		if (tempN == NULL)
		{
			return n;
		}
		n = tempN;
	}
	return NULL;
}

/* Return the successor of the node 'n' in the range tree, or NULL if 'n' is already the maximum */
static NodeP successor(NodeP n, NodeP maximumNode)
{
	NodeP tempN;
	assert(n != NULL);

	/* Check if 'n' is the maximum */
	if (n == maximumNode)
	{
		return NULL;
	}

	/* if 'n' has a right child go visit its minimum */
	tempN = getChildren(n, RIGHT);
	if (tempN != NULL)
	{
		return getMinimum(tempN);
	}

	/* Get 'n' node first father such that 'n' it's his left son */
	while (1)
	{
		NodeP oldN = n;
		n = getParent(n);
		assert(n != NULL);
		if (LEFT == whichChild(n, oldN))
		{
			break;
		}
	}

	return n;
}

/* Search the tree, find the node that contains the worker with the smallest
   paycheck that is bigger than p1 paycheck */
static NodeP findMinAboveElement(ConstRangeTreeP tree, ConstElement p)
{
	NodeP retVal = NULL;
	NodeP curr;
	assert(tree != NULL);
	assert(p != NULL);
	curr = getRoot(tree);
	while (curr != NULL)
	{
		ConstElement currElement = getNodeKey(curr);
		if (tree->lmCmp(currElement, p) >= 0)
		{
			if (retVal == NULL)
			{
				retVal = curr;
			}
			if (tree->lmCmp(getNodeKey(curr), getNodeKey(retVal)) < 0)
			{
				retVal = curr;
			}
			curr = getChildren(curr, LEFT);
		}
		else
		{
			curr = getChildren(curr, RIGHT);
		}
	}
	return retVal;
}

/**
 * Print the tree according to a range quory - print all the workers that
 * are paid at least as p1, and at most as p2.
 */
void printRange(ConstRangeTreeP tree, ConstElement p1, ConstElement p2)
{
	NodeP opt;
	if (tree == NULL || p1 == NULL || p2 == NULL)
	{
		ERROR_MESSAGE(NULL_INPUT);
	}
	if (tree->lmCmp(p1, p2) > 0)
	{
		ERROR_MESSAGE(BAD_RANGE);
	}
	opt = findMinAboveElement(tree, p1);
	if (opt == NULL)
	{
		return;
	}
	while (tree->lmCmp(getNodeKey(opt), p2) <= 0)
	{
		char* elementString = tree->lmLbl(getNodeKey(opt));
		printf("%s\n", elementString);
		free(elementString);
		elementString = NULL;
		opt = successor(opt, tree->_maxNode);
		if (opt == NULL)
		{
			return;
		}
	}
}

/**
 * Used for debbugin
 * Verify that the Node is leagal (as a node in a binary search tree)
 * Then verify all it successors recursively
 */
static void debugCheckNode(NodeP n, int (*lmCmp)(ConstElement, ConstElement))
{
	if (n == NULL)
	{
		return;
	}
	if (n->_left != NULL)
	{
		assert(n->_left->_parent == n);
		assert(lmCmp(getNodeKey(n->_left), getNodeKey(n)) < 0);
		debugCheckNode(n->_left, lmCmp);
	}
	if (n->_right != NULL)
	{
		assert(n->_right->_parent == n);
		assert(lmCmp(getNodeKey(n->_right), getNodeKey(n)) > 0);
		debugCheckNode(n->_right, lmCmp);
	}
}

/**
 * Debbuging function - you don't have to use it but you may find it helpful.
 */
void debugStableCheck(ConstRangeTreeP tree)
{
	assert(tree != NULL);
	debugCheckNode(tree->_root, tree->lmCmp);
	assert(tree->_root == NULL || (tree->_maxNode != NULL && tree->_maxNode->_right == NULL));
}

#ifndef NDEBUG
#define NUM_PAR 10

/**
 * If the condition false, prints an error
 */
static void checkForError(Boolean condition, char *message)
{
	if(condition)
	{
		return;
	}
	fprintf(stderr, "ERROR: %s\n", message);
	exit(1);
}

/**
 * Creating a new integer with a given value
 */
int* getInt(int value)
{
	int* retVal = (int*) malloc(sizeof(int));
	if (retVal == NULL)
	{
		ERROR_MESSAGE(OUT_OF_MEMORY);
	}
	*retVal = value;
	return retVal;
}

/**
 * Compare Manager Salaries
 */
static int intCmp(ConstElement c1, ConstElement c2)
{
	return *(int*)c1 - *(int*)c2;
}

/**
 * Copy Integer function
 */
static Element cpyInt(ConstElement c)
{
	int* temp = (int*)malloc(sizeof(int));
	if (temp == NULL)
	{
		ERROR_MESSAGE(OUT_OF_MEMORY);
	}
	*temp = *(int*)c;
	return ((Element) temp);
}

/**
 * lbl Manager function
 */
static char* lblInt(ConstElement c)
{
	char* numToStr = (char*)malloc(sizeof(char)*10);
	if (numToStr == NULL)
	{
		ERROR_MESSAGE(OUT_OF_MEMORY);
	}
	sprintf(numToStr, "%d", *(int*)c);
	return numToStr;
}

/**
 * Free Manager function
*/
static void freInt(Element c)
{
	free((int*)c);
	c = NULL;
}

/**
 * Create a tree and check if its creation went well. Return the tree.
 */
static RangeTreeP createTreeCheck(int treeSize, Element lmArray[treeSize], lmCmpFunc cmp,
								  lmCpyFunc cpy, lmLblFunc lbl, lmFreFunc fre, Element maxKey,
								  Element minKey)
{
	// Check if empty tree being created correctly
	Element emptyArray[0];
	RangeTreeP emptyTree = createNewRangeTree(emptyArray, 0, *cmp, *cpy, *lbl, *fre);
	checkForError(emptyTree != NULL, "Tree can't be NULL after being created");
	checkForError(emptyTree->_root == NULL, "Root must be NULL in an empty tree");
	checkForError(emptyTree->_maxNode == NULL, "Tree max element must be NULL in an empty tree");
	checkForError(emptyTree->_size == 0, "Tree size must be 0 in an empty tree");
	checkForError(emptyTree->lmCmp != NULL, "Compare function must not be NULL after empty tree "
				  "creation");
	checkForError(emptyTree->lmCpy != NULL, "Copy function must not be NULL after empty tree "
				  "creation");
	checkForError(emptyTree->lmFre != NULL, "Free function must not be NULL after empty tree "
				  "creation");
	checkForError(emptyTree->lmLbl != NULL, "Lbl function must not be NULL after empty tree "
				  "creation");
	destroyRangeTree(emptyTree);

	// Check if not an empty tree being created correctly
	RangeTreeP tree = createNewRangeTree(lmArray, treeSize, *cmp, *cpy, *lbl, *fre);
	checkForError(emptyTree != NULL, "Tree can't be NULL after being created");
	checkForError(tree->_root != NULL, "Tree root must not be NULL in a tree that ain't empty");
	checkForError(tree->lmCmp(tree->_maxNode->_key, maxKey) == 0,
			 "Tree max element is incorrect after creation");
	ConstElement minTreeKey = getMinimum(tree->_root)->_key;
	checkForError(tree->lmCmp(minTreeKey, minKey) == 0, "Tree minimum element is incorrect after "
			"creation");
	checkForError(tree->_size == treeSize, "Tree size is incorrect after creation");
	return tree;
}

/**
 * Add an element to the tree and check if it was added correctly.
 */
static void addElementCheck(RangeTreeP tree, int treeSize, Element keyToAdd)
{
	addElement(tree, keyToAdd);
	treeSize++;
	checkForError(tree->_size == treeSize, "Tree size is incorrect after addition");
	checkForError(tree->lmCmp(tree->_maxNode->_key, keyToAdd) >= 0,
			 "Tree max element is incorrect after addition");
	NodeP nodeFound = subTreeSearch(tree, keyToAdd);
	checkForError(tree->lmCmp(nodeFound->_key, keyToAdd) == 0,
			 "The element isn't found in the tree after addition");
}

/**
 * The main method for the debug process
 */
int main()
{
	RangeTreeP tree;
	Element tempArr[NUM_PAR];
	int* numbers[NUM_PAR];

	// Initializing the numbers array
	numbers[0] = getInt(5);
	numbers[1] = getInt(2);
	numbers[2] = getInt(3);
	numbers[3] = getInt(4);
	numbers[4] = getInt(1);
	numbers[5] = getInt(6);
	numbers[6] = getInt(7);
	numbers[7] = getInt(10);
	numbers[8] = getInt(8);
	numbers[9] = getInt(9);
	int* maxKey = numbers[7];
	int* minKey = numbers[4];

	int currNum;
	for (currNum = 0 ; currNum < NUM_PAR ; ++currNum)
	{
		tempArr[currNum] = (Element) numbers[currNum];
	}

	// Checking the creation of the tree
	tree = createTreeCheck(NUM_PAR, tempArr, &intCmp, &cpyInt, &lblInt,
						 &freInt, maxKey, minKey);

	// Checking the element adding
	int* newInt = getInt(11);
	addElementCheck(tree, NUM_PAR, newInt);

	// Free all the variables
	for (currNum = 0 ; currNum < NUM_PAR ; ++currNum)
	{
		freInt(numbers[currNum]);
	}
	freInt(newInt);
	destroyRangeTree(tree);
	return EXIT_SUCCESS;
}
#endif
