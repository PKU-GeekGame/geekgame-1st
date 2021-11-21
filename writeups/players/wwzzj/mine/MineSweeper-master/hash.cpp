#include "stdafx.h"

#define HASH_TABLE		65536				// number of hash table keys


struct HashData
{
	int			wins;						// probability of winning
	int			best;						// best move
	char		string[END_PLAY_COUNT + 1];	// current puzzle state
	HashData * next;						// next entry in list
};

HashData	*	HashTable[HASH_TABLE];		// hash table
HashData		HashArray[HASH_ARRAY];		// hash data
HashData	*	HashNextFree;				// next free entry in HashArray[]

int				HashHits;					// number of hits
int				HashUsed;					// number of entries used


void HashInit()
{
	HashHits = 0;
	HashUsed = 0;
	for (int i = 0; i < HASH_TABLE; i++)
	{
		HashTable[i] = NULL;
	}
	HashNextFree = NULL;
	for (int i = HASH_ARRAY - 1; i >= 0; i--)
	{
		HashArray[i].next = HashNextFree;
		HashNextFree = &HashArray[i];
	}
}


void HashClear()
{
	HashHits = 0;
	HashUsed = 0;
	for (int i = 0; i < HASH_TABLE; i++)
	{
		HashData * ptr = HashTable[i];
		if (ptr == NULL)
			continue;
		while (ptr->next != NULL)				// go to the end of the data
		{
			ptr = ptr->next;
		} 
		ptr->next = HashNextFree;				// link this list with the free entries
		HashNextFree = HashTable[i];			// new start of the free entries
		HashTable[i] = NULL;
	}
}


UINT HashKey(char * s)
{
	UINT key;
	for (key = 0; *s != '\0'; s++)
		key = *s + 31 * key;
	return key % HASH_TABLE;
}


bool HashGet(int & key, char * string, int & best, int & wins)
{
	key = (int)HashKey(string);
	HashData * data = HashTable[key];
	while (data != NULL)
	{
		if (strcmp(string, data->string) == 0)
		{
			best = data->best;
			wins = data->wins;
			HashHits++;
			if (maxHashHits < HashHits)
				maxHashHits = HashHits;
			return true;
		}
		data = data->next;
	}
	return false;
}


bool HashPut(int key, char * string, int best, int wins)
{
	if (HashNextFree == NULL)					// no free entries
	{
		if (testAllResults != NULL)
		{
			fprintf(testAllResults, "***** ERROR: Puzzle=%d hash table full (%d allocated)\n", testing, HASH_ARRAY);
		}
		return false;
	}
	if (key < 0)								// no key provided so compute it
		key = (int)HashKey(string);
	HashData * data = HashNextFree;
	HashNextFree = HashNextFree->next;
	data->next = HashTable[key];
	HashTable[key] = data;
	strcpy(data->string, string);
	data->best = best;
	data->wins = wins;
	HashUsed++;
	if (maxHashUsed < HashUsed)
		maxHashUsed = HashUsed;
	return true;
}


bool HashReplace(int key, char * string, int best, int wins)
{
	if (key < 0)								// no key provided so compute it
		key = (int)HashKey(string);
	HashData * data = HashTable[key];
	while (data != NULL)
	{
		if (strcmp(string, data->string) == 0)
		{
			data->best = best;
			data->wins = wins;
			HashHits++;
			if (maxHashHits < HashHits)
				maxHashHits = HashHits;
			return true;
		}
		if (data->next == NULL)
			break;
		data = data->next;
	}
	if (HashNextFree == NULL)					// no free entries
		return false;
	if (data == NULL)							// hash table has no data yet
	{
		HashTable[key] = HashNextFree;
		data = HashNextFree;
	}
	else
	{
		data->next = HashNextFree;				// add to end of list
		data = data->next;
	}
	HashNextFree = HashNextFree->next;
	strcpy(data->string, string);
	data->best = best;
	data->wins = wins;
	data->next = NULL;
	HashUsed++;
	if (maxHashUsed < HashUsed)
		maxHashUsed = HashUsed;
	return true;
}


void HashStats(int & used, int & hits)
{
	used = HashUsed;
	hits = HashHits;
}

int CompareHash(const void * e1, const void * e2)
{
	HashData * d1 = (HashData *)e1;
	HashData * d2 = (HashData *)e2;
	return strcmp(d1->string, d2->string);
}


void HashDump(FILE * file)
{
	qsort(HashArray, HashUsed, sizeof(HashData), CompareHash);
	fprintf(file, "#############################################\n");
	for (int i = 0; i < HashUsed; i++)
	{
		HashData * data = &HashArray[i];
		fprintf(file, "%s wins=%d best=%d\n", data->string, data->wins, data->best);
	}
	HashInit();
}
