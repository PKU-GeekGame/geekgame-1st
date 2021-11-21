#include "stdafx.h"


struct Recover													// used to recover similar entries
{
	List *	list;												// perimeter affected
	List *	similar;
	int		numSimilar;
};

struct ForcedMove
{
	List *	list;												// entry that was forced
	bool	exposed;											// exposed or a mine
};


struct MergedList
{
	List *	list0;												// two entries that were merged
	List *	list1;												
};

#define		MAX_SIMILAR			32
#define		MAX_FORCED			32
Recover		RecoverSimilar[MAX_SIMILAR];
int			recoverCount;
ForcedMove	ForcedMoves[MAX_FORCED];
int			forcedCount;
int			fastMinesUsed;
int			fastConflicts;
int			fastCompletedSolutions;
bool		excludeLists[MAX_LISTS];


void FastCreateSolutions(List * list, List * end, int maxCount);
void FastTryExpose(List * list, List * next, List * end, int maxCount);
void ForceMine(List * original, List * list, Neighbor * neighbor);


void VerifyList()
{
	for (int i = 0; i < lists; i++)
	{
		List * list = &listArray[listStart[i]];
		List * list0 = list;
		do
		{
			list = list->next;
		} while (list != list0);
	}
}


SuperLoc * SuperEntry(List * list)
{
	for (int n = 0; n < superNext; n++)
	{
		if (SuperLocArray[n].list == list)
			return &SuperLocArray[n];
	}
	return NULL;
}


void AddLinks(SuperLoc * superLoc, List * list1)
{
	if (superLoc->list == list1)
		return;
	SuperLoc * super1 = SuperEntry(list1);
	if (super1 == NULL)											// not an entry we are concerned with
		return;
	for (int n = 0; n < superLoc->numLinks; n++)
	{
		if (superLoc->links[n] == list1)						// already linked
			return;
	}
	if (superLoc->numLinks < MAX_LINKS - 1)
	{
		superLoc->links[superLoc->numLinks++] = list1;
	}
}


void ForceExpose(List * original, List * list, Neighbor * neighbor)
{
	for (int n = 0; n < neighbor->count; n++)					// fine perimeter that is a forced mine
	{
		List * list1 = neighbor->entries[n];
		if (list1->tried < 0)
		{
			ForcedMoves[forcedCount].list = list1;
			ForcedMoves[forcedCount].exposed = true;
			forcedCount++;
			list1->tried = 0;									// expose this
			for (int n = 0; n < list1->numNeighbors; n++)		// reflect that in our neighbors
			{
				--list1->neighbors[n]->unexposed;
			}
			for (int n = 0; n < list1->numNeighbors; n++)		// reflect that in our neighbors
			{
				if (list1->neighbors[n]->unexposed > 0
				&& list1->neighbors[n]->unexposed == list1->neighbors[n]->needed)
					ForceMine(original, list1, list1->neighbors[n]);	// force a mine	
			}
		}
	}
}


void ForceMine(List * original, List * list, Neighbor * neighbor)
{
	for (int n = 0; n < neighbor->count; n++)					// fine perimeter that is a forced mine
	{
		List * list1 = neighbor->entries[n];
		if (list1->tried < 0)
		{
			ForcedMoves[forcedCount].list = list1;
			ForcedMoves[forcedCount].exposed = false;
			forcedCount++;
			list1->tried = 1;									// place mine
			fastMinesUsed++;
			for (int n = 0; n < list1->numNeighbors; n++)		// reflect that in our neighbors
			{
				--list1->neighbors[n]->needed;
				--list1->neighbors[n]->unexposed;
			}
			for (int n = 0; n < list1->numNeighbors; n++)		// reflect that in our neighbors
			{
				if (list1->neighbors[n]->unexposed > 0
				&& list1->neighbors[n]->needed == 0)
					ForceExpose(original, list1, list1->neighbors[n]);	// force expose 
			}
		}
	}
}


void ForceMoves(List * list, Neighbor * neighbor)
{
	for (int n = 0; n < list->numNeighbors; n++)
	{
		if (list->neighbors[n]->unexposed > 0
		&& list->neighbors[n]->unexposed == list->neighbors[n]->needed)
			ForceMine(list, list, list->neighbors[n]);			// force a mine here
	}
}


void ForceUndoMoves()
{
	while (forcedCount > 0)
	{
		forcedCount--;
		List * list = ForcedMoves[forcedCount].list;
		list->tried = -1;										// undo forced move
		if (ForcedMoves[forcedCount].exposed)
		{
			for (int n = 0; n < list->numNeighbors; n++)		// update our neighbors
			{
				++list->neighbors[n]->unexposed;
			}
		}
		else
		{
			for (int n = 0; n < list->numNeighbors; n++)		// update our neighbors
			{
				++list->neighbors[n]->needed;
				++list->neighbors[n]->unexposed;
			}
		}
	}
}


// add temp neighbor and update this list entries neighbors
Neighbor * FastCreateNeighbor(List * list)
{
	list->tried = 0;											// expose this
	forcedCount = 0;
	Neighbor * next = &neighborArray[neighborNext];
	next->x = list->x;
	next->y = list->y;
	next->unexposed = unexposed[list->y][list->x];
	// next->needed filled in later
	next->count = 0;
	neighborNext++;
	for (int n = 0; n < list->numNeighbors; n++)
	{
		list->neighbors[n]->unexposed--;						// reduce counts for our neighbors
	}
	return next;
}


void FastAddPerimeter(List * last, int x, int y, Neighbor * neighbor, int numSimilar)
{
	listUsed[y][x] = listNext;
	List * list = &listArray[listNext];
	list->x = x;
	list->y = y;
	if (neighbor == NULL)
		list->numNeighbors = 0;
	else
	{
		list->numNeighbors = 1;
		list->neighbors[0] = neighbor;
	}
	list->dead = false;
	list->numSimilar = numSimilar;
	if (numSimilar != 0)
	{
		list->similar = &listArray[listNext - numSimilar + 1];
		if (numSimilar > 1)
		{
			List * list0 = list->similar;
			while (list0 != list)
			{
				list0->numSimilar = numSimilar;
				list0++;
			}
		}
	}
	list->next = last->next;
	last->next = list;
	list->tried = -1;
	list->list = lists;
	list->merged = -1;
	listNext++;
	if (maxPerimeter < listNext)
		maxPerimeter = listNext;
}


void FastAddAdjacent(List * list, List * last, int x, int y)
{
	listUsed[y][x] = listNext;
	list->x = x;
	list->y = y;
	list->numNeighbors = 0;
	list->dead = false;
	list->numSimilar = 0;
	list->similar = NULL;
	list->next = list;
	list->next = last->next;
	last->next = list;
	list->tried = -1;
	list->merged = -1;
	list->list = lists;
	listNext++;
	if (maxPerimeter < listNext)
		maxPerimeter = listNext;
}


List * FastCreateNewPerimeter(SuperLoc * superLoc)
{
	int x = superLoc->x;
	int y = superLoc->y;
	superLoc->list = &listArray[listNext];
	if (x > 0 && y > 0)
	{
		if (listUsed[y - 1][x - 1] >= 0)					// found adjacent list
		{
			List * list1 = &listArray[listUsed[y - 1][x - 1]];
			FastAddAdjacent(superLoc->list, list1, x, y);	// crete new perimeter
			return list1;
		}
	}
	if (y > 0)
	{
		if (listUsed[y - 1][x] >= 0)						// found adjacent list
		{
			List * list1 = &listArray[listUsed[y - 1][x]];
			FastAddAdjacent(superLoc->list, list1, x, y);	// crete new perimeter
			return list1;
		}
	}
	if (x < width - 1 && y > 0)
	{
		if (listUsed[y - 1][x + 1] >= 0)					// found adjacent list
		{
			List * list1 = &listArray[listUsed[y - 1][x + 1]];
			FastAddAdjacent(superLoc->list, list1, x, y);	// crete new perimeter
			return list1;
		}
	}
	if (x > 0)
	{
		if (listUsed[y][x - 1] >= 0)						// found adjacent list
		{
			List * list1 = &listArray[listUsed[y][x - 1]];
			FastAddAdjacent(superLoc->list, list1, x, y);	// crete new perimeter
			return list1;
		}
	}
	if (x < width - 1)
	{
		if (listUsed[y][x + 1] >= 0)						// found adjacent list
		{
			List * list1 = &listArray[listUsed[y][x + 1]];
			FastAddAdjacent(superLoc->list, list1, x, y);	// crete new perimeter
			return list1;
		}
	}
	if (x > 0 && y < height - 1)
	{
		if (listUsed[y + 1][x - 1] >= 0)					// found adjacent list
		{
			List * list1 = &listArray[listUsed[y + 1][x - 1]];
			FastAddAdjacent(superLoc->list, list1, x, y);	// crete new perimeter
			return list1;
		}
	}
	if (y < height - 1)
	{
		if (listUsed[y + 1][x] >= 0)						// found adjacent list
		{
			List * list1 = &listArray[listUsed[y + 1][x]];
			FastAddAdjacent(superLoc->list, list1, x, y);	// crete new perimeter
			return list1;
		}
	}
	if (x < width - 1 && y < height - 1)
	{
		if (listUsed[y + 1][x + 1] >= 0)					// found adjacent list
		{
			List * list1 = &listArray[listUsed[y + 1][x + 1]];
			FastAddAdjacent(superLoc->list, list1, x, y);	// crete new perimeter
			return list1;
		}
	}
	return NULL;
}


// add new perimeter to this list and return the number we added
int FastAddPerimeter(List * list, Neighbor * neighbor)
{
	int count = 0;
	int x = list->x;
	int y = list->y;
	if (x > 0 && y > 0)
	{
		if (listUsed[y - 1][x - 1] < 0					// not on a list
		&& exposed[y - 1][x - 1] < 0)					// find an unexposed tile
		{
			count++;
			FastAddPerimeter(list, x - 1, y - 1, neighbor, count);
		}
	}
	if (y > 0)
	{
		if (listUsed[y - 1][x] < 0						// not on a list
		&& exposed[y - 1][x] < 0)						// find an unexposed tile
		{
			count++;
			FastAddPerimeter(list, x, y - 1, neighbor, count);
		}
	}
	if (x < width - 1 && y > 0)
	{
		if (listUsed[y - 1][x + 1] < 0					// not on a list
		&& exposed[y - 1][x + 1] < 0)					// find an unexposed tile
		{
			count++;
			FastAddPerimeter(list, x + 1, y - 1, neighbor, count);
		}
	}
	if (x > 0)
	{
		if (listUsed[y][x - 1] < 0						// not on a list
		&& exposed[y][x - 1] < 0)						// find an unexposed tile
		{
			count++;
			FastAddPerimeter(list, x - 1, y, neighbor, count);
		}
	}
	if (x < width - 1)
	{
		if (listUsed[y][x + 1] < 0						// not on a list
		&& exposed[y][x + 1] < 0)						// find an unexposed tile
		{
			count++;
			FastAddPerimeter(list, x + 1, y, neighbor, count);
		}
	}
	if (x > 0 && y < height - 1)
	{
		if (listUsed[y + 1][x - 1] < 0					// not on a list
		&& exposed[y + 1][x - 1] < 0)					// find an unexposed tile
		{
			count++;
			FastAddPerimeter(list, x - 1, y + 1, neighbor, count);
		}
	}
	if (y < height - 1)
	{
		if (listUsed[y + 1][x] < 0						// not on a list
		&& exposed[y + 1][x] < 0)						// find an unexposed tile
		{
			count++;
			FastAddPerimeter(list, x, y + 1, neighbor, count);
		}
	}
	if (x < width - 1 && y < height - 1)
	{
		if (listUsed[y + 1][x + 1] < 0					// not on a list
		&& exposed[y + 1][x + 1] < 0)					// find an unexposed tile
		{
			count++;
			FastAddPerimeter(list, x + 1, y + 1, neighbor, count);
		}
	}
	return count;
}


void FastAddMergeLists(List * list0, List * list1)
{
	List * next = list1->next;
	list1->next = list0->next;
	list0->next = next;
	excludeLists[list1->list] = true;
}


// see if lists merged
// if so link them together
// and save the links to restore them later
// also flag the excludeLists so the probability calculation will be correct
void FastMergeLists(List * list)
{
	int x = list->x;
	int y = list->y;
	int index;
	if (x > 0 && y > 0)
	{
		if (listUsed[y - 1][x - 1] >= 0					// on a list 
		&& excludeLists[listArray[index = listUsed[y - 1][x - 1]].list] == false)
		{
			FastAddMergeLists(list, &listArray[index]);
		}
	}
	if (y > 0)
	{
		if (listUsed[y - 1][x] >= 0						// not on a list
		&& excludeLists[listArray[index = listUsed[y - 1][x]].list] == false)
		{
			FastAddMergeLists(list, &listArray[index]);
		}
	}
	if (x < width - 1 && y > 0)
	{
		if (listUsed[y - 1][x + 1] >= 0					// not on a list
		&& excludeLists[listArray[index = listUsed[y - 1][x + 1]].list] == false)
		{
			FastAddMergeLists(list, &listArray[index]);
		}
	}
	if (x > 0)
	{
		if (listUsed[y][x - 1] >= 0						// not on a list
		&& excludeLists[listArray[index = listUsed[y][x - 1]].list] == false)
		{
			FastAddMergeLists(list, &listArray[index]);
		}
	}
	if (x < width - 1)
	{
		if (listUsed[y][x + 1] >= 0						// not on a list
		&& excludeLists[listArray[index = listUsed[y][x + 1]].list] == false)
		{
			FastAddMergeLists(list, &listArray[index]);
		}
	}
	if (x > 0 && y < height - 1)
	{
		if (listUsed[y + 1][x - 1] >= 0					// not on a list
		&& excludeLists[listArray[index = listUsed[y + 1][x - 1]].list] == false)
		{
			FastAddMergeLists(list, &listArray[index]);
		}
	}
	if (y < height - 1)
	{
		if (listUsed[y + 1][x] >= 0						// not on a list
		&& excludeLists[listArray[index = listUsed[y + 1][x]].list] == false)
		{
			FastAddMergeLists(list, &listArray[index]);
		}
	}
	if (x < width - 1 && y < height - 1)
	{
		if (listUsed[y + 1][x + 1] >= 0					// not on a list
		&& excludeLists[listArray[index = listUsed[y + 1][x + 1]].list] == false)
		{
			FastAddMergeLists(list, &listArray[index]);
		}
	}
}


void SaveSimilarEntry(List * list)
{
	for (int i = 0; i < recoverCount; i++)
	{
		if (RecoverSimilar[i].list == list)
			return;
	}
	RecoverSimilar[recoverCount].list = list;
	RecoverSimilar[recoverCount].similar = list->similar;
	RecoverSimilar[recoverCount].numSimilar = list->numSimilar;
	recoverCount++;
}


void SaveSimilar(List * list, List * list1)				// save similar if necessary
{
	if (list1->similar != NULL)
	{
		SaveSimilarEntry(list1);
		List * parent = list1->similar;
		List * newParent = (list1->similar == list1 ? NULL : list1->similar);
		int n = list1->numSimilar - 1;					// remaining similar entries
		list1->numSimilar = 0;
		list1->similar = NULL;
		List * next = (newParent == NULL ? list1->next : newParent);
		while (n > 0)
		{
			if (next->similar == parent)
			{
				SaveSimilarEntry(next);
				next->numSimilar--;
				if (newParent == NULL)
					newParent = next;
				next->similar = newParent;
				n--;
			}
			next = next->next;
		}
	}
}


void FastUpdateNeighbors(List *list, Neighbor * neighbor)
{
	List * list1;
	int x = list->x;
	int y = list->y;
	recoverCount = 0;									// to restore similar entries
	if (x > 0 && y > 0)
	{
		if (listUsed[y - 1][x - 1] >= 0					// perimeter and on any list but the new one
		&& (list1 = &listArray[listUsed[y - 1][x - 1]])->list != lists) 
		{
			list1->neighbors[list1->numNeighbors++] = neighbor;
			neighbor->entries[neighbor->count++] = list1;
			SaveSimilar(list, list1);					// save similar if necessary
		}
	}
	if (y > 0)
	{
		if (listUsed[y - 1][x] >= 0						// perimeter and on any list but the new one
		&& (list1 = &listArray[listUsed[y - 1][x]])->list != lists)
		{
			list1->neighbors[list1->numNeighbors++] = neighbor;
			neighbor->entries[neighbor->count++] = list1;
			SaveSimilar(list, list1);					// save similar if necessary
		}
	}
	if (x < width - 1 && y > 0)
	{
		if (listUsed[y - 1][x + 1] >= 0					// perimeter and on any list but the new one
		&& (list1 = &listArray[listUsed[y - 1][x + 1]])->list != lists)
		{
			list1->neighbors[list1->numNeighbors++] = neighbor;
			neighbor->entries[neighbor->count++] = list1;
			SaveSimilar(list, list1);					// save similar if necessary
		}
	}
	if (x > 0)
	{
		if (listUsed[y][x - 1] >= 0						// perimeter and on any list but the new one
		&& (list1 = &listArray[listUsed[y][x - 1]])->list != lists)
		{
			list1->neighbors[list1->numNeighbors++] = neighbor;
			neighbor->entries[neighbor->count++] = list1;
			SaveSimilar(list, list1);					// save similar if necessary
		}
	}
	if (x < width - 1)
	{
		if (listUsed[y][x + 1] >= 0						// perimeter and on any list but the new one
		&& (list1 = &listArray[listUsed[y][x + 1]])->list != lists)
		{
			list1->neighbors[list1->numNeighbors++] = neighbor;
			neighbor->entries[neighbor->count++] = list1;
			SaveSimilar(list, list1);					// save similar if necessary
		}
	}
	if (x > 0 && y < height - 1)
	{
		if (listUsed[y + 1][x - 1] >= 0					// perimeter and on any list but the new one
		&& (list1 = &listArray[listUsed[y + 1][x - 1]])->list != lists)
		{
			list1->neighbors[list1->numNeighbors++] = neighbor;
			neighbor->entries[neighbor->count++] = list1;
			SaveSimilar(list, list1);					// save similar if necessary
		}
	}
	if (y < height - 1)
	{
		if (listUsed[y + 1][x] >= 0						// perimeter and on any list but the new one
		&& (list1 = &listArray[listUsed[y + 1][x]])->list != lists)
		{
			list1->neighbors[list1->numNeighbors++] = neighbor;
			neighbor->entries[neighbor->count++] = list1;
			SaveSimilar(list, list1);					// save similar if necessary
		}
	}
	if (x < width - 1 && y < height - 1)
	{
		if (listUsed[y + 1][x + 1] >= 0					// perimeter and on any list but the new one
		&& (list1 = &listArray[listUsed[y + 1][x + 1]])->list != lists)
		{
			list1->neighbors[list1->numNeighbors++] = neighbor;
			neighbor->entries[neighbor->count++] = list1;
			SaveSimilar(list, list1);					// save similar if necessary
		}
	}
	if (list->similar != NULL)
		SaveSimilar(list, list);						// save similar if necessary
}


// remove temp neighbor and update this list entries neighbors
void FastRemoveNeighbor(List * list, int oldNeighborNext)
{
	neighborNext = oldNeighborNext;
	Neighbor * neighbor = &neighborArray[neighborNext];
	for (int n = 0; n < neighbor->count; n++)			// remove us as a neighbor
		neighbor->entries[n]->numNeighbors--;
	for (int n = 0; n < list->numNeighbors; n++)
	{
		list->neighbors[n]->unexposed++;
	}
	list->tried = -1;
}


// remove new perimeter by resetting the next pointers
void FastRemovePerimeter(List * list, int count)
{
	for (int n = count; n > 0; n--)
	{
		--listNext;
		List * list1 = &listArray[listNext];
		listUsed[list1->y][list1->x] = -1;
	}
	// restore links now
	for (int i = 0; i < lists; i++)
	{
		if (excludeLists[i] == false)					// this list was not touched
			continue;
		for (int n = listStart[i]; n < listEnd[i]; n++)
			listArray[n].next = &listArray[n + 1];
		listArray[listEnd[i]].next = &listArray[listStart[i]];
	}
	// restore similar links too
	for (int n = 0; n < recoverCount; n++)
	{
		List * list1 = RecoverSimilar[n].list;
		list1->numSimilar = RecoverSimilar[n].numSimilar;
		list1->similar = RecoverSimilar[n].similar;
	}
}


double FastCountSolutions(int list, int mines)
{
	while (excludeLists[list])								// exclude old list
		list++;
	if (list >= lists)
		return Comb(nonPerimeter, mines);
	double solutions = 0;
	for (int mi = 0; mi <= listMaxMines[list] - listMinMines[list]; mi++)
	{
		solutions += listSolutions[list][mi] * FastCountSolutions(list + 1, mines - listMinMines[list] - mi);
	}
	return solutions;
}


double FastTotalSolutions()
{
	double totalSolutions = 0.0;
	for (int mi = 0; mi <= listMaxMines[lists] - listMinMines[lists]; mi++)
	{
		totalSolutions += listSolutions[lists][mi] * FastCountSolutions(0, mineCount - listMinMines[lists]  - mi);
	}
	return totalSolutions;
}


void FastCountCompletedSolutions(int list)
{
	fastCompletedSolutions++;
	// add multiplier for similar entries and update true mines used
	int multiply = 1;
	int minesUsedAdjust = fastMinesUsed;
	List * ptr = &listArray[listStart[list]];
	do
	{
		if (ptr->similar != NULL)								// similar may be a mine
		{
			if (ptr->neighbors[0]->needed > 0)					// a mine is needed
				ptr->clear = false;
		}
		else if (ptr->tried > 0)								// this is not a cleared entry
			ptr->clear = false;
		if (ptr->similar == ptr)								// the parent
		{
			int needed = ptr->neighbors[0]->needed;
			minesUsedAdjust += needed;
			multiply *= Combinations[ptr->numSimilar][needed];
		}
		ptr = ptr->next;
	} while (ptr != &listArray[listStart[list]]);				// stop when we loop back
	if (listMinMines[lists] > minesUsedAdjust)
	{
		if (listMinMines[lists] != MAX_MINES)					// no entry yet
		{
			int up = listMinMines[lists] - minesUsedAdjust;		// amount to move entries up
			for (int i = listMaxMines[lists] - listMinMines[lists]; i >= 0; i--)
			{
				listSolutions[lists][i + up] = listSolutions[lists][i];
			}
			while (--up >= 0)
			{
				listSolutions[lists][up] = 0;					// clear moved entries
			}
		}
		listMinMines[lists] = minesUsedAdjust;
	}
	if (listMaxMines[lists] < minesUsedAdjust)
		listMaxMines[lists] = minesUsedAdjust;
	listSolutions[lists][minesUsedAdjust - listMinMines[lists]] += multiply;	// count these solutions
	if (saveSuperLocResults > 2									// list all solutions
	&&  superLocResults != NULL)
	{
		for (int i = 0; i < listNext; i++)
		{
			if (listArray[i].tried < 0)
				fprintf(superLocResults, ".");
			else if (listArray[i].tried > 0)
				fprintf(superLocResults, "M");
			else
				fprintf(superLocResults, "-");
		}
		fprintf(superLocResults, " mines=%d mul=%d\n", minesUsedAdjust, multiply);
	}
}


void FastTryMine(List * list, List * next, List * end, int maxCount)
{
	if (fastMinesUsed == maxCount)								// no mines left to add
	{
		fastConflicts++;
		return;
	}
	List * forceExpose = NULL;
	List * forceMine = NULL;
	list->tried = 1;											// place mine
	fastMinesUsed++;
	bool conflict = false;
	for (int n = 0; n < list->numNeighbors; n++)				// reflect that in our neighbors
	{
		if (--list->neighbors[n]->needed < 0)
			conflict = true;
		if (list->neighbors[n]->needed > --list->neighbors[n]->unexposed)
			conflict = true;
		else if (conflict == false
		&& forceExpose == NULL
		&& forceMine == NULL)
		{
			if (list->neighbors[n]->needed == list->neighbors[n]->unexposed
			&&  list->neighbors[n]->unexposed > 0)
			{
				for (int i = 0; i < list->neighbors[n]->count; i++)
				{
					if (list->neighbors[n]->entries[i]->tried < 0
					&& list->neighbors[n]->entries[i]->similar == 0)
					{
						forceMine = list->neighbors[n]->entries[i];
						break;
					}
				}
			}
			else if (list->neighbors[n]->needed == 0
			&& list->neighbors[n]->unexposed > 0)
			{
				for (int i = 0; i < list->neighbors[n]->count; i++)
				{
					if (list->neighbors[n]->entries[i]->tried < 0
					&& list->neighbors[n]->entries[i]->similar == 0)
					{
						forceExpose = list->neighbors[n]->entries[i];
						break;
					}
				}
			}
		}
	}
	if (conflict == false)
	{
		if (forceMine != NULL)
			FastTryMine(forceMine, next, end, maxCount);		// forced mine at this entry	
		else if (forceExpose != NULL)
			FastTryExpose(forceExpose, next, end, maxCount);	// forced expose this entry	
		else
			FastCreateSolutions(next, end, maxCount);			// continue until we are done or abort
	}
	else
		fastConflicts++;
	list->tried = -1;
	fastMinesUsed--;
	for (int n = 0; n < list->numNeighbors; n++)				// undo mine
	{
		list->neighbors[n]->needed++;
		list->neighbors[n]->unexposed++;
	}
}


void FastTryExpose(List * list, List * next, List * end, int maxCount)
{
	List * forceExpose = NULL;
	List * forceMine = NULL;
	list->tried = 0;											// expose this
	bool conflict = false;
	for (int n = 0; n < list->numNeighbors; n++)				// reflect that in our neighbors
	{
		if (list->neighbors[n]->needed > --list->neighbors[n]->unexposed)
			conflict = true;
		else if (conflict == false
		&& forceExpose == NULL
		&& forceMine == NULL)
		{
			if (list->neighbors[n]->needed == list->neighbors[n]->unexposed
			&&  list->neighbors[n]->unexposed > 0)
			{
				for (int i = 0; i < list->neighbors[n]->count; i++)
				{
					if (list->neighbors[n]->entries[i]->tried < 0
					&& list->neighbors[n]->entries[i]->similar == 0)
					{
						forceMine = list->neighbors[n]->entries[i];
						break;
					}
				}
			}
			else if (list->neighbors[n]->needed == 0
			&& list->neighbors[n]->unexposed > 0)
			{
				for (int i = 0; i < list->neighbors[n]->count; i++)
				{
					if (list->neighbors[n]->entries[i]->tried < 0
					&& list->neighbors[n]->entries[i]->similar == 0)
					{
						forceExpose = list->neighbors[n]->entries[i];
						break;
					}
				}
			}
		}
	}
	if (conflict == false)
	{
		if (forceMine != NULL)
			FastTryMine(forceMine, next, end, maxCount);		// forced mine at this entry	
		else if (forceExpose != NULL)
			FastTryExpose(forceExpose, next, end, maxCount);	// forced expose this entry	
		else
			FastCreateSolutions(next, end, maxCount);			// continue until we are done or abort 
	}
	else
		fastConflicts++;
	list->tried = -1;
	for (int n = 0; n < list->numNeighbors; n++)				// undo expose
	{
		list->neighbors[n]->unexposed++;
	}
}


void FastCreateSolutions(List * list, List * end, int maxCount)
{
	do
	{
		if (list->tried < 0										// not tried yet
		&& list->numSimilar == 0)								// not a similar entry
		{
			if (fastMinesUsed < maxCount)						// no more mines allowed
				FastTryMine(list, list->next, end, maxCount);	// try a mine here 
			FastTryExpose(list, list->next, end, maxCount);		// try expose here 
			return;
		}
		list = list->next;
	} while (list != end);
	FastCountCompletedSolutions(list->list);
}


void FastCalculateSolutions(SuperLoc * superLoc)
{
	List * list = superLoc->list;
	int newPerimeter = 0;
	bool adjacent = false;
	List * adjacentList = NULL;
	if (list == NULL)
	{
		adjacent = true;
		newPerimeter++;
		adjacentList = FastCreateNewPerimeter(superLoc);		// create list entry for the adjacent 
		list = superLoc->list;
		list->list = list->next->list;							// use correct list
		list->prob = nonPerimeterProb;
		list->minValue = mines[superLoc->y][superLoc->x];
		list->maxValue = list->minValue + adjacentCount[superLoc->y][superLoc->x];
	}
	for (int i = 0; i < lists; i++)
		excludeLists[i] = false;
	excludeLists[list->list] = true;							// do not count this list in solutions
	excludeLists[lists] = true;									// do not merge with new entries
	// add this entry as a neighbor and adjust its neighbors
	fastMinesUsed = 0;
	int oldNeighborNext = neighborNext;
	Neighbor * neighbor = FastCreateNeighbor(list);				// create a neighbor for current entry
	newPerimeter += FastAddPerimeter(list, neighbor);			// add new perimeter if necessary
	FastMergeLists(list);										// merge lists if necessary
	FastUpdateNeighbors(list, neighbor);						// add new neighbor to any other perimeter
	neighbor->needed = 10;										// to prevent us from filling other neighbors
	ForceMoves(list, neighbor);									// make any forced moves
	int neededAdjust = neighbor->needed - 10;					// new adjustment on needed for forced moves
	nonPerimeter -= newPerimeter;
	// find new max mines allowed
	int maxMines = mineCount;
	for (int i = 0; i < lists; i++)
	{
		if (excludeLists[i])
			continue;
		maxMines -= listMinMines[i];
	}
	superLoc->conflicts = 0;
	superLoc->numResults = 0;
	superLoc->solutions = 0;
	superLoc->progress = 0.0;
	superLoc->exposed = 1.0 - list->prob;						// probability of a clear
	int min = list->minValue;
	int max = list->maxValue + adjacentCount[list->y][list->x];
	U64 links0 = 0xffffffffffffffff;
	U64 links1 = 0xffffffffffffffff;
	for (int val = min; val <= max; val++)
	{
		// clear solution counts for the new list
		for (int i = 0; i < MAX_COUNTS; i++)
			listSolutions[lists][i] = 0;
		listMinMines[lists] = MAX_MINES;
		listMaxMines[lists] = 0;
		// clear fields
		for (List * list1 = list->next; list1 != list; list1 = list1->next)
			list1->clear = true;
		neighbor->needed = neededAdjust + val - mines[list->y][list->x];	// needed mines
		// search for solutions
		fastConflicts = 0;
		fastCompletedSolutions = 0;
		FastCreateSolutions(list->next, list, maxMines);		// create solutions
		superLoc->conflicts += fastConflicts;
		superLoc->solutions += fastCompletedSolutions;
		if (fastCompletedSolutions == 0)						// no solutions
		{
			if (saveSuperLocResults > 1							// list all solutions
			&& superLocResults != NULL)
			{
				fprintf(superLocResults, "%d,%d=%d\tsolutions=0 conflicts=%d\n",
					superLoc->x, superLoc->y, val, superLoc->conflicts);
			}
			if (adjacent)										// we may need to try higher values
				continue;
			if (val == min)										// merged lists will cause val=min to fail
			{
				max++;											// increase max too for merged lists
				continue;
			}
			if (superLoc->progress == 0.0)						// no solutions yet
				continue;
			break;
		}
		// get exposed count from number of clear entries
		int exposed = 0;
		U64 clears0 = 0;
		U64 clears1 = 0;
		for (List * list1 = list->next; list1 != list; list1 = list1->next)
		{
			if (list1->clear)
			{
				exposed++;										// jar uses only this data
				if (useNewSuperLocMethod == 0)					// old method
				{	
					// BUG: the old method incorrectly assumed all entries are on the same list
					// but this is not the case
					// The following is incorrect but generates better results!!
					// It should be: 
					//  S64 n = list1 - &listArray[listStart[list->list]];
					// but then I need to change code elsewhere to match
					S64 n = list1 - &listArray[listStart[list1->list]];
					while (n < 0)
						n += 64;
					while (n >= 64)								// use wraparound and hope we do not collide
						n -= 64;
					clears0 |= ((U64)1 << n);
				}
				else if (useNewSuperLocMethod != 0)				// new or jar method
				{
					if (list1->list < lists)					// not a new perimeter entry
					{
						S64 n = list1 - &listArray[0];
						while (n >= 128)						// for the one in a million where we have more than 128 listArray entries
							n -= 128;
						if (n >= 64)
							clears1 |= ((U64)1 << (n - 64));
						else
							clears0 |= ((U64)1 << n);
					}
					else
					{
						// new added perimeter may be different for each linked entry
						// but not necessarily so keep list of added perimeter
						// which exists for the minimum value only
						superLoc->edges[superLoc->numEdges][0] = list1->x;
						superLoc->edges[superLoc->numEdges][1] = list1->y;
						superLoc->numEdges++;
					}
				}
			}
		}
		links0 &= clears0;										// mask of all clears gives us linked items
		links1 &= clears1;										// mask of all clears gives us linked items
		if (exposed == 0)
		{
			if (saveSuperLocResults > 1							// list all solutions
			&& superLocResults != NULL)
			{
				fprintf(superLocResults, "%d,%d=%d\tclears=0 solutions=%d conflicts=%d\n",
					superLoc->x, superLoc->y, val, 
					superLoc->solutions, superLoc->conflicts);
			}
			break;
		}
		double newTotalSolutions = FastTotalSolutions();
		double odds = newTotalSolutions / totalSolutions;		// odds we succeeded
		superLoc->progress += odds;
		superLoc->exposed += odds * exposed;
		superLoc->linkMask[superLoc->numResults][0] = clears0;
		superLoc->linkMask[superLoc->numResults][1] = clears1;
		superLoc->results[superLoc->numResults].odds = odds;
		superLoc->results[superLoc->numResults].exposed = exposed;
		superLoc->numResults++;
		if (saveSuperLocResults > 1								// list all solutions
		&& superLocResults != NULL)
		{
			fprintf(superLocResults, "%d,%d=%d\tprogress=%6.3f%% clears=%d solutions=%d conflicts=%d 0x%016llX 0x%016llX\n",
				superLoc->x, superLoc->y, val, odds * 100.0, exposed,
				superLoc->solutions, superLoc->conflicts, clears1, clears0);
		}
		if (adjacent == true)									// no need for higher values
			break;
	}
	FastRemovePerimeter(list, newPerimeter);
	nonPerimeter += newPerimeter;
	if (adjacentList != NULL)
		superLoc->list = NULL;
	FastRemoveNeighbor(list, oldNeighborNext);
	ForceUndoMoves();
	// NOTE: It is possible one forces a link to another but not vice versa
	// See superTesting/super17.txt or superTesting/super19.txt
	if ((links0 != 0 || links1 != 0)
	&& superLoc->list != NULL)
	{
		List * list0;
		if (useNewSuperLocMethod == 0)							// old method
			list0 = &listArray[listStart[superLoc->list->list]];
		else
			list0 = &listArray[0];
		int n;
		for (n = 0; links0 != 0; n++, links0 >>= 1)
		{
			if ((links0 & 1) != 0)
			{
				List * list1 = list0 + n;
				AddLinks(superLoc, list1);
			}
		}
		for (n = 64; links1 != 0; n++, links1 >>= 1)
		{
			if ((links1 & 1) != 0)
			{
				List * list1 = list0 + n;
				AddLinks(superLoc, list1);
			}
		}
	}
}


//
//	Jar method uses what the jar program does
//	This duplicates the count of some entries
//
double FastCalculateLinkedClears(SuperLoc * superLoc, int link, double odds, int expose)
{
	double results = 0.0;
	double newOdds;
	int newExpose;
	SuperLoc * superLoc2 = SuperEntry(superLoc->links[link]);
	for (int i = 0; i < superLoc2->numResults; i++)
	{
		newOdds = odds * superLoc2->results[i].odds / superLoc2->progress;
		newExpose = expose + superLoc2->results[i].exposed - superLoc->numLinks;
		if (link + 1 < superLoc->numLinks)
			results += FastCalculateLinkedClears(superLoc, link + 1, newOdds, newExpose);
		else
			results += newOdds * newExpose;
	}
	return results;
}


int numEdges;
char edges[32][2];

void AddEdges(SuperLoc * superLoc)
{
	for (int i = 0; i < superLoc->numEdges; i++)
	{
		int n;
		for (n = 0; n < numEdges; n++)
		{
			if (edges[n][0] == superLoc->edges[i][0]
			&& edges[n][1] == superLoc->edges[i][1])
				goto SKIP;
		}
		edges[numEdges][0] = superLoc->edges[i][0];
		edges[numEdges][1] = superLoc->edges[i][1];
		numEdges++;
SKIP:	;
	}
}


int CountMask(U64 mask)
{
	int count = 0;
	while (mask != 0)
	{
		if ((mask & 1) != 0)
			count++;
		mask >>= 1;
	}
	return count;
}


//
// new method reduces clear counts by not counting duplicated entries in exposed counts 
// old method estimates duplicated entries especially new created perimeter
//
double FastCalculateLinkedClears(SuperLoc * superLoc, int link, double odds, U64 linkMask0, U64 linkMask1)
{
	double results = 0.0;
	double newOdds;
	SuperLoc * superLoc2 = SuperEntry(superLoc->links[link]);
	int oldEdges = numEdges;
	for (int i = 0; i < superLoc2->numResults; i++)
	{
		if (useNewSuperLocMethod > 0				// new method counts edges
		&&  i == 0)
			AddEdges(superLoc2);
		else
			numEdges = oldEdges;
		newOdds = odds * superLoc2->results[i].odds / superLoc2->progress;
		if (link + 1 < superLoc->numLinks)
			results += FastCalculateLinkedClears(superLoc, link + 1, newOdds, 
				linkMask0 | superLoc2->linkMask[i][0],
				linkMask1 | superLoc2->linkMask[i][1]);
		else
		{
			U64 mask0 = linkMask0 | superLoc2->linkMask[i][0];
			U64 mask1 = linkMask1 | superLoc2->linkMask[i][1];
			int count = CountMask(mask0);
			count += CountMask(mask1);
			results += newOdds * (count + numEdges);
		}
	}
	return results;
}


void FastCalculateLinkedClears(SuperLoc * superLoc)
{
	double exposed = 0;
	if (useNewSuperLocMethod < 0)						// jar method
	{
		for (int i = 0; i < superLoc->numResults; i++)
		{
			exposed += FastCalculateLinkedClears(superLoc, 0, superLoc->results[i].odds, superLoc->results[i].exposed);
		}
		superLoc->exposed = exposed + superLoc->prob;
	}
	else
	{
		for (int i = 0; i < superLoc->numResults; i++)
		{
			numEdges = 0;
			if (useNewSuperLocMethod > 0				// new method counts edges
			&& i == 0)
				AddEdges(superLoc);
			exposed += FastCalculateLinkedClears(superLoc, 0, superLoc->results[i].odds, superLoc->linkMask[i][0], superLoc->linkMask[i][1]);
		}
		superLoc->exposed = exposed + superLoc->prob;
	}
}

