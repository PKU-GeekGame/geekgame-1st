#include "stdafx.h"


// See https://luckytoilet.wordpress.com/2012/12/23/2125/
// which describes the "tank" algorithm or the method to handle the perimeter
// first we must create independent lists of tiles that are connected
// then we can run all possible combinations for each list
// to get a count of a mine for each entry
// If we have an entry with 0% probability we can expose this entry
// If we have an entry with 100% probability we can add a mine there
// Otherwise we can pick the lowest proabability for our choice
// For the end game include all entries in one list 
// to find entries with no possible mine due to the mine quota

int minX, maxX;								// region touched by trying to make moves
int minY, maxY;

int		Combinations[MAX_NEIGHBORS + 1][MAX_NEIGHBORS + 2] =
{
	{ 1, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 2, 1, 0, 0, 0, 0, 0, 0 },
	{ 1, 3, 3, 1, 0, 0, 0, 0, 0 },
	{ 1, 4, 6, 4, 1, 0, 0, 0, 0 },
	{ 1, 5, 10, 10, 5, 1, 0, 0, 0 },
	{ 1, 6, 15, 20, 15, 6, 1, 0, 0 },
	{ 1, 7, 21, 35, 35, 21, 7, 1, 0 },
	{ 1, 8, 28, 56, 70, 56, 28, 8, 1 },
};

int		bestx, besty;							// best move
int		maxCounts;								// max counts for solutions
int		maxLists;								// max number of sequence lists (see MAX_LISTS)
int		maxPerimeter;							// max number of entries in listArray (see MAX_LIST_LENGTH)
int		maxSinglePerimeter;						// longest single perimeter
int		maxNeighbors;							// max number of entries in neighborArray 
int		numEndPlays;
int		numMissedEndPlays;						// number of missed end plays with <= maxSolutions
int		maxEndPlayLength;						// length of max end play
int		numEndPlaySmall;
int		numEndPlayLarge;
int		maxListSolutions;
int		maxCompletedSolutions;
int		gametreeUsed;
int		gametreeUsedEarly;
int		gametreeEntriesUsed;
int		gametreeHelped;
int		gametreeLinksNeeded;
int		gametreeMaxDepth;
int		superLocMaxCandidates;					// max candidates found
int		superLocMultiplierUsed;					// number of times we used the threshold
int		superLocMultiplierImproved;				// number of times the old choice failed and this one succeeds
int		superLocMultiplierFailed;				// number of times the new choice failed and the old one succeeds
int		superLocMultiplyUsed;					// number of times we used the multiply to use a higher clear count entry
int		totalMaxCounts;							// max counts for solutions (see MAX_COUNTS)
int		totalMaxLists;							// max number of sequence lists (see MAX_LISTS)
int		totalMaxPerimeter;						// max number of entries in listArray (see MAX_LIST_LENGTH)
int		totalMaxSinglePerimeter;				// longest single perimeter
int		totalMaxNeighbors;						// max number of entries in neighborArray 
int		totalNumEndPlays;						// number of times we use end play
int		totalNumMissedEndPlays;					// number of missed end plays with <= maxSolutions
int		totalMaxEndPlayLength;					// length of max end play
int		totalNumEndPlaySmall;					// count small end plays (< SMALL_SOLUTIONS)
int		totalNumEndPlayLarge;					// count large end plays (> maxSolutions)
int		totalMaxListSolutions;					// max number of tank solutions
int		totalMaxCompletedSolutions;				// max number of tank solutions
int		totalGametreeUsed;						// number of times we used a gametree
int		totalGametreeUsedEarly;					// number of times we used a gametree early
int		totalGametreeEntriesUsed;				// max number of entries we used for a gametree
int		totalGametreeHelped;					// number of times gametree improved odds
int		totalGametreeLinksNeeded;				// number of game tree links needed 
int		totalGametreeMaxDepth;					// max depth of the gamet tree search
int		totalSuperLocMaxCandidates;				// max candidates found
int		totalSuperLocMultiplierUsed;
int		totalSuperLocMultiplierImproved;
int		totalSuperLocMultiplierFailed;
int		totalSuperLocMultiplyUsed;
int		superNext;								// number of super locations
double	bestSuperLocProgress;					// best progress we found
SuperLoc *	bestSuperLoc;						// best super location found so far (maybe NULL)
SuperLoc	SuperLocArray[MAX_SUPERLOCATIONS];
int		gameTreeNext;							// number of game tree locations
GameTree	GameTreeArray[MAX_GAMETREELOCATIONS];
int		neighborNext;							// next neighbor entry to use
int		neighborStart[MAX_LISTS];				// start and end indices for neighborArray
int		neighborEnd[MAX_LISTS];
Neighbor neighborArray[MAX_NEIGHBORS_LIST];		// list of all neighbors
int		solutionValuesNext;						// next entry into solutionValues
char	solutionValues[MAX_SOLUTION_VALUES];	// values for all solutions
int		gametreeLinksNext;						// next available gametreeLinks entry 
Link	gametreeLinks[MAX_SOLUTIONS];			// links to all solutions for our game tree (end play only)
int		listNext;								// next entry to use
List	listArray[MAX_LIST_LENGTH];				// list entries
int		solutionLinksNext;						// next available entry in solutionLinks[]
Link	solutionLinks[MAX_SOLUTION_VALUES];		// links for solutions[]
Solutions solutions[MAX_SOLUTIONS];				// link all solutions 
int		listUsed[MAX_HEIGHT][MAX_WIDTH];		// index into listArray (-1 if none)
int		adjacentCount[MAX_HEIGHT][MAX_WIDTH];	// count of adjacent to perimeter or adjacent to free entries
int		adjacentMask[MAX_HEIGHT][MAX_WIDTH];	// mask for adjacent entries
int		lists;									// number of lists found
int		similarCount;							// count of similar entries in listArray to be treated differently
int		culledCount;							// number of neighbors culled
int		deadCount;								// number of dead perimeters
int		completedSolutions;						// number of solutions processed 
int		conflictsInSolutions;					// number of times we found conflicts when creating solutions
int		minesUsed;								// current count of mines used
bool	listEnclosed[MAX_LISTS];				// is this list enclosed i.e. no free entries surrounding it
bool	listDead[MAX_LISTS];					// is this list dead (i.e. all entries are dead)
int		listStart[MAX_LISTS];					// starting index for a list 
int		listEnd[MAX_LISTS];						// last entry for a list
int		listSolutions[MAX_LISTS][MAX_COUNTS];	// number of possible choices for each list and each count of mines
int		listMinMines[MAX_LISTS];				// min number of mines used
int		listMaxMines[MAX_LISTS];				// max number of mines used
int		nonPerimeter;							// number of non-perimeter entries (not in any sequence)
int		nonPerimeterInEndGame;					// count of non-perimter in end game
int		nonPerimeterMines;						// total of all mines in the non-perimeter area for all solutions
double	totalSolutions;							// total number of solutions to the puzzle
double	weightedSolutions;						// total weighted number of solutions 
double	nonPerimeterProb;						// non-perimeter prob
double	bestProb;								// best probability for the perimeter


enum
{
	MASK_UL = 0x1,
	MASK_U = 0x2,
	MASK_UR = 0x4,
	MASK_LX = 0x8,
	MASK_RX = 0x80,
	MASK_LL = 0x10,
	MASK_L = 0x20,
	MASK_LR = 0x40,
	MASK_UPPER = MASK_UL | MASK_U | MASK_UR,
	MASK_LEFT = MASK_UL | MASK_LX | MASK_LL,
	MASK_RIGHT = MASK_UR | MASK_RX | MASK_LR,
	MASK_LOWER = MASK_LL | MASK_L | MASK_LR,
} MASKS;

int MaskCount[0x100] = 
{ // 0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15
	0,	1,	1,	2,	1,	2,	2,	3,	1,	2,	2,	3,	2,	3,	3,	4,	// 0x00
	1,	2,	2,	3,	2,	3,	3,	4,	2,	3,	3,	4,	3,	4,	4,	5,	// 0x10
	1,	2,	2,	3,	2,	3,	3,	4,	2,	3,	3,	4,	3,	4,	4,	5,	// 0x20
	2,	3,	3,	4,	3,	4,	4,	5,	3,	4,	4,	5,	4,	5,	5,	6,	// 0x30
	1,	2,	2,	3,	2,	3,	3,	4,	2,	3,	3,	4,	3,	4,	4,	5,	// 0x40
	2,	3,	3,	4,	3,	4,	4,	5,	3,	4,	4,	5,	4,	5,	5,	6,	// 0x50
	2,	3,	3,	4,	3,	4,	4,	5,	3,	4,	4,	5,	4,	5,	5,	6,	// 0x60
	3,	4,	4,	5,	4,	5,	5,	6,	4,	5,	5,	6,	5,	6,	6,	7,	// 0x70
	1,	2,	2,	3,	2,	3,	3,	4,	2,	3,	3,	4,	3,	4,	4,	5,	// 0x80
	2,	3,	3,	4,	3,	4,	4,	5,	3,	4,	4,	5,	4,	5,	5,	6,	// 0x90
	2,	3,	3,	4,	3,	4,	4,	5,	3,	4,	4,	5,	4,	5,	5,	6,	// 0xA0
	3,	4,	4,	5,	4,	5,	5,	6,	4,	5,	5,	6,	5,	6,	6,	7,	// 0xB0
	2,	3,	3,	4,	3,	4,	4,	5,	3,	4,	4,	5,	4,	5,	5,	6,	// 0xC0
	3,	4,	4,	5,	4,	5,	5,	6,	4,	5,	5,	6,	5,	6,	6,	7,	// 0xD0
	3,	4,	4,	5,	4,	5,	5,	6,	4,	5,	5,	6,	5,	6,	6,	7,	// 0xE0
	4,	5,	5,	6,	5,	6,	6,	7,	5,	6,	6,	7,	6,	7,	7,	8,	// 0xF0
};


void CreateList(int x, int y, bool endGame = false);
void CreateSolutions(int list, int index, int maxCount, bool saveSolutions);
int CheckSolutions(int list, int index, int max);


bool Equal(double a, double b)
{
	double diff = a - b;
	if (fabs(diff) < 0.0000001)
		return true;
	return false;
}

bool GreaterThan(double a, double b)
{
	if (a - 0.0000001 > b)
		return true;
	return false;
}

bool LessThan(double a, double b)
{
	if (a + 0.0000001 < b)
		return true;
	return false;
}


void AddNeighbor(List * list, int x, int y)
{
	Neighbor * next = &neighborArray[lists];
	for (; next < &neighborArray[neighborNext]; next++)
	{
		if (next->x == x && next->y == y)
			goto FOUND;										// already on the list
	}
	neighborNext++;
	next->x = x;
	next->y = y;
	next->unexposed = unexposed[y][x];
	next->needed = needed[y][x];
	next->count = 0;
FOUND:
	next->entries[next->count++] = list;
	list->neighbors[list->numNeighbors] = next;
	list->numNeighbors++;
}


bool Match(Neighbor * n1, Neighbor * n2)
{
	if (n1->count != n2->count)
		return false;
	int n;
	for (n = 0; n < n1->count; n++)
	{
		int m;
		for (m = 0; m < n2->count; m++)
		{
			if (n1->entries[n] == n2->entries[m])
				goto NEXT;
		}
		return false;
NEXT:;
	}
	return true;
}


void RemoveNeighbor(List * list, Neighbor * neighbor)
{
	int n;
	for (n = 0; n < list->numNeighbors; n++)
	{
		if (list->neighbors[n] == neighbor)
			break;
	}
	while (n < list->numNeighbors - 1)
	{
		list->neighbors[n] = list->neighbors[n + 1];
		n++;
	}
	list->neighbors[n] = NULL;
	list->numNeighbors--;
}


void CullNeighbors(int start, int end)
{
	for (int i = start; i <= end; i++)
	{
		if (neighborArray[i].count <= 1)					// already removed or 1 it will not work
			continue;
		List * list = neighborArray[i].entries[0];
		// pick any perimeter entry
		// find any neighbors that match the one we started with
		// remove it from all perimeter entries
		for (int n = 0; n < list->numNeighbors; n++)
		{
			if (list->neighbors[n] == &neighborArray[i])
				continue;
			Neighbor * neighbor = list->neighbors[n];
			if (Match(neighbor, &neighborArray[i]))
			{
				// remove neighbor from all those that point to it
				for (int m = 0; m < neighbor->count; m++)
					RemoveNeighbor(neighbor->entries[m], neighbor);
				neighbor->count = 0;
				culledCount++;
				break;
			}
		}
	}
}


void CheckNeighbors(int x, int y)
{
	if (x > 0 && y > 0
	&& exposed[y - 1][x - 1] < 0							// unexposed
	&& listUsed[y - 1][x - 1] < 0)							// not on some list
		CreateList(x - 1, y - 1);
	if (y > 0
	&& exposed[y - 1][x] < 0								// unexposed
	&& listUsed[y - 1][x] < 0)								// not on some list
		CreateList(x, y - 1);
	if (x < width - 1 && y > 0
	&& exposed[y - 1][x + 1] < 0							// unexposed
	&& listUsed[y - 1][x + 1] < 0)							// not on some list
		CreateList(x + 1, y - 1);
	if (x > 0
	&& exposed[y][x - 1] < 0								// unexposed
	&& listUsed[y][x - 1] < 0)								// not on some list
		CreateList( x - 1, y);
	if (x < width - 1
	&& exposed[y][x + 1] < 0								// unexposed
	&& listUsed[y][x + 1] < 0)								// not on some list
		CreateList(x + 1, y);
	if (x > 0 && y < height - 1
	&& exposed[y + 1][x - 1] < 0							// unexposed
	&& listUsed[y + 1][x - 1] < 0)							// not on some list
		CreateList(x - 1, y + 1);
	if (y < height - 1
	&& exposed[y + 1][x] < 0								// unexposed
	&& listUsed[y + 1][x] < 0)								// not on some list
		CreateList(x, y + 1);
	if (x < width - 1 && y < height - 1
	&& exposed[y + 1][x + 1] < 0							// unexposed
	&& listUsed[y + 1][x + 1] < 0)							// not on some list
		CreateList(x + 1, y + 1);
}


void CreateList(int x, int y, bool endGame)
{
	listUsed[y][x] = listNext;
	List * list = &listArray[listNext];
	list->x = x;
	list->y = y;
	list->numNeighbors = 0;
	list->adjacentLists = 0;
	list->dead = false;
	list->numSimilar = 0;
	list->similar = NULL;
	if (list != &listArray[listStart[lists]])
		(list - 1)->next = list;							// previous points to us
	list->next = &listArray[listStart[lists]];				// next is the first entry
	list->tried = -1;
	list->list = lists;
	list->merged = -1;
	listNext++;
	if (endGame)
	{
		list->prob = nonPerimeterProb;
		return;
	}
	if (x > 0 && y > 0)
	{
		if (exposed[y - 1][x - 1] > 0
		&& exposed[y - 1][x - 1] < EXPOSE_MINE)
		{
			AddNeighbor(list, x - 1, y - 1);
			CheckNeighbors(x - 1, y - 1);
		}
	}
	if (y > 0)
	{
		if (exposed[y - 1][x] > 0
		&& exposed[y - 1][x] < EXPOSE_MINE)
		{
			AddNeighbor(list, x, y - 1);
			CheckNeighbors(x, y - 1);
		}
	}
	if (x < width - 1 && y > 0)
	{
		if (exposed[y - 1][x + 1] > 0
		&& exposed[y - 1][x + 1] < EXPOSE_MINE)
		{
			AddNeighbor(list, x + 1, y - 1);
			CheckNeighbors(x + 1, y - 1);
		}
	}
	if (x > 0)
	{
		if (exposed[y][x - 1] > 0
			&& exposed[y][x - 1] < EXPOSE_MINE)
		{
			AddNeighbor(list, x - 1, y);
			CheckNeighbors(x - 1, y);
		}
	}
	if (x < width - 1)
	{
		if (exposed[y][x + 1] > 0
		&& exposed[y][x + 1] < EXPOSE_MINE)
		{
			AddNeighbor(list, x + 1, y);
			CheckNeighbors(x + 1, y);
		}
	}
	if (x > 0 && y < height - 1)
	{
		if (exposed[y + 1][x - 1] > 0
		&& exposed[y + 1][x - 1] < EXPOSE_MINE)
		{
			AddNeighbor(list, x - 1, y + 1);
			CheckNeighbors(x - 1, y + 1);
		}
	}
	if (y < height - 1)
	{
		if (exposed[y + 1][x] > 0
		&& exposed[y + 1][x] < EXPOSE_MINE)
		{
			AddNeighbor(list, x, y + 1);
			CheckNeighbors(x, y + 1);
		}
	}
	if (x < width - 1 && y < height - 1)
	{
		if (exposed[y + 1][x + 1] > 0
		&& exposed[y + 1][x + 1] < EXPOSE_MINE)
		{
			AddNeighbor(list, x + 1, y + 1);
			CheckNeighbors(x + 1, y + 1);
		}
	}
}


void UpdateFree(int x, int y, int list)
{
	if (exposed[y][x] >= 0)									// cannot be a free entry
		return;
	if (adjacentCount[y][x] != 0)							// already done
	{
		listEnclosed[list] = false;
		return;
	}
	if (x > 0 && y > 0
	&& listUsed[y - 1][x - 1] >= 0)							// on the perimeter
	{
		adjacentMask[y][x] |= MASK_UL;
		adjacentCount[y][x]++;
		listEnclosed[list] = false;
	}
	if (y > 0
	&& listUsed[y - 1][x] >= 0)								// on the perimeter
	{
		adjacentMask[y][x] |= MASK_U;
		adjacentCount[y][x]++;
		listEnclosed[list] = false;
	}
	if (x < width - 1 && y > 0
	&& listUsed[y - 1][x + 1] >= 0)							// on the perimeter
	{
		adjacentMask[y][x] |= MASK_UR;
		adjacentCount[y][x]++;
		listEnclosed[list] = false;
	}
	if (x > 0
	&& listUsed[y][x - 1] >= 0)								// on the perimeter
	{
		adjacentMask[y][x] |= MASK_LX;
		adjacentCount[y][x]++;
		listEnclosed[list] = false;
	}
	if (x < width - 1
	&& listUsed[y][x + 1] >= 0)								// on the perimeter
	{
		adjacentMask[y][x] |= MASK_RX;
		adjacentCount[y][x]++;
		listEnclosed[list] = false;
	}
	if (x > 0 && y < height - 1
	&& listUsed[y + 1][x - 1] >= 0)							// on the perimeter
	{
		adjacentMask[y][x] |= MASK_LL;
		adjacentCount[y][x]++;
		listEnclosed[list] = false;
	}
	if (y < height - 1
	&& listUsed[y + 1][x] >= 0)								// on the perimeter
	{
		adjacentMask[y][x] |= MASK_L;
		adjacentCount[y][x]++;
		listEnclosed[list] = false;
	}
	if (x < width - 1 && y < height - 1
	&& listUsed[y + 1][x + 1] >= 0)							// on the perimeter
	{
		adjacentMask[y][x] |= MASK_LR;
		adjacentCount[y][x]++;
		listEnclosed[list] = false;
	}
}


void UpdateAdjacent(int x, int y, int list)
{
	if (x > 0 && y > 0)
	{
		if (listUsed[y - 1][x - 1] >= 0)					// list entry
		{
			if (listArray[listUsed[y - 1][x - 1]].list != list)
			{
				listArray[listUsed[y][x]].adjacentLists++;
				listEnclosed[list] = false;					// adjacent to another list
				adjacentCount[y][x]++;
				listArray[listUsed[y - 1][x - 1]].merged = list;
			}
		}
		else if (exposed[y - 1][x - 1] < 0)
		{
			adjacentMask[y][x] |= MASK_UL;
			adjacentCount[y][x]++;
			UpdateFree(x - 1, y - 1, list);
		}
	}
	if (y > 0)
	{
		if (listUsed[y - 1][x] >= 0)						// list entry
		{
			if (listArray[listUsed[y - 1][x]].list != list)
			{
				listArray[listUsed[y][x]].adjacentLists++;
				listEnclosed[list] = false;					// adjacent to another list
				adjacentCount[y][x]++;
				listArray[listUsed[y - 1][x]].merged = list;
			}
		}
		else if (exposed[y - 1][x] < 0)
		{
			adjacentMask[y][x] |= MASK_U;
			adjacentCount[y][x]++;
			UpdateFree(x, y - 1, list);
		}
	}
	if (x < width - 1 && y > 0)
	{
		if (listUsed[y - 1][x + 1] >= 0)					// list entry
		{
			if (listArray[listUsed[y - 1][x + 1]].list != list)
			{
				listArray[listUsed[y][x]].adjacentLists++;
				listEnclosed[list] = false;					// adjacent to another list
				adjacentCount[y][x]++;
				listArray[listUsed[y][x]].merged = list;
			}
		}
		else if(exposed[y - 1][x + 1] < 0)
		{
			adjacentMask[y][x] |= MASK_UR;
			adjacentCount[y][x]++;
			UpdateFree(x + 1, y - 1, list);
		}
	}
	if (x > 0)
	{
		if (listUsed[y][x - 1] >= 0)						// list entry
		{
			if (listArray[listUsed[y][x - 1]].list != list)
			{
				listArray[listUsed[y][x]].adjacentLists++;
				listEnclosed[list] = false;					// adjacent to another list
				adjacentCount[y][x]++;
				listArray[listUsed[y][x - 1]].merged = list;
			}
		}
		else if (exposed[y][x - 1] < 0)
		{
			adjacentMask[y][x] |= MASK_LX;
			adjacentCount[y][x]++;
			UpdateFree(x - 1, y, list);
		}
	}
	if (x < width - 1)
	{
		if (listUsed[y][x + 1] >= 0)						// list entry
		{
			if (listArray[listUsed[y][x + 1]].list != list)
			{
				listArray[listUsed[y][x]].adjacentLists++;
				listEnclosed[list] = false;					// adjacent to another list
				adjacentCount[y][x]++;
				listArray[listUsed[y][x + 1]].merged = list;
			}
		}
		else if (exposed[y][x + 1] < 0)
		{
			adjacentMask[y][x] |= MASK_RX;
			adjacentCount[y][x]++;
			UpdateFree(x + 1, y, list);
		}
	}
	if (x > 0 && y < height - 1)
	{
		if (listUsed[y + 1][x - 1] >= 0)					// list entry
		{
			if (listArray[listUsed[y + 1][x - 1]].list != list)
			{
				listArray[listUsed[y][x]].adjacentLists++;
				listEnclosed[list] = false;					// adjacent to another list
				adjacentCount[y][x]++;
				listArray[listUsed[y + 1][x - 1]].merged = list;
			}
		}
		else if (exposed[y + 1][x - 1] < 0)
		{
			adjacentMask[y][x] |= MASK_LL;
			adjacentCount[y][x]++;
			UpdateFree(x - 1, y + 1, list);
		}
	}
	if (y < height - 1)
	{
		if (listUsed[y + 1][x] >= 0)						// list entry
		{
			if (listArray[listUsed[y + 1][x]].list != list)
			{
				listArray[listUsed[y][x]].adjacentLists++;
				listEnclosed[list] = false;					// adjacent to another list
				adjacentCount[y][x]++;
				listArray[listUsed[y + 1][x]].merged = list;
			}
		}
		else if (exposed[y + 1][x] < 0)
		{
			adjacentMask[y][x] |= MASK_L;
			adjacentCount[y][x]++;
			UpdateFree(x, y + 1, list);
		}
	}
	if (x < width - 1 && y < height - 1)
	{
		if (listUsed[y + 1][x + 1] >= 0)					// list entry
		{
			if (listArray[listUsed[y + 1][x + 1]].list != list)
			{
				listArray[listUsed[y][x]].adjacentLists++;
				listEnclosed[list] = false;					// adjacent to another list
				adjacentCount[y][x]++;
				listArray[listUsed[y + 1][x + 1]].merged = list;
			}
		}
		else if (exposed[y + 1][x + 1] < 0)
		{
			adjacentMask[y][x] |= MASK_LR;
			adjacentCount[y][x]++;
			UpdateFree(x + 1, y + 1, list);
		}
	}
}


void StartList(int x, int y)
{
	if (listUsed[y][x] >= 0							// already on some list
	|| exposed[y][x] >= 0							// find an unexposed tile
	|| neighbors[y][x] == 0)						// we need real neighbors (not just mines)
		return;
	listStart[lists] = listNext;
	listEnclosed[lists] = true;
	listDead[lists] = true;
	neighborStart[lists] = neighborNext;
	CreateList(x, y);
	neighborEnd[lists] = neighborNext - 1;
	listEnd[lists] = listNext - 1;
	lists++;
}


void CreatePerimeterLists()
{
	listNext = 0;
	neighborNext = 0;
	lists = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			listUsed[y][x] = -1;
		}
	}
	nonPerimeterMines = 0;
	nonPerimeterInEndGame = 0;
	culledCount = 0;
	deadCount = 0;
	// create separate lists for each connected entry
	if (topToBottomPriority)
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				StartList(x, y);
			}
		}
	}
	else
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				StartList(x, y);
			}
		}
	}
	if (maxPerimeter < listNext)
		maxPerimeter = listNext;
	if (maxNeighbors < neighborNext)
		maxNeighbors = neighborNext;
	nonPerimeter = remainingCount + mineCount - listNext;	// number of entries remaining that are not in any sequence
	if (maxLists < lists)
		maxLists = lists;

	// set adjacentCount and listEnclosed 
	memset(adjacentCount, 0, sizeof(adjacentCount));
	memset(adjacentMask, 0, sizeof(adjacentMask));
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (listUsed[y][x] >= 0)						// on some list
			{
				int list = listArray[listUsed[y][x]].list;
				UpdateAdjacent(x, y, list);					// update counts for adjacent to perimeter
			}
		}
	}

#if 1	// turn off similar neighbors for testing
	// create blocks or similar list entries
	similarCount = 0;
	for (int n = 0; n < neighborNext; n++)
	{
		for (int i = 0; i < neighborArray[n].count; i++)
		{
			List * list = neighborArray[n].entries[i];
			if (list->numNeighbors != 1)						// skip perimeter with more than one neighbor
				continue;
			list->numSimilar++;									// how many entries are part of this block
			List * firstList = list;
			list->similar = firstList;							// point to ourselves
			for (i++; i < neighborArray[n].count; i++)
			{
				list = neighborArray[n].entries[i];
				if (list->numNeighbors != 1)
					continue;
				list->similar = firstList;
				firstList->numSimilar++;						// how many entries are part of this block
			}
			similarCount += firstList->numSimilar;
			for (int m = 0; m < neighborArray[n].count; m++)
			{
				List * list = neighborArray[n].entries[m];
				if (list == firstList
				|| list->similar == firstList)
				{
					list->numSimilar = firstList->numSimilar;
				}
			}
		}
	}
#endif

	// NOTE: we must do this after we create similar lists
	// otherwise we get incorrect similar entries
	// NOTE: this reduces solution creation time but only by a small amount < 10%
	// so remove this if the number of solutions gets reduced
	for (int i = 0; i < lists; i++)
		CullNeighbors(neighborStart[i], neighborEnd[i]);		// reduce our neighbors
}


bool Conflict(List * list, int maxCount)
{
	for (int n = 0; n < list->numNeighbors; n++)
	{
		if (list->neighbors[n]->needed < 0)
		{
			conflictsInSolutions++;
			return true;
		}
		if (list->neighbors[n]->needed > 0
		&& list->neighbors[n]->needed > list->neighbors[n]->unexposed)
		{
			conflictsInSolutions++;
			return true;
		}
	}
	return false;
}


void TryMine(int list, int index, int maxCount, bool saveSolutions)
{
	if (minesUsed == maxCount)									// no mines left to add
		return;
	listArray[index].tried = 1;									// place mine
	minesUsed++;
	for (int n = 0; n < listArray[index].numNeighbors; n++)		// reflect that in our neighbors
	{
		listArray[index].neighbors[n]->needed--;
		listArray[index].neighbors[n]->unexposed--;
	}
	if (Conflict(&listArray[index], maxCount) == false)			// we have no conflict so far
	{
		CreateSolutions(list, index+1, maxCount, saveSolutions);	// continue until we are done or abort
	}
	listArray[index].tried = -1;				
	minesUsed--;
	for (int n = 0; n < listArray[index].numNeighbors; n++)		// undo mine
	{
		listArray[index].neighbors[n]->needed++;
		listArray[index].neighbors[n]->unexposed++;
	}
}


void TryExpose(int list, int index, int maxCount, bool saveSolutions)
{
	listArray[index].tried = 0;									// expose this
	for (int n = 0; n < listArray[index].numNeighbors; n++)		// reflect that in our neighbors
	{
		listArray[index].neighbors[n]->unexposed--;
	}
	if (Conflict(&listArray[index], maxCount) == false)
	{
		CreateSolutions(list, index+1, maxCount, saveSolutions);	// continue until we are done or abort 
	}
	listArray[index].tried = -1;
	for (int n = 0; n < listArray[index].numNeighbors; n++)		// undo expose
	{
		listArray[index].neighbors[n]->unexposed++;
	}
}


int CountExposedMines(int x, int y)
{
	int count = 0;
	count += (x > 0 && y > 0 && exposed[y - 1][x - 1] == EXPOSE_MINE ? 1 : 0);
	count += (y > 0 && exposed[y - 1][x] == EXPOSE_MINE ? 1 : 0);
	count += (x < width - 1 && y > 0 && exposed[y - 1][x + 1] == EXPOSE_MINE ? 1 : 0);
	count += (x > 0 && exposed[y][x - 1] == EXPOSE_MINE ? 1 : 0);
	count += (exposed[y][x] == EXPOSE_MINE ? 1 : 0);
	count += (x < width - 1 && exposed[y][x + 1] == EXPOSE_MINE ? 1 : 0);
	count += (x > 0 && y < height - 1 && exposed[y + 1][x - 1] == EXPOSE_MINE ? 1 : 0);
	count += (y < height - 1 && exposed[y + 1][x] == EXPOSE_MINE ? 1 : 0);
	count += (x < width - 1 && y < height - 1 && exposed[y + 1][x + 1] == EXPOSE_MINE ? 1 : 0);
	return count;
}


int CountFreeEntries(int x, int y)
{
	int count = 0;
	count += (x > 0 && y > 0 && exposed[y - 1][x - 1] < 0 ? 1 : 0);
	count += (y > 0 && exposed[y - 1][x] < 0 ? 1 : 0);
	count += (x < width - 1 && y > 0 && exposed[y - 1][x + 1] < 0 ? 1 : 0);
	count += (x > 0 && exposed[y][x - 1] < 0 ? 1 : 0);
	count += (exposed[y][x] < 0 ? 1 : 0);
	count += (x < width - 1 && exposed[y][x + 1] < 0 ? 1 : 0);
	count += (x > 0 && y < height - 1 && exposed[y + 1][x - 1] < 0 ? 1 : 0);
	count += (y < height - 1 && exposed[y + 1][x] < 0 ? 1 : 0);
	count += (x < width - 1 && y < height - 1 && exposed[y + 1][x + 1] < 0 ? 1 : 0);
	return count;
}


void RestoreExposed(int list)
{
	if (gametreeEntriesUsed < solutionValuesNext)
		gametreeEntriesUsed = solutionValuesNext;
	for (int i = listStart[list]; i <= listEnd[list]; i++)
	{
		exposed[listArray[i].y][listArray[i].x] = -1;
	}
}


struct Extras
{
	int count;											// number of this neighbor that are adjacent
	Neighbor *	neighbor;
}	extras[4];
int	numExtras;

int AddExtra(List * list)
{
	if (list->numSimilar == 1)							// only one entry in this block
	{
		if (list->neighbors[0]->needed == 0)			// it must be an open entry
			return 0;
		return 1;										// it must be a mine
	}
	int value = 0;
	for (int i = 0; i < numExtras; i++)
	{
		if (extras[i].neighbor == list->neighbors[0])
		{
			extras[i].count++;
			if (extras[i].count == list->numSimilar)	// we are next to all the entries
			{
				for ( i++; i < numExtras; i++)			// move higher entries down
				{
					extras[i - 1] = extras[i];
				}
				numExtras--;
				value += list->neighbors[0]->needed;	// number of mines in this area
			}
			return value;
		}
	}
	extras[numExtras].neighbor = list->neighbors[0];
	extras[numExtras].count = 1;
	numExtras++;
	return value;
}


int AdjacentMines(int list, List * ptr)
{
	numExtras = 0;
	int value = mines[ptr->y][ptr->x];					// old mines next to this location
	for (int ii = listStart[list]; ii <= listEnd[list]; ii++)
	{
		if (&listArray[ii] == ptr)						// exclude ourself
			continue;
		if (listArray[ii].tried == 0)					// not a mine
			continue;
		int y = listArray[ii].y - ptr->y;
		if (y < -1 || y > 1)
			continue;
		int x = listArray[ii].x - ptr->x;
		if (x < -1 || x > 1)
			continue;
		if (listArray[ii].tried < 0)					// similar entry
		{
			value += AddExtra(&listArray[ii]);
			continue;
		}
		value++;
	}
	return value;
}


void UpdateValues(List * ptr, int value, int add)
{
	if (ptr->minValue > value)
		ptr->minValue = value;
	if (ptr->maxValue < value)
		ptr->maxValue = value;
	if (ptr->minValue == ptr->maxValue					// always the same value
	&&	ptr->adjacentLists == 0							// not adjacent to any other lists
	&&  adjacentCount[ptr->y][ptr->x] == 0)				// and not adjacent open areas
		ptr->dead = true;								// this entry is dead so far
	else
		ptr->dead = false;
}


// This may not be completely accurate because one similar entry may affect another
// However I have not found such a case yet
// So for one similar entry we correctly compute the values 
// and for more than one we recurse on each each entry
// Normally for independent events with m mines in n similar entries
// If we see c of those n entries then the exact probability of i mines is
// P = Comb(i,c)*Comb(m-i,n-c)/Comb(m,n) 
// given i <= c and m >= n and n >= c and m-i <= n-c
// c = extras[0].count
// m = extras[0].neighbor->needed
// n = extras[0].neighbor->unexposed (or ptr->numSimilar)
//
void UpdateExtraValues(List * ptr, int value, int multiply, int index)
{
	int n, m, c, denom;
	n = extras[index].neighbor->unexposed;
	if (ptr->numSimilar != 0
	&&  ptr->neighbors[0] == extras[index].neighbor)	// this entry is part of that block
		n--;
	m = extras[index].neighbor->needed;
	c = extras[index].count;
	denom = Combinations[n][m];
	for (int i = 0; i <= c; i++)
	{
		if (m < i)
			break;
		if (n - c < m - i)
			continue;
		int add = multiply * Combinations[c][i] * Combinations[n - c][m - i] / denom;
		if (index == 0)
			UpdateValues(ptr, value + i, add);
		else
			UpdateExtraValues(ptr, value + i, add, index - 1);	// do this for other blocks
	}
}


void CountCompletedSolutions(int list, int maxCount, bool saveSolutions)
{
	// add multiplier for similar entries and update true minesUsed
	int multiply = 1;
	int minesUsedAdjust = 0;
	for (int i = listStart[list]; i <= listEnd[list]; i++)
	{
		if (listArray[i].similar != &listArray[i])				// not the parent
			continue;
		int needed = listArray[i].neighbors[0]->needed;
		minesUsedAdjust += needed;
		multiply *= Combinations[listArray[i].numSimilar][needed];
	}
	if (minesUsed + minesUsedAdjust > maxCount)					// this cannot be a solution
	{
		conflictsInSolutions++;
		return;
	}
	completedSolutions++;
	minesUsed += minesUsedAdjust;
	// count completed solution
	int entry;
	if (listMaxMines[list] == 0)								// first time here
	{
		for (int i = listStart[list]; i <= listEnd[list]; i++)
		{
			for (int n = 0; n < MAX_COUNTS; n++)
				listArray[i].counts[n] = 0;
			if (saveSolutions)									// only when saving solutions
			{
				int m = i - listStart[list];
				solutions[m].index = i;							// index into listArray
				solutions[m].total = 0;
				solutions[m].max = 0;
				solutions[m].min = 0;
				solutions[m].order = m;
				for (int n = 0; n <= MAX_NEIGHBORS; n++)
				{
					solutions[m].solutions[n].index = 0;		// realy a count of links
					solutions[m].solutions[n].next = NULL;
				}
			}
		}
		listMinMines[list] = minesUsed;
		listMaxMines[list] = minesUsed;
		entry = 0;
	}
	else if (listMinMines[list] > minesUsed)					// new min count
	{
		int count = listMaxMines[list] - minesUsed + 1;			// number of different counts
		if (maxCounts < count)
			maxCounts = count;
		if (count >= MAX_COUNTS)								// use lowest one if necessary
		{
			if (testAllResults != NULL)
				fprintf(testAllResults, "\n***** ERROR: Puzzle=%d Unable to create all sets of counts (%d >= %d)\n", testing, count, MAX_COUNTS);
			count = MAX_COUNTS - 1;
			minesUsed = listMaxMines[list] - count;
		}
		count = listMinMines[list] - minesUsed;				// how many to move up
		for (int i = listStart[list]; i <= listEnd[list]; i++)
		{
			for (int n = listMaxMines[list] - listMinMines[list]; n >= 0; n--)
				listArray[i].counts[n + count] = listArray[i].counts[n];
			for (int n = 0; n < count; n++)
				listArray[i].counts[n] = 0;
		}
		for (int n = listMaxMines[list] - listMinMines[list]; n >= 0; n--)
			listSolutions[list][n + count] = listSolutions[list][n];
		for (int n = 0; n < count; n++)
			listSolutions[list][n] = 0;
		listMinMines[list] = minesUsed;
		entry = 0;
	}
	else if (listMaxMines[list] < minesUsed)				// new max count
	{
		int count = minesUsed - listMinMines[list] + 1;		// number of different counts
		if (maxCounts < count)
			maxCounts = count;
		if (count >= MAX_COUNTS)								// use highest one if necessary
		{
			if (testAllResults != NULL)
				fprintf(testAllResults, "\n***** ERROR: Puzzle=%d Unable to create all sets of counts (%d >= %d)\n", testing, count, MAX_COUNTS);
			count = MAX_COUNTS - 1;
			minesUsed = listMinMines[list] + count;
		}
		listMaxMines[list] = minesUsed;
		entry = minesUsed - listMinMines[list];
	}
	else
		entry = minesUsed - listMinMines[list];
	listSolutions[list][entry] += multiply;							// count these solutions

	for (int i = listStart[list]; i <= listEnd[list]; i++)
	{
		if (listArray[i].tried < 0)									// this must be part of an untested block (see ->similar)
		{
			int count = listArray[i].numSimilar;
			int needed = listArray[i].neighbors[0]->needed;
			int mines = multiply * needed / count;					// count of mines here
			listArray[i].counts[entry] += mines;
			int value = AdjacentMines(list, &listArray[i]);
			int remain = multiply - mines;							// count of non-mines solutions 
			if (remain != 0)
			{
				if (numExtras > 0)
					UpdateExtraValues(&listArray[i], value, remain, numExtras - 1); // update values and dead
				else
					UpdateValues(&listArray[i], value, remain);		// update values and dead flag
			}
		}
		else if (listArray[i].tried > 0)							// count solutions when this is a mine
			listArray[i].counts[entry] += multiply;
		else
		{
			int value = AdjacentMines(list, &listArray[i]);			// count of mines adjacent
			if (numExtras > 0)
				UpdateExtraValues(&listArray[i], value, multiply, numExtras - 1);	// update values and dead
			else
				UpdateValues(&listArray[i], value, multiply);		// update values and dead
		}
	}

	// create solution lists for gametree in end game
	if (saveSolutions)
	{
		// first set all exposed[][] entries for CountExposedMines()
		// we will reset these later in RestoreExposed()
		for (int i = listStart[list]; i <= listEnd[list]; i++)
		{
			if (listArray[i].tried > 0)
			{
				exposed[listArray[i].y][listArray[i].x] = EXPOSE_MINE;
				if (listArray[i].numNeighbors == 0)
					nonPerimeterMines++;						// count mines in all solutions for non-perimeter area
			}
			else
				exposed[listArray[i].y][listArray[i].x] = 0;
		}
		if (gametreeLinksNext != 0)								// point previous entry to us
			gametreeLinks[gametreeLinksNext - 1].next = &gametreeLinks[gametreeLinksNext];
		gametreeLinks[gametreeLinksNext].next = NULL;
		gametreeLinks[gametreeLinksNext++].index = solutionValuesNext;
		if (gametreeLinksNeeded < gametreeLinksNext)			// record max links usage
			gametreeLinksNeeded = gametreeLinksNext;
		for (int i = listStart[list]; i <= listEnd[list]; i++)
		{
			if (solutionValuesNext == MAX_SOLUTION_VALUES)		// we ran out of entries (should never happen)
				break;
			if (listArray[i].tried > 0)
			{
				solutionValues[solutionValuesNext++] = -1;
				if (saveGameTreeResults > 2
				&&  gametreeResults != NULL)
				{
					fprintf(gametreeResults, ".");
				}
			}
			else
			{
				int count = CountExposedMines(listArray[i].x, listArray[i].y);
				if (saveGameTreeResults > 2
				&& gametreeResults != NULL)
					fprintf(gametreeResults, "%d", count);
				int entry = i - listStart[list];
				solutions[entry].total++;
				solutions[entry].solutions[count].index++;		// count each value
				solutionLinks[solutionLinksNext].index = solutionValuesNext - entry;
				solutionLinks[solutionLinksNext].next = solutions[entry].solutions[count].next;
				solutions[entry].solutions[count].next = &solutionLinks[solutionLinksNext];
				int x = listArray[solutions[entry].index].x;
				int y = listArray[solutions[entry].index].y;
				if (mines[y][x] == count)				// if this is basicaly a zero count
					solutions[entry].min = solutions[entry].solutions[count].index;
				else if (solutions[entry].max < solutions[entry].solutions[count].index)
					solutions[entry].max = solutions[entry].solutions[count].index;
				solutionLinksNext++;
				solutionValues[solutionValuesNext++] = count;
			}
		}
		if (saveGameTreeResults > 2
		&& gametreeResults != NULL)
			fprintf(gametreeResults, "\n");
	}
	minesUsed -= minesUsedAdjust;
}


void CreateSolutions(int list, int index, int maxCount, bool saveSolutions)
{
	for (; index <= listEnd[list]; index++)
	{
		if (listArray[index].tried >= 0							// already filled this entry
		|| listArray[index].numSimilar != 0)					// ignore these for now
			continue;
		if (saveSolutions == false								// not in end play
		|| listEnd[list] - index + minesUsed >= maxCount)		// we can reach our goal of mines
			TryExpose(list, index, maxCount, saveSolutions);	// try expose here 
		if (minesUsed < maxCount)								// no more mines allowed
			TryMine(list, index, maxCount, saveSolutions);		// try a mine here 
		return;
	}
	CountCompletedSolutions(list, maxCount, saveSolutions);
}


int CheckExpose(int list, int index, int max)
{
	int count = 0;
	listArray[index].tried = 0;									// expose this
	for (int n = 0; n < listArray[index].numNeighbors; n++)		// reflect that in our neighbors
	{
		listArray[index].neighbors[n]->unexposed--;
	}
	if (Conflict(&listArray[index], mineCount) == false)
	{
		count = CheckSolutions(list, index + 1, max);			// continue until we are done or abort 
	}
	listArray[index].tried = -1;
	for (int n = 0; n < listArray[index].numNeighbors; n++)		// undo expose
	{
		listArray[index].neighbors[n]->unexposed++;
	}
	return count;
}


int CheckMine(int list, int index, int max)
{
	int count = 0;
	listArray[index].tried = 1;							// place mine
	minesUsed++;
	for (int n = 0; n < listArray[index].numNeighbors; n++)		// reflect that in our neighbors
	{
		listArray[index].neighbors[n]->needed--;
		listArray[index].neighbors[n]->unexposed--;
	}
	if (Conflict(&listArray[index], mineCount) == false)	// we have no conflict so far
	{
		count = CheckSolutions(list, index + 1, max);	// continue until we are done or abort
	}
	listArray[index].tried = -1;
	minesUsed--;
	for (int n = 0; n < listArray[index].numNeighbors; n++)		// undo mine
	{
		listArray[index].neighbors[n]->needed++;
		listArray[index].neighbors[n]->unexposed++;
	}
	return count;
}


int CheckSolutions(int list, int index, int max)
{
	int count = 0;
	if (index <= listEnd[list])
	{
		count += CheckMine(list, index, max);		// try a mine here 
		if (count >= max)
			return count;
		count += CheckExpose(list, index, max);	// try expose here
		return count;
	}
	else
		return 1;										// we have a solution 
}


int GetSolutionCount()
{
	int sum = 0;
	for (int i = 0; i < lists; i++)
	{
		for (int n = 0; n <= listMaxMines[i] - listMinMines[i]; n++)
			sum += listSolutions[i][n];
	}
	return sum;
}


void UpdatePerimeterStats()
{
	int sum = GetSolutionCount();
	if (maxListSolutions < sum)
		maxListSolutions = sum;
}


//	Create all possible solutions for each list
//	We do this by adding a mine in the next position
//	and if this causes a conflict return false 
//	and if this exposes an entry then see if this causes a conflist too
//	If there are no conflicts, repeat the process on the next unused entry on the list
//	When done mark those list entries with a mine and update the count of solutions
//	Repeat for all lists
int CreatePerimeterSolutions(int start, int end, int maxMines, bool saveSolutions)
{
	completedSolutions = 0;
	conflictsInSolutions = 0;
	if (end < 0)
		end = lists - 1;
	for (int list = start; list <= end; list++)
	{
		for (int j = 0; j < MAX_COUNTS; j++)
			listSolutions[list][j] = 0;
		listMaxMines[list] = 0;
		listMinMines[list] = MAX_MINES;
		int len = listEnd[list] - listStart[list] + 1;
		if (maxSinglePerimeter < len)
			maxSinglePerimeter = len;
		if (len > WARNING_SEQUENCE)						// warning about very long lists
		{
			if (testAllResults != NULL)
			{
				if (saveInteresting)
				{
					char testFileName[MAX_PATH];
					sprintf(testFileName, "%s\\veryLongSequence seed=0x%llx.txt", prefixString, saveSeed);
					SavePuzzle(testFileName);
				}
				if (testResults != NULL)
					fprintf(testResults, "\n***** INTERESTING: Puzzle=%d Very long sequence (len=%d)\n", testing, len);
			}
		}
		minesUsed = 0;
		solutionValuesNext = 0;
		gametreeLinksNext = 0;
		solutionLinksNext = 0;
		for (int i = listStart[list]; i <= listEnd[list]; i++)
		{
			listArray[i].minValue = MAX_NEIGHBORS;
			listArray[i].maxValue = 0;
		}
		int max = maxMines - (end - list);
		CreateSolutions(list, listStart[list], max, saveSolutions);	// create solutions
		RestoreExposed(list);							// restore entries used for game tree
		maxMines -= listMinMines[list];					// mines remaining to be used
	}
	if (maxCompletedSolutions < completedSolutions)
		maxCompletedSolutions = completedSolutions;
	if (completedSolutions > WARNING_SOLUTIONS)			// we would like to know if we computed many solutions
	{
		if (testAllResults != NULL)
		{
			if (saveInteresting)
			{
				char testFileName[MAX_PATH];
				sprintf(testFileName, "%s\\ManySolutions seed=0x%llx.txt", prefixString, saveSeed);
				SavePuzzle(testFileName);
			}
			if (testResults != NULL)
				fprintf(testResults, "\n***** INTERESTING: Puzzle=%d ManySolutions (len=%d)\n", testing, completedSolutions);
		}
	}

	if (saveSolutions)
		return completedSolutions;
	// mark dead lists
	for (int i = listStart[start]; i <= listEnd[end]; i++)
	{
		if (listArray[i].dead == false)						// not dead
		{
			listDead[listArray[i].list] = false;
		}
	}
	// reduce the max range to a value that keeps the limits right
	// this means the max of one plus the min of the others <= mineCount
	// NOTE: this is quite correct since this effects the perimeter and 
	// the non-perimeter areas
	int max = 0;
	int min = 0;
	for (int i = 0; i < lists; i++)
	{
		min += listMinMines[i];
		max += listMaxMines[i];
	}
	if (max > mineCount)
	{
		int play = mineCount - min;						// max amount of play in our solutions
		max = mineCount;
		for (int i = 0; i < lists; i++)
		{
			if (listMaxMines[i] - listMinMines[i] > play)
			{
				listMaxMines[i] = listMinMines[i] + play;
				// no need to redo listArray[i].counts[] because the used ones will be ignored
				// no game tree (not in end play) so need to redo that either
			}
		}
	}
	return completedSolutions;
}


double combResults[MAX_WIDTH * MAX_HEIGHT / 2];			// save results so we can look them up again
int	combN = -1;											// last value of n used
int	combM = 0;											// last entry set
double Comb(int n, int m)
{
	if (m > n - m)										// use the smallest term for m
		m = n - m;
	if (m < 0)											// illegal term
		return 0.0;
	if (n != combN)										// new value for our combination array
	{
		combResults[0] = 1.0;							// Comb(n,0) = 1.0 always
		combM = 0;										// last entry set
		combN = n;										// new n
	}
	if (m > combM)										// we need to create newer terms
	{
		for (int i = combM + 1; i <= m; i++)
		{
			combResults[i] = combResults[i - 1] * (n + 1 - i) / i;
		}
		combM = m;
	}
	return combResults[m];
}


double CountSolutions(int list, int mines, int notList)
{
	if (list >= lists)
		return Comb(nonPerimeter, mines);
	if (notList == list)
		return CountSolutions(list + 1, mines, notList);
	double solutions = 0;
	for (int mi = 0; mi <= listMaxMines[list] - listMinMines[list]; mi++)
	{
		solutions += listSolutions[list][mi] * CountSolutions(list + 1, mines - listMinMines[list] - mi, notList);
	}
	return solutions;
}


double CountWeightedSolutions(int list, int mines)
{
	if (list >= lists)
		return mines * Comb(nonPerimeter, mines);
	double solutions = 0;
	for (int mi = 0; mi <= listMaxMines[list] - listMinMines[list]; mi++)
	{
		solutions += listSolutions[list][mi] * CountWeightedSolutions(list + 1, mines - listMinMines[list] - mi);
	}
	return solutions;
}


void CalculateProb()
{
	totalSolutions = 0.0;
	weightedSolutions = 0.0;
	if (lists == 0)										// no perimeter only open area
	{
		totalSolutions = Comb(nonPerimeter, mineCount);
		nonPerimeterProb = (double)mineCount / nonPerimeter;
	}
	else
	{
		for (int mi = 0; mi <= listMaxMines[0] - listMinMines[0]; mi++)
		{
			totalSolutions += listSolutions[0][mi] * CountSolutions(1, mineCount - listMinMines[0] - mi, -1);
			weightedSolutions += listSolutions[0][mi] * CountWeightedSolutions(1, mineCount - listMinMines[0] - mi);
		}
		if (nonPerimeter != 0)
			nonPerimeterProb = weightedSolutions / (totalSolutions * nonPerimeter);
		else
			nonPerimeterProb = 1.0;
	}
	int list = 0;
	bestProb = 1.0;
	for (int i = 0; i < listNext; i++)
	{
		double prob = 0;								// weighted prob for current location
		if (listArray[i].dead)
			deadCount++;
		for (int mi = 0; mi <= listMaxMines[list] - listMinMines[list]; mi++)
		{
			prob += listArray[i].counts[mi] * CountSolutions(0, mineCount - listMinMines[list] - mi, list);
		}
		listArray[i].prob = prob / totalSolutions;
		if (listArray[i].dead == false					// do not count dead entries for best
		&&	GreaterThan(bestProb, listArray[i].prob))
		{
			bestProb = listArray[i].prob;				// best prob
		}
		if (listArray[i].prob > 1.0000001)
		{
			if (testAllResults != NULL)
				fprintf(testAllResults, "\n***** ERROR: Puzzle=%d prob=%f > 1.0\n", testing, listArray[i].prob);
			listArray[i].prob = 1.0;					// this is an ERROR
		}
		if (i == listEnd[list])
			list++;										// use next list
	}	
}


bool FindEasySolution()
{
	if (listNext < remainingCount + mineCount)			// some non-perimeter entries exist
	{
		if (Equal(nonPerimeterProb, 0.0))				// it is all open
		{
			SaveBackUp();								// save current board
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (exposed[y][x] <	0				// a free entry
					&&  listUsed[y][x] < 0)
						Expose(x, y);					// one try will do it
				}
			}
			return true;
		}
		if (Equal(nonPerimeterProb, 1.0))				// it is all all mines
		{
			SaveBackUp();								// save current board
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					if (exposed[y][x] <	0				// a free entry
					&& listUsed[y][x] < 0)
						AddMine(x, y);
				}
			}
			return true;
		}
	}
	for (int i = 0; i < listNext; i++)
	{
		if (Equal(listArray[i].prob, 0.0))
		{
			SaveBackUp();								// save current board
			Expose(listArray[i].x, listArray[i].y);
			return true;
		}
		if (Equal(listArray[i].prob, 1.0))
		{
			SaveBackUp();								// save current board
			AddMine(listArray[i].x, listArray[i].y);
			return true;
		}
	}
	return false;
}


int CompareGameTreeList(const void * e1, const void * e2)
{
	Solutions * s1 = (Solutions *)e1;
	Solutions * s2 = (Solutions *)e2;
	if (s1->dead 
	&&  s2->dead)
		goto ORDER;
	if (s1->dead)										// dead entries last
		return 1;
	if (s2->dead)
		return -1;
	if (s1->total < s2->total)							// sort by largest first (best prob for pruning)
		return 1;
	if (s1->total > s2->total)
		return -1;
	if (s1->min < s2->min)								// largest min is best
		return 1;
	if (s1->min > s2->min)
		return -1;
	if (s1->max > s2->max)								// lowest max count in solutions reduces search depth & time 
		return 1;
	if (s1->max < s2->max)
		return -1;
ORDER:
	if (s1->order < s2->order)							// to stablize qsort
		return -1;
	return 1;
}


int CompareGameTreeList2(const void * e1, const void * e2)
{
	Solutions * s1 = (Solutions *)e1;
	Solutions * s2 = (Solutions *)e2;
	if (s1->total < s2->total)							// sort by largest first (best prob of success)
		return 1;
	if (s1->total > s2->total)
		return -1;
	if (s1->max > s2->max)								// lowest max count in solutions is best
		return 1;
	if (s1->max < s2->max)
		return -1;
	if (s1->order < s2->order)							// to stablize qsort
		return -1;
	return 1;
}


int gametreeNodes;
Solutions newSolutions[MAX_DEPTH][END_PLAY_COUNT];		// building new solution lists and links
Link	newSolutionLinks[MAX_DEPTH][END_PLAY_COUNT * MAX_SOLUTIONS];
int		newSolutionLinksNext[MAX_DEPTH];

int GameTreeProcess(int depth, Solutions * solutions, int startIndex, int solutionCount, int allSolutions, int & best, int bestWinCount, char * string);

// if there is a move that is 100% safe use that and do not any other moves
// if there is a move with max = 1 use that and do not use any other moves
// if we get a prob of 100% do not use any other moves unless survival prob is better
//
int GameTreeProb(int depth, int n, Solutions * solutions, int startIndex, int solutionCount, int allSolutions, bool & skip, int bestWinCount, char * string)
{
	char newString[END_PLAY_COUNT + 1];
	gametreeNodes++;									// count nodes processed
	if (gametreeMaxDepth < depth)
		gametreeMaxDepth = depth;
	int totalRemaining = solutions[n].total;
	int wins;
	int currentWins = 0;
	skip = false;
	for (int count = 0; totalRemaining > 0; count++)
	{
		int numSolutions = solutions[n].solutions[count].index;		// number of solutions
		if (numSolutions == 0)										// nothing to process
			continue;
		int listIndex = solutions[n].index;
		// pruning code
		if (bestWinCount >= currentWins + totalRemaining)
		{
			if (saveGameTreeResults > 2
			&& gametreeResults != NULL)
			{
				for (int i = 1; i < depth; i++)
					fprintf(gametreeResults, "--");
				fprintf(gametreeResults, "skip bestWinCount=%d >= wins=%d + remaining=%.d\n",
					bestWinCount, currentWins, totalRemaining);
			}
			skip = true;
			return 0;
		}
		if (saveGameTreeResults > 2
		&& gametreeResults != NULL)
		{
			for (int i = 1; i < depth; i++)
				fprintf(gametreeResults, "--");
			fprintf(gametreeResults, "%d,%d->%d solutions=%d\n",
				listArray[listIndex].x, listArray[listIndex].y, count, numSolutions);
		}
		if (numSolutions == 1)								// we can always solve this
			wins = 1;
		else
		{
			int key = -1;
			int best2;
			int d = depth - 1;
			strcpy(newString, string);
			newString[listIndex - startIndex] = '0' + count;				// modify search string
			if (d < MAX_DEPTH)								// build new solutions list
			{
				newSolutionLinksNext[d] = 0;				// next available entry in newSolutionLinks[]
				for (int n = 0; n < solutionCount; n++)
				{
					newSolutions[d][n].index = solutions[n].index;
					newSolutions[d][n].total = 0;
					newSolutions[d][n].max = 0;
					newSolutions[d][n].min = 0;
					newSolutions[d][n].order = solutions[n].order;
					newSolutions[d][n].dead = solutions[n].dead;
					for (int m = 0; m <= MAX_NEIGHBORS; m++)
					{
						newSolutions[d][n].solutions[m].index = 0;
						newSolutions[d][n].solutions[m].next = NULL;
					}
				}
				Link * cur = solutions[n].solutions[count].next;
				while (cur != NULL)
				{
					// add cur solution to entry for values of index except the current one
					for (int nn = 0; nn < solutionCount; nn++)
					{
						if (solutions[nn].index == listIndex)	// skip current entry
							continue;
						int value = solutionValues[cur->index + solutions[nn].index];
						if (value >= 0)							// not a mine
						{
							newSolutionLinks[d][newSolutionLinksNext[d]].index = cur->index;
							newSolutionLinks[d][newSolutionLinksNext[d]].next = newSolutions[d][nn].solutions[value].next;
							newSolutions[d][nn].solutions[value].next = &newSolutionLinks[d][newSolutionLinksNext[d]];
							newSolutions[d][nn].solutions[value].index++;
							if (newSolutions[d][nn].max < newSolutions[d][nn].solutions[value].index)
								newSolutions[d][nn].max = newSolutions[d][nn].solutions[value].index;
							newSolutions[d][nn].total++;		// one more solution
							newSolutionLinksNext[d]++;
						}
					}
					cur = cur->next;
				}
				int newSolutionCount = 0;
				// just compress the list removing unnecessary entries
				for (int n = 0; n < solutionCount; n++)
				{
					if (newSolutions[d][n].total == 0)			// this must be a mine
						continue;
					// if max == number of solutions then the value is constant
					if (newSolutions[d][n].max == numSolutions)	
					{
						int index = newSolutions[d][n].index;
						int first = 0;
						while (newSolutions[d][n].solutions[first].index == 0)
							first++;
						newString[index - startIndex] = '0' + first;	// set all these in the hash string
						continue;								// do not look at this entry either
					}
					// if allSolutions == total and max == 1 then we can always win
					if (newSolutions[d][n].total == numSolutions	// only choice
					&&  newSolutions[d][n].max == 1)
					{
						wins = numSolutions;
						best2 = newSolutions[d][n].index;
						goto WIN;
					}
					if (n != newSolutionCount)
						newSolutions[d][newSolutionCount] = newSolutions[d][n];
					newSolutionCount++;
				}
				if (numSolutions == 2							// always 50% unless max == 1
				&& newSolutions[d][0].max > 1)
				{
					wins = 1;
					best2 = newSolutions[d][n].index;
					goto WIN;
				}
				if (HashGet(key, newString, best2, wins))		// hash entry found
				{
					if (saveGameTreeResults > 2
					&& gametreeResults != NULL)
					{
						fprintf(gametreeResults, "Hash Table hit: wins=%d\n", wins);
					}
				}
				else if (newSolutionCount == 0)
				{
					// NOTE: not sure why this works but when we do not use dead entries
					// we can get 0 solutions 
					wins = 1;
#if 0
					if (testResults != NULL)
					{
						fprintf(testResults, "***** WARNING: Puzzle=%d solutionCount==0!\n", testing);
						char testFileName[MAX_PATH];
						sprintf(testFileName, "%s\\solutionCount=0 seed=0x%llx.txt", prefixString, saveSeed);
						SavePuzzle(testFileName);
					}
#endif
				}
				else
				{
					// this is necessary because the code in GameTreeProcess() expects the highest count first for pruning
					// although it does use more time despite the reduction in nodes searched
					qsort(newSolutions[d], newSolutionCount, sizeof(Solutions), CompareGameTreeList2);
					wins = GameTreeProcess(depth + 1, newSolutions[d], startIndex, newSolutionCount, numSolutions, best2, bestWinCount - currentWins - remainingCount, newString);
					if (wins != numSolutions					// do not add pruned entries and 100% wins
					&&  wins != 0)
						HashPut(key, newString, best2, wins);	// enter new hash table data
				}
			}
			else
			{
				if (testAllResults != NULL)
				{
					char testFileName[MAX_PATH];
					sprintf(testFileName, "%s\\DepthExceeded seed=0x%llx.txt", prefixString, saveSeed);
					SavePuzzle(testFileName);
					fprintf(testAllResults, "***** ERROR: Puzzle=%d gametree depth exceeds %d\n", testing, MAX_DEPTH);
				}
				wins = numSolutions;							// assume we can win all the remaining
			}
		}
WIN:	currentWins += wins;
		totalRemaining -= numSolutions;							// remaining solutions
		if (saveGameTreeResults > 2
		&& gametreeResults != NULL)
		{
			for (int i = 1; i < depth; i++)
				fprintf(gametreeResults, "--");
			fprintf(gametreeResults, "%d,%d->%d add=%d wins=%d nodes=%d\n",
				listArray[listIndex].x, listArray[listIndex].y, count, wins, currentWins, gametreeNodes);
		}
	}
	return currentWins;
}


int GameTreeProcess(int depth, Solutions * solutions, int startIndex, int solutionCount, int allSolutions, int & best, int bestWinCount, char * string)
{
	int gametreeWins = bestWinCount = 0;
	int start = 0;
	int end = solutionCount;
	// if allSolutions == total for any entry just use that entry and no others
	for (int n = 0; n < solutionCount; n++)			// try each possible entry
	{
		if (solutions[n].total == allSolutions		// this uses all solutions
		&&  solutions[n].max != allSolutions)		// we cannot reduce anything in this case
		{
			start = n;
			end = start + 1;
			break;
		}
	}
	for (int n = start; n < end; n++)				// try each possible entry if necessary	
	{
		int maxWins = solutions[n].total;
		int i = solutions[n].index;
		if (maxWins <= bestWinCount)				// we can never do better
		{
			if (saveGameTreeResults == 2
			&& gametreeResults != NULL
			&& depth == 1)
			{
				fprintf(gametreeResults, "play=%d,%d skipped nodes=%d\n",
					listArray[i].x, listArray[i].y, gametreeNodes);
			}
			continue;
		}
		if (saveGameTreeResults > 2
		&& gametreeResults != NULL)
		{
			if (depth == 1)
				fprintf(gametreeResults, "############################################# nodes=%d\n", gametreeNodes);
			for (int i = 1; i < depth; i++)
				fprintf(gametreeResults, "--");
			fprintf(gametreeResults, "trying=%d,%d solutions=%d %s\n",
				listArray[i].x, listArray[i].y, solutions[n].total,
				(start + 1 == end ? "One Choice" : ""));
		}
		bool skip;
		int wins = GameTreeProb(depth, n, solutions, startIndex, solutionCount, allSolutions, skip, bestWinCount, string);
		if (automatic && depth == 1)
		{
			if (wins > bestWinCount)
				gameTreeNext = 0;
			if (wins >= bestWinCount)
			{
				GameTreeArray[gameTreeNext].x = listArray[i].x;
				GameTreeArray[gameTreeNext].y = listArray[i].y;
				gameTreeNext++;
			}
		}
		if (saveGameTreeResults == 2
		&& gametreeResults != NULL
		&& depth == 1)
		{
			if (skip)
				fprintf(gametreeResults, "play=%d,%d pruned nodes=%d\n",
					listArray[i].x, listArray[i].y, gametreeNodes);
			else
				fprintf(gametreeResults, "play=%d,%d solutions=%d wins=%d (%.2f%%) nodes=%d\n",
					listArray[i].x, listArray[i].y, solutions[n].total, wins, 100.0 * wins / allSolutions, gametreeNodes);
		}
		if (skip)										// this entry will not help 
			continue;
		if (saveGameTreeResults > 2
		&& gametreeResults != NULL)
		{
			for (int i = 1; i < depth; i++)
				fprintf(gametreeResults, "--");
			fprintf(gametreeResults, "tried=%d,%d wins=%d %s\n",
				listArray[i].x, listArray[i].y, wins,
				(start + 1 == end ? "One Choice" : ""));
		}
		if (gametreeWins < wins)						// best result so far
		{
			gametreeWins = wins;
			best = i;
			bestWinCount = wins;
			if (wins == allSolutions)					// we have a 100% solution so no need to check anything else									
				break;		
		}
	}
	return gametreeWins;
}


bool KnownProb(int x, int y)
{
	// check for linked triples
	//	X2X	X1X
	//	---	---
	//	 ?	 ?
	// or doubles
	//	X2X	X1X
	//	-X-	-X-
	//	 ?	 ?
	int mask = adjacentMask[y][x];
	if (x > 1
	&& MaskCount[mask & MASK_LEFT] == unexposed[y][x - 2] 
	&& exposed[y][x - 2] >= 0
	&& exposed[y][x - 2] < EXPOSE_MINE)
		mask &= ~MASK_LEFT;
	if (x < width - 2
	&& MaskCount[mask & MASK_RIGHT] == unexposed[y][x + 2] 
	&& exposed[y][x + 2] >= 0
	&& exposed[y][x + 2] < EXPOSE_MINE)
		mask &= ~MASK_RIGHT;
	if (y > 1
	&& MaskCount[mask & MASK_UPPER] == unexposed[y - 2][x]
	&& exposed[y - 2][x] >= 0
	&& exposed[y - 2][x] < EXPOSE_MINE)
		mask &= ~MASK_UPPER;
	if (y < height - 2
	&& MaskCount[mask & MASK_LOWER] == unexposed[y + 2][x]
	&& exposed[y + 2][x] >= 0
	&& exposed[y + 2][x] < EXPOSE_MINE)
		mask &= ~MASK_LOWER;
	// check for linked pairs
	//	|2M		|M2X	X2M|	X2X	
	//	|--		|--X	X--|	-M-
	//	|?P		|?P 	 P?|	 ? 
	if (x > 1
	&&  MaskCount[mask & MASK_LEFT] == 2)
	{
		if (exposed[y][x - 1] == EXPOSE_MINE
		&&  needed[y][x - 2] == 1)
			mask &= ~MASK_LEFT;
		else if (exposed[y][x - 1] < 0)
		{
			List * list = &listArray[listUsed[y][x - 1]];
			for (int n = 0; n < list->numNeighbors; n++)
			{
				if (list->neighbors[n]->count == 2
				&&  list->neighbors[n]->needed == 1)
				{
					mask &= ~MASK_LEFT;
					break;
				}
			}
		}
	}
	if (x < width - 2
	&& MaskCount[mask & MASK_RIGHT] == 2)
	{
		if (exposed[y][x + 1] == EXPOSE_MINE
		&&  needed[y][x + 2] == 1)
			mask &= ~MASK_RIGHT;
		else if (exposed[y][x + 1] < 0)
		{
			List * list = &listArray[listUsed[y][x + 1]];
			for (int n = 0; n < list->numNeighbors; n++)
			{
				if (list->neighbors[n]->count == 2
				&&  list->neighbors[n]->needed == 1)
				{
					mask &= ~MASK_RIGHT;
					break;
				}
			}
		}
	}
	if (y > 1
	&& MaskCount[mask & MASK_UPPER] == 2)
	{
		if (exposed[y - 1][x] == EXPOSE_MINE
		&&  needed[y - 2][x] == 1)
			mask &= ~MASK_UPPER;
		else if (exposed[y - 1][x] < 0)
		{
			List * list = &listArray[listUsed[y - 1][x]];
			for (int n = 0; n < list->numNeighbors; n++)
			{
				if (list->neighbors[n]->count == 2
				&& list->neighbors[n]->needed == 1)
				{
					mask &= ~MASK_UPPER;
					break;
				}
			}
		}
	}
	if (y < height - 2
	&& MaskCount[mask & MASK_LOWER] == 2)
	{
		if (exposed[y + 1][x] == EXPOSE_MINE
		&&  needed[y + 2][x] == 1)
			mask &= ~MASK_LOWER;
		else if (exposed[y + 1][x] < 0)
		{
			List * list = &listArray[listUsed[y + 1][x]];
			for (int n = 0; n < list->numNeighbors; n++)
			{
				if (list->neighbors[n]->count == 2
				&&	list->neighbors[n]->needed == 1)
				{
					mask &= ~MASK_LOWER;
					break;
				}
			}
		}
	}
	return mask == 0;
}


double openProb[MAX_NEIGHBORS + 2];							// probability of getting n free areas with 0 mines

void AddSuperLocEntry(double & bestSuperLocProb, int x, int y)
{
	if (exposed[y][x] >= 0						// exposed
	|| listUsed[y][x] >= 0						// on the perimeter
	|| unexposed[y][x] == 0)					// ignore single entries
		return;
	int count = unexposed[y][x] - adjacentCount[y][x];
	double prob = openProb[count + 1];
	if (GreaterThan(bestSuperLocProb, prob))	// no chance the perimeter will help
		return;
	double exposed = 1.0 - nonPerimeterProb + count * prob;
	if (adjacentCount[y][x] != 0)				// adjacent to perimeter
	{
		if (KnownProb(x, y))					// see if we know the prob
		{
			if (GreaterThan(bestSuperLocProb, prob))
				return;
			goto PRUNE;
		}
		exposed = 0.0;							// save for later evaluation
		if (LessThan(bestSuperLocProb, prob))
			goto ADD;
		return;									// we cannot beat the best
	}
	else if (count >= 5)						// take only one entry along the edge
	{
		if (Equal(bestSuperLocProb, prob))
			return;
	}
PRUNE:			
	bestSuperLocProb = prob;
	int end = superNext;
	superNext = 0;								// remove worse entries
	for (int i = 0; i < end; i++)
	{
		if (LessThan(SuperLocArray[i].progress, prob))
			continue;
		if (Equal(SuperLocArray[i].progress, prob)
			&& SuperLocArray[i].exposed == 0.0)		// remove adjacents with no better progress
			continue;
		if (superNext != i)
			SuperLocArray[superNext] = SuperLocArray[i];
		superNext++;
	}
ADD:	
	if (superNext < MAX_SUPERLOCATIONS)			// do not overflow the list
	{
		SuperLocArray[superNext].x = x;
		SuperLocArray[superNext].y = y;
		SuperLocArray[superNext].list = NULL;
		SuperLocArray[superNext].numLinks = 0;
		SuperLocArray[superNext].numEdges = 0;
		SuperLocArray[superNext].progress = prob;
		SuperLocArray[superNext].exposed = exposed;
		superNext++;
	}
}

void FindSuperLocations()
{
	double threshold;
	if (LessThan(bestProb, nonPerimeterProb))				// use only the edges
		threshold = 1.0 - (guessThreshold * (1.0 - bestProb));
	else 
		threshold = 1.0 - (guessThreshold * (1.0 - nonPerimeterProb));
	openProb[0] = 1.0;
	double solutions;
	for (int i = 1; i <= MAX_NEIGHBORS + 1; i++)			// calculate probability for n open places
	{
		nonPerimeter -= i;
		solutions = CountSolutions(0, mineCount, -1);
		nonPerimeter += i;
		openProb[i] = solutions / totalSolutions;
	}
	superNext = 0;											// next entry in SuperLocArray
	double bestSuperLocProb = 0.0;
	if (GreaterThan(nonPerimeterProb, threshold) == false)	// off perimeter is fine
	{
		if (topToBottomPriority)
		{
			for (int x = 0; x < width; x++)
			{
				for (int y = 0; y < height; y++)
				{
					AddSuperLocEntry(bestSuperLocProb, x, y);	// add new entry if possible
				}
			}
		}
		else
		{
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					AddSuperLocEntry(bestSuperLocProb, x, y);	// add new entry if possible
				}
			}
		}
	}
	if (superNext == 0										// no solutions in the open area
	&& GreaterThan(bestProb, threshold))					// and we would look at the perimeter
	{
		threshold = 1.0 - (guessThreshold * (1.0 - bestProb));
	}
	if (GreaterThan(bestProb, threshold) == false)			// perimeter is fine
	{
		for (int i = 0; i < listNext; i++)
		{
			if (GreaterThan(listArray[i].prob, threshold))
				continue;
			if (listArray[i].dead)
				continue;									// ignore dead entries as well
			if (superNext >= MAX_SUPERLOCATIONS)			// do not overflow the list
			{
				if (testAllResults != NULL)
				{
					char testFileName[MAX_PATH];
					sprintf(testFileName, "%s\\SuperLocationOverflow seed=0x%llx.txt", prefixString, saveSeed);
					SavePuzzle(testFileName);
					int sum = GetSolutionCount();
					fprintf(testAllResults, "\n***** ERROR: Puzzle=%d SuperLocationOverflow more than %d super locations!\n",
						testing, superNext);
				}
				break;
			}
			int x = listArray[i].x;
			int y = listArray[i].y;
			int val = listArray[i].minValue;
			SuperLocArray[superNext].x = x;
			SuperLocArray[superNext].y = y;
			SuperLocArray[superNext].numLinks = 0;
			SuperLocArray[superNext].numEdges = 0;
			SuperLocArray[superNext].list = &listArray[i];
			SuperLocArray[superNext].progress = 0.0;
			SuperLocArray[superNext].exposed = 0.0;
			superNext++;
		}
	}
}


int CompareSuperLoc(const void * e1, const void * e2)
{
	SuperLoc * s1 = (SuperLoc *)e1;
	SuperLoc * s2 = (SuperLoc *)e2;
	if (GreaterThan(s2->progress, s1->progress))				// largest progress is best
		return 1;
	if (GreaterThan(s1->progress,s2->progress))
		return -1;
	if (GreaterThan(s2->exposed, s1->exposed))					// largest clears is best
		return 1;
	if (GreaterThan(s1->exposed, s2->exposed))
		return -1;
#if 0	// this seems to make no difference
	if (s1->list != NULL  && s2->list == NULL)					// pick perimeter first
		return -1;
	if (s1->list == NULL  && s2->list != NULL)
		return 1;
	if (adjacentCount[s1->y][s1->x] != 0 && adjacentCount[s2->y][s2->x] == 0)
		return -1;												// adjacent over open
	if (adjacentCount[s1->y][s1->x] == 0 && adjacentCount[s2->y][s2->x] != 0)
		return 1;										
#endif
	if (s1->order < s2->order)									// first is best
		return -1;
	return 1;
}


SuperLoc * SuperLocations()
{
	// we may have nothing but single open entries
	if (superNext == 0)
		return NULL;
	// only one solution so exit
	if (superNext == 1)
	{
		bestSuperLocProgress = SuperLocArray[0].progress;
		bestSuperLoc = &SuperLocArray[0];
		return bestSuperLoc;
	}
	if (superLocMaxCandidates < superNext)
		superLocMaxCandidates = superNext;
	if (saveSuperLocResults > 1										// show perimeter mask
	&& superLocResults != NULL)
	{
		U64 mask0, mask1;
		if (listNext < 64)
		{
			mask0 = ((U64)1 << listNext) - 1;
			mask1 = 0;
		}
		else
		{
			mask0 = 0xffffffffffffffff;
			mask1 = ((U64)1 << (listNext - 64)) - 1;
		}
		fprintf(superLocResults, "Perimeter mask=0x%016llX 0x%016llX\n", mask1, mask0);
	}
	for (int n = 0; n < superNext; n++)
	{
		List * list = SuperLocArray[n].list;
		if (list == NULL											// open area
		&&  SuperLocArray[n].exposed != 0.0)						// already calculated progress and clears
			continue;
		TimeStart2();
		FastCalculateSolutions(&SuperLocArray[n]);
		SuperLocArray[n].time = TimeEnd2();
	}
	bestSuperLocProgress = -1.0;									// in case all superloc locations have no clears
	double bestSuperLocExposed = 0;
	for (int n = 0; n < superNext; n++)
	{
		SuperLocArray[n].order = n;									// this is to stablize the qsort
		if (SuperLocArray[n].list != NULL)
			SuperLocArray[n].prob = 1.0 - SuperLocArray[n].list->prob;
		else
			SuperLocArray[n].prob = 1.0 - nonPerimeterProb;
		if (SuperLocArray[n].numLinks != 0)							// get clears for linked entries
			FastCalculateLinkedClears(&SuperLocArray[n]);
		if (GreaterThan(SuperLocArray[n].progress, bestSuperLocProgress)
		|| (Equal(SuperLocArray[n].progress, bestSuperLocProgress) && GreaterThan(SuperLocArray[n].exposed, bestSuperLocExposed)))
		{
			bestSuperLoc = &SuperLocArray[n];
			bestSuperLocProgress = bestSuperLoc->progress;
			bestSuperLocExposed = bestSuperLoc->exposed;
		}
	}
	bestSuperLoc->order = -1;										// this entry first
	if (saveSuperLocResults > 0										// list final result
	&& superLocResults != NULL)
	{
		int x = bestSuperLoc->x;
		int y = bestSuperLoc->y;
		// sort entries
		qsort(SuperLocArray, superNext, sizeof(SuperLoc), CompareSuperLoc);
		for (int n = 0; n < superNext; n++)
		{
			char string[72];
			char * linked = "";
			if (SuperLocArray[n].list != NULL)
			{
				if (SuperLocArray[n].numLinks > 0)
				{
					char string2[12];
					strcpy(string, "Linked to ");
					for (int nn = 0; nn < SuperLocArray[n].numLinks; nn++)
					{
						sprintf(string2, "%d,%d ", SuperLocArray[n].links[nn]->x, SuperLocArray[n].links[nn]->y);
						strncat(string, string2, 72);
					}
					linked = string;
				}
			}
			else
			{
				if (adjacentCount[SuperLocArray[n].y][SuperLocArray[n].x] != 0)
					linked = "Adjacent ";
				else
					linked = "Open ";
			}
			fprintf(superLocResults, "%d,%d\tprob=%.3f%%  progress=%6.3f%% clears=%6.3f time=%.3fms %s%s\n",
				SuperLocArray[n].x, SuperLocArray[n].y, SuperLocArray[n].prob * 100.0,
				SuperLocArray[n].progress * 100.0, SuperLocArray[n].exposed,
				SuperLocArray[n].time * 1000.0, 
				(SuperLocArray[n].list != NULL && SuperLocArray[n].list->merged >= 0 ? "merges " : ""), linked);
		}
		bestSuperLoc = &SuperLocArray[0];
	}
	if (useSuperLocMultiplier)
	{
		SuperLoc * oldBestSuperLoc = bestSuperLoc;
		for (int n = 0; n < superNext; n++)
		{
			if (&SuperLocArray[n] == bestSuperLoc)						// this is already the best
				continue;
			double diff = bestSuperLoc->prob * bestSuperLoc->progress - SuperLocArray[n].prob * SuperLocArray[n].progress;
			if ((diff < 0.0
				&&  GreaterThan(SuperLocArray[n].exposed, bestSuperLocExposed))
			|| GreaterThan(SuperLocArray[n].exposed, bestSuperLocExposed * (1.0 + diff * 100.0 * clearMultiplier)))	// meets our criteria
			{
				bestSuperLoc = &SuperLocArray[n];
				bestSuperLocExposed = bestSuperLoc->exposed;
			}
		}
		// show those cases where we pick something other than the best super location
		if (oldBestSuperLoc != bestSuperLoc)
		{
			superLocMultiplierUsed++;
			double old = oldBestSuperLoc->prob * bestSuperLoc->progress;
			if (GreaterThan(old, bestSuperLoc->prob * bestSuperLoc->progress))
				superLocMultiplyUsed++;
			char * state;
			if (puzzle[oldBestSuperLoc->y][oldBestSuperLoc->x] != 0)
			{
				if (puzzle[bestSuperLoc->y][bestSuperLoc->x] != 0)
					state = "both-failed";
				else
				{
					superLocMultiplierImproved++;
					state = "SUCCEED";
				}
			}
			else
			{
				if (puzzle[bestSuperLoc->y][bestSuperLoc->x] == 0)
					state = "both-work";
				else
				{
					superLocMultiplierFailed++;
					state = "FAILED";
				}
			}
			if (saveSuperLocMultiplierChanges
			&&	testAllResults != NULL)
			{
				char testFileName[MAX_PATH];
				sprintf(testFileName, "%s\\SuperLocMultiplier %s (%.3f%%,%.3f) vs (%.3f%%,%.3f).txt",
				prefixString, state,
				100.0*oldBestSuperLoc->progress, oldBestSuperLoc->exposed,
				100.0*bestSuperLoc->progress, bestSuperLoc->exposed);
				SavePuzzle(testFileName);
			}
			if (saveSuperLocResults > 0								// list final result
			&& superLocResults != NULL)
			{
				double prob = oldBestSuperLoc->prob - bestSuperLoc->prob;
				double diff = oldBestSuperLoc->progress - bestSuperLoc->progress;
				double exposed = bestSuperLoc->exposed - oldBestSuperLoc->exposed;
				fprintf(superLocResults, "%d,%d\tprob=%.3f%%  progress=%6.3f%% clears=%6.3f diff=%.3f%%,%.3f%%,%.3f Threshold selection\n",
					bestSuperLoc->x, bestSuperLoc->y, bestSuperLoc->prob * 100.0,
					bestSuperLoc->progress * 100.0, bestSuperLoc->exposed,
					prob * 100.0, diff * 100.0, exposed);
			}
		}
	}
	return bestSuperLoc;
}


bool UseSuperLoc()
{
	SuperLoc * superEntry;
	FindSuperLocations();										// Find super locations so we can show them in automatic mode
#if SUPERLOC_NEW_METHOD_TEST <= 0								// used to find differences between new/old/jar methods
	SuperLoc * oldSuperEntry = NULL;
	if (testAllResults != NULL)									// in test mode
	{
		useNewSuperLocMethod = SUPERLOC_NEW_METHOD_TEST;		// use alternate method
		oldSuperEntry = SuperLocations();						// see if super locations are found
		if (SUPERLOC_NEW_METHOD_TEST != -2)
			useNewSuperLocMethod = 1;							// use new method
		else
			useNewSuperLocMethod = 0;							// use old method
	}
#endif
	TimeStart();
	superEntry = SuperLocations();								// see if super locations are found
	double time = TimeEnd();
	if (time > WARNING_TIME)
	{
		if (testAllResults != NULL)
		{
			if (testResults != NULL)
				fprintf(testResults, "***** TIMING: superLocTimeCheck Puzzle=%d superLoc time=%.3f entries=%d\n", testing, time, superNext);
			if (saveTiming)
			{
				char testFileName[MAX_PATH];
				sprintf(testFileName, "%s\\superLocTimeCheck seed=0x%llx.txt", prefixString, saveSeed);
				SavePuzzle(testFileName);
			}
		}
	}
#if SUPERLOC_NEW_METHOD_TEST <= 0								// used to find differences between new/old/jar methods
	if (testAllResults != NULL									// in test mode
	&& superEntry != oldSuperEntry
	&& superEntry->list != NULL
	&& oldSuperEntry != NULL
	&& puzzle[oldSuperEntry->list->y][oldSuperEntry->list->x] != puzzle[superEntry->list->y][superEntry->list->x] != 0)
	{
		char testFileName[MAX_PATH];
		sprintf(testFileName, "%s\\%s=%d,%d clears=%.2f %s%s %s=%d,%d clears=%.2f %s%s.txt",
			prefixString, (SUPERLOC_NEW_METHOD_TEST < 0 ? "jar" : "old"),
			oldSuperEntry->list->x, oldSuperEntry->list->y, oldSuperEntry->exposed,
			(oldSuperEntry->list->merged >= 0 ? "merges" : ""),
			(puzzle[oldSuperEntry->list->y][oldSuperEntry->list->x] != 0 ? " failed" : ""),
			(SUPERLOC_NEW_METHOD_TEST == -2 ? "old" : "new"),
			superEntry->list->x, superEntry->list->y, superEntry->exposed,
			(superEntry->list->merged >= 0 ? "merges" : ""),
			(puzzle[superEntry->list->y][superEntry->list->x] != 0 ? " failed" : ""));
			SavePuzzle(testFileName);
	}
#endif

	if (superEntry != NULL)
	{
		double odds;
		if (superEntry->list == NULL)
			lastRuleUsed = GUESS_1;
		else if (superNext == 1)								// only one entry is good
			lastRuleUsed = GUESS_2;
		else
			lastRuleUsed = SUPERLOC;
		if (automatic)
		{
			bestx = superEntry->x;
			besty = superEntry->y;
			return true;
		}
		if (lastRuleUsed == GUESS_1)
		{
			odds = 1.0 - nonPerimeterProb;
			guess1Prob += odds;
			rulesCount[GUESS_1]++;
		}
		else if (lastRuleUsed == GUESS_2)
		{
			odds = 1.0 - superEntry->list->prob;
			guess2Prob += odds;
			rulesCount[GUESS_2]++;
		}
		else // if (lastRuleUsed == SUPERLOC)
		{
			odds = 1.0 - superEntry->list->prob;
			superLocProb += odds;
			rulesCount[SUPERLOC]++;
		}
		SaveBackUp();											// save current board
		guesses++;
		prob *= odds;											// probability
		Expose(superEntry->x, superEntry->y);
		return true;
	}
	return false;
}


int firstx;
int firsty;
int bestMin;

void FindBest(int x, int y)
{
	if (exposed[y][x] < 0										// not exposed
	&& listUsed[y][x] < 0)										// and not in any list
	{
		if (firstx < 0)											// first free entry
		{
			firstx = x;
			firsty = y;
		}
		int min = unexposed[y][x] - adjacentCount[y][x];
		if (unexposed[y][x] > 0)								// not an entry surrounded by mines
		{
			if (bestMin > min)
			{
				bestMin = min;
				bestx = x;
				besty = y;
			}
		}
	}
}


bool UseGuess1()
{
	if (nonPerimeter == 0)
		return false;
	if (GreaterThan(nonPerimeterProb, bestProb))	// perimeter is better
		return false;
	if (testAllResults != NULL)
	{
// always save these because I have not seen a valid case yet
//		if (saveInteresting)
		{
			char testFileName[MAX_PATH];
			sprintf(testFileName, "%s\\Guess1Only seed=0x%llx.txt", prefixString, saveSeed);
			SavePuzzle(testFileName);
		}
		if (testResults != NULL)
			fprintf(testResults, "\n***** GUESS1ONLY: Puzzle=%d Guess1Only! Check this case out.\n", testing);
	}
	int index = -1;
	// find any open area with the fewest adjacent neighbors > 1
	firstx = -1;
	firsty;
	bestMin = 99;
	// find first best candidates
	if (topToBottomPriority)
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				FindBest(x, y);						// find best entry
			}
		}
	}
	else
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				FindBest(x, y);						// find best entry
			}
		}
	}
	// no free entries meet our criteria
	// so pick the first entry 
	if (index < 0)											
	{
		bestx = firstx;
		besty = firsty;
	}
	lastRuleUsed = GUESS_1;
	if (automatic)
		return true;
	SaveBackUp();											// save current board
	guesses++;
	rulesCount[GUESS_1]++;
	double odds = 1.0 - nonPerimeterProb;
	guess1Prob += odds;
	prob *= odds;											// probability
	Expose(bestx, besty);
	bestx = -1;
	return true;
}


bool Guess0Horizontal(int x, int y, int index)
{
	if (x > 0)
	{
		if ((y > 0 && exposed[y - 1][x - 1] != EXPOSE_MINE)
		|| exposed[y][x - 1] != EXPOSE_MINE
		|| (y < height - 1 && exposed[y + 1][x - 1] != EXPOSE_MINE))
			return false;
	}
	if (x < width - 2)
	{
		if ((y > 0 && exposed[y - 1][x + 2] != EXPOSE_MINE)
		|| exposed[y][x + 2] != EXPOSE_MINE
		|| (y < height - 1 && exposed[y + 1][x + 2] != EXPOSE_MINE))
			return false;
	}
	// one neighbor must have 2 unexposed entries needing one mine
	for (int i = 0; i < listArray[index].numNeighbors; i++)
	{
		if (listArray[index].neighbors[i]->needed == 1)
		{
#if 0
			if (testAllResults != NULL)
			{
				char testFileName[MAX_PATH];
				sprintf(testFileName, "%s\\specialGuess0 seed=0x%llx.txt", prefixString, saveSeed);
				SavePuzzle(testFileName);
			}
#endif
			return true;
		}
	}
	return false;
}


bool Guess0Vertical(int x, int y, int index)
{
	if (y > 0)
	{
		if ((x > 0 && exposed[y - 1][x - 1] != EXPOSE_MINE)
		|| exposed[y - 1][x] != EXPOSE_MINE
		|| (x < width - 1 && exposed[y - 1][x + 1] != EXPOSE_MINE))
			return false;
	}
	if (y < height - 2)
	{
		if ((x > 0 && exposed[y + 2][x - 1] != EXPOSE_MINE)
		|| exposed[y + 2][x] != EXPOSE_MINE
		|| (x < width - 1 && exposed[y + 2][x + 1] != EXPOSE_MINE))
			return false;
	}
	// one neighbor must have 2 unexposed entries needing one mine
	for (int i = 0; i < listArray[index].numNeighbors; i++)
	{
		if (listArray[index].neighbors[i]->needed == 1)
		{
#if 0
			if (testAllResults != NULL)
			{
				char testFileName[MAX_PATH];
				sprintf(testFileName, "%s\\specialGuess0 seed=0x%llx.txt", prefixString, saveSeed);
				SavePuzzle(testFileName);
			}
#endif
			return true;
		}
	}
	return false;
}


//	If we have an enlosed area that is all dead then we must just guess
//	See "examples/guess0 dead area.txt"
//	However we must use the same number of mines for all solutions
//	See "examples/guess0 false at 8,12 dead area.txt"
//	The probability may not be the same for all entries
//	See "examples/guess0 at 0,11 is not wrong.txt"
//	The odds is always 1 in n solutions 
//	picking 0,11 is 1/3 chace of being OK but solves the puzzle 100% therafter
//	picking 0,15 is 2/3 chance of being OK but you need a 1/2 guess again later
//	The odds may be different than 50%!
//	See "examples/guess0 66%.txt"
//
//	It is not sufficient to have all the probabilities be the same 
//	See "examples/guess0 false at 15,0.txt"
//	If we are not enclosed in most cases this guess will not work
//	See "example/guess0 false at 0,15.txt" or "examples/guess0 false at 5,12.txt" 
//	So I try to verify that any list has exactly two solutions
//	so I only look at perimeters with an even number of entries
//	and build the solution list and aborting at 3 or more solutions
//	See "examples/guess0.txt"
//
//	It is even possible to have dead areas within the perimeter with adjacent areas.
//	Here is exact case where "-" needs just one mine and nothing else can effect these entries
//	xMMM
//	xX-?
//	xY-?
//	xMMM
//	M are mines or the edge of the puzzle
//	X or Y may be a mine but if not then they need 1 mine and have only 2 unexposed neighbors
//	See "examples/guess0 at 24,8 not enclosed.txt" or "examples/guess0 at 6,14 not enclosed.txt"
//
//	Here is a similar layout that also we should guess right away
//	If the open entry on the left is a mine then both entries on the right are safe
//	However if the entry on the left is not a mine we have a 50-50 guess as above
//	xMMM
//	xX-?
//	-Y-?
//	xMMM
//	See "examples/guess0 at 16,14.txt" or "examples/guess0 at 19,14.txt"
//
//	It is possible to use guess0 on 25% cases but I will not implement that.
//	See "examples/guess0 25%.txt"
//	
bool PickGuess0()
{
	bool ret = false;
	int list;
	int index;
	for (list = 0; list < lists; list++)
	{
		if (listEnclosed[list]
		&& listDead[list]
		&& listMinMines[list] == listMaxMines[list])
		{
			// the first entry will do but we pick the best just to look good
			index = listStart[list];
			int index0;
			for (index0 = listStart[list] + 1; index0 <= listEnd[list]; index0++)
			{
				if (listArray[index].prob > listArray[index0].prob)
					index = index0;
			}
			goto GUESS0;
		}
	}
	for (index = 0; index < listNext; index++)
	{
		int x = listArray[index].x;
		int y = listArray[index].y;
		if (x < width - 1
		&& listUsed[y][x + 1] >= 0
		&& Guess0Horizontal(x, y, index))
			goto GUESS0;
		if (y < height - 1
		&& listUsed[y + 1][x] >= 0
		&& Guess0Vertical(x, y, index))
			goto GUESS0;
	}
	for (list = 0; list < lists; list++)
	{
		if (listEnclosed[list] == false)					// list must be enclosed
			continue;
		int len = listEnd[list] + 1 - listStart[list];
		if ((len & 1) != 0)									// odd length will not do
			continue;
		int count = CheckSolutions(list, listStart[list], 3);	// check first 3 solutions
		if (count != 2)
			continue;
		index = listStart[list];
GUESS0:	lastRuleUsed = GUESS_0;
		if (automatic)
		{
			bestx = listArray[index].x;
			besty = listArray[index].y;
			return true;
		}
		SaveBackUp();										// save current board
		guesses++;
		rulesCount[GUESS_0]++;
		guess0Prob += (1.0 - listArray[index].prob);
		prob *= (1.0 - listArray[index].prob);				// probability
		Expose(listArray[index].x, listArray[index].y);
		ret = true;
		break;
	}
	return ret;
}


void AdjustPerimeterLists()
{
	listNext -= nonPerimeterInEndGame;
	listEnd[0] = listNext - 1;
	nonPerimeter = nonPerimeterInEndGame;
	nonPerimeterInEndGame = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (listUsed[y][x] >= listNext)						// remove from the list of used entries
				listUsed[y][x] = -1;
		}
	}
	// adjacentCount[][],  adjacentMask[][]  and neighbors[][] do not change
}


// end game should use not use similar entries
// otherwise the mine count is off 
// which screws up the pruning in CreateSolutions()
void RemoveSimilarEntries(int start, int end)
{
	for (int i = listStart[start]; i <= listEnd[end]; i++)
	{
		if (listArray[i].numSimilar != 0)
		{
			listArray[i].numSimilar = 0;
			listArray[i].similar = NULL;
		}
	}
}


bool UseGameTree()
{
	int start, end, length, expectedSolutions, maxMines;
	if (lists > 1)
	{
		for (int list = 0; list < lists; list++)
		{
			if (listEnclosed[list]
			&& listDead[list] == false
			&& listMinMines[list] == listMaxMines[list])
			{
				start = list;
				end = list;
				length = listEnd[list] - listStart[list] + 1;
				maxMines = listMinMines[list];
				expectedSolutions = listSolutions[list][0];
				RemoveSimilarEntries(start, end);			// remove all similar entries to get full solutions
#if 0
				if (testAllResults != NULL)
				{
					if (saveInteresting)
					{
						char testFileName[MAX_PATH];
						sprintf(testFileName, "%s\\earlyGametree seed=0x%llx.txt", prefixString, saveSeed);
						SavePuzzle(testFileName);
					}
				}
#endif
				gametreeUsedEarly++;
				goto RUN;
			}
		}
	}

	if (totalSolutions <= (double)maxSolutions
	&&  maxEndPlayLength < remainingCount + mineCount)	// save length of any valid end play
		maxEndPlayLength = remainingCount + mineCount;
	if (remainingCount + mineCount > END_PLAY_COUNT)
	{
		if (totalSolutions <= (double)maxSolutions)
		{
			numMissedEndPlays++;
			if (testAllResults != NULL)
			{
				if (saveInteresting)
				{
					char testFileName[MAX_PATH];
					sprintf(testFileName, "%s\\MissedEndPlay seed=0x%llx.txt", prefixString, saveSeed);
					SavePuzzle(testFileName);
				}
				if (testResults != NULL)
					fprintf(testResults, "\n***** INTERESTING: Puzzle=%d MissedEndPlay (len=%d)\n", testing, remainingCount + mineCount);
			}
		}
		return false;
	}
	// add stats for end plays
	numEndPlays++;										// end play possible
	if (totalSolutions <= (double)SMALL_SOLUTIONS)
		numEndPlaySmall++;
	else if (totalSolutions > (double)maxSolutions)
	{
		numEndPlayLarge++;
		return false;
	}

	RemoveSimilarEntries(0, lists - 1);					// remove all similar entries to get full solutions

	// end game check use all unexposed entries
	// even if nonPerimeter == 0 because it still improves the odds
	// reset next pointers when we combine all lists
	for (int i = 1; i < lists; i++)
	{
		listArray[listEnd[i - 1]].next = &listArray[listStart[i]];
		listArray[listEnd[i]].next = &listArray[0];
	}
	lists = 0;											// to insure CreateLists() below adds them to the right list
	nonPerimeterInEndGame = nonPerimeter;
	nonPerimeter = 0;
	if (topToBottomPriority)
	{
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				if (exposed[y][x] < 0						// find an unexposed tile
				&& listUsed[y][x] < 0)						// not on the list already
				{
					CreateList(x, y, true);					// no neighbors should be used
				}
			}
		}
	}
	else
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (exposed[y][x] < 0						// find an unexposed tile
				&& listUsed[y][x] < 0)						// not on the list already
				{
					CreateList(x, y, true);					// no neighbors should be used
				}
			}
		}
	}
	listEnd[0] = listNext - 1;
	lists = 1;
	if (maxPerimeter < listNext)
		maxPerimeter = listNext;
	start = 0;
	end = lists - 1;
	length = listNext;
	expectedSolutions = (int)totalSolutions;
	maxMines = mineCount;

	// recreate solutions and save them
RUN:int solutionCount = CreatePerimeterSolutions(start, end, maxMines, true);	// save solutions now
	if (solutionCount != expectedSolutions)
	{
		if (testAllResults != NULL)
		{
			int sum = GetSolutionCount();
			fprintf(testAllResults, "\n***** ERROR: Puzzle=%d Gametree solution count %d != expected count %d\n",
				testing, solutionCount, expectedSolutions);
		}
	}
	int base = listStart[start];
	for (int n = base; n < base + length; n++)
	{
		if (listArray[n].minValue == listArray[n].maxValue)
			listArray[n].dead = true;						// these are dead 
	}
	int entries = 0;
	for (int i = 0; i < length; i++)
	{
		// dead entries will not help so we skip those
		solutions[i].dead = listArray[base + i].dead;
		if (solutions[i].dead)
			continue;
		entries++;
	}
	// if entries == 0 any place will do
	if (entries == 0)
		entries = 1;
	if (testResults == NULL)
		gametreeMaxDepth = 0;
	TimeStart();
	HashClear();											// restart hash table 
	gametreeUsed++;
	gametreeNodes = 0;
	int	gametreeBest = 0;
	qsort(solutions, length, sizeof(Solutions), CompareGameTreeList);
	if (saveGameTreeResults > 1
	&& gametreeResults != NULL)
	{
		// show all entries in order
		fprintf(gametreeResults, "----------------------------------------------\n");
		for (int i = base; i < base + length; i++)
			fprintf(gametreeResults, "index=%2d %d,%d mines=%d %s\n",
				i, listArray[i].x, listArray[i].y, mines[listArray[i].y][listArray[i].x],
				(listArray[i].dead ? "Dead" : ""));
		fprintf(gametreeResults, "----------------------------------------------\n");
		for (int i = 0; i < entries; i++)
		{
			int y = listArray[solutions[i].index].y;
			int x = listArray[solutions[i].index].x;
			fprintf(gametreeResults, "%d,%d total=%3d min=%3d max=%3d solutions=",
				x, y, solutions[i].total, solutions[i].min, solutions[i].max);
			for (int j = mines[y][x]; j <= 8; j++)
				fprintf(gametreeResults, "%d,", solutions[i].solutions[j].index);
			fprintf(gametreeResults, "\n");
		}
		fprintf(gametreeResults, "----------------------------------------------\n");
	}
	char string[END_PLAY_COUNT + 1];
	for (int i = 0; i < length; i++)
		string[i] = '.';
	string[length] = 0;
	HashClear();
	int gametreeWins = GameTreeProcess(1, solutions, base, entries, gametreeLinksNext, gametreeBest, 0, string);
	if (!Equal(bestProb, listArray[gametreeBest].prob))
		gametreeHelped++;
	lastRuleUsed = GAMETREE;
	int index = gametreeBest;
	prob = listArray[index].prob;
	double time = TimeEnd();
#if SHOW_GAMETREE_TIMES
	if (gametreeTimes != NULL)
	{
		fprintf(gametreeTimes, "entries=%2d mines=%2d P=%2d NP=%2d NP mines=%6.2f solutions=%3d depth=%2d time=%5.3f seconds nodes=%10d\n",
			entries, mineCount, remainingCount + mineCount - nonPerimeterInEndGame, nonPerimeterInEndGame,
			(double)nonPerimeterMines / listSolutions[0][0], listSolutions[0][0],
			gametreeMaxDepth, time, gametreeNodes);
	}
#endif
	if (saveGameTreeResults > 0
	&& gametreeResults != NULL)
	{
		// show results
		int hits, used;
		HashStats(used, hits);							// get hash table results
		if (saveGameTreeResults > 2)
		{
			HashDump(gametreeResults);					// dump hash table for inspection
			HashInit();
		}
		fprintf(gametreeResults, "#############################################\n");
		fprintf(gametreeResults, "mines=%d entries=%d solutions=%d\n", mineCount, entries, solutionCount);
		fprintf(gametreeResults, "hash table entries=%d hits=%d\n", used, hits);
		fprintf(gametreeResults, "perimeter P=%d non-perimeter NP=%d NP mines=%.2f\n",
			remainingCount + mineCount - nonPerimeterInEndGame, nonPerimeterInEndGame,
			(double)nonPerimeterMines / solutionCount);
		fprintf(gametreeResults, "best=%d,%d wins=%d (%5.2f%%) time=%5.3f seconds nodes=%d\n",
			listArray[gametreeBest].x, listArray[gametreeBest].y, gametreeWins,
			100.0 * gametreeWins / solutionCount, time, gametreeNodes);
		int index = 0;
		while (solutions[index].index != gametreeBest)
			index++;
		fprintf(gametreeResults, "picked entry %d of %d  depth=%d\n", index + 1, entries, gametreeMaxDepth);
	}
	if (time > WARNING_TIME)
	{
		int index = 0;
		while (solutions[index].index != gametreeBest)
			index++;
		if (testAllResults != NULL)
		{
			if (testResults != NULL)
			{
				int hits, used;
				HashStats(used, hits);							// get hash table results
				fprintf(testResults, "***** TIMING: Puzzle=%d gametree time=%.3f nodes=%d mines=%d entries=%d solutions=%d wins=%d\n",
					testing, time, gametreeNodes, mineCount, listNext, listSolutions[0][0], gametreeWins);
				fprintf(testResults, "***** TIMING: hash table entries=%d hits=%d\n", used, hits);
				fprintf(testResults, "***** TIMING: Puzzle=%d perimeter P=%d non-perimeter NP=%d NP mines=%.2f picked %d of %d\n",
					testing, remainingCount + mineCount - nonPerimeterInEndGame, nonPerimeterInEndGame,
					(double)nonPerimeterMines / listSolutions[0][0], index + 1, entries);
			}
			if (saveTiming)
			{
				char testFileName[MAX_PATH];
				sprintf(testFileName, "%s\\gameTreeTimeCheck seed=0x%llx.txt", prefixString, saveSeed);
				SavePuzzle(testFileName);
			}
		}
	}
	// make move now
	lastRuleUsed = GAMETREE;
	if (automatic)
	{
		bestx = listArray[index].x;
		besty = listArray[index].y;
		return true;
	}
	rulesCount[GAMETREE]++;
	SaveBackUp();										// save current board
	guesses++;
	double odds = (1.0 - listArray[index].prob);
	prob *= odds;										// probability
	gameTreeProb += odds;
	Expose(listArray[index].x, listArray[index].y);
	AdjustPerimeterLists();								// reset what we changed
	return true;
}
