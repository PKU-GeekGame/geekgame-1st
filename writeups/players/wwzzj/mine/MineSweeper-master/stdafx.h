// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN							// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <shlobj.h>									// to use SHGetKnownFolderPath
#include <commdlg.h>								// required for load and save files
#include <stdio.h>									// for fopen and sprintf
#include <time.h>									// for time functions
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "resource.h"
#include "rand.h"


// Super loc method tests
#define SUPERLOC_NEW_METHOD_TEST 1					// 1=no test, 0=test new vs old method, -1=test new vs jar method, -2= test jar vs old method

// save file parameters
#define	SHOW_GAMETREE_TIMES false					// save all game tree time and parameters to review gametree

// build parameters
#define SPEED				10						// number of milliseconds between puzzles for solving 
#define WARNING_TIME		0.5						// if this time is exceeded issue a warning
#define WARNING_SEQUENCE	70						// warn us when we get sequences longer than this
#define	WARNING_SOLUTIONS	10000					// warning we we create this many solutions or more
#define MARGIN_OF_ERROR		1.96					// 95% margin of error (2.33 for 98%, 2.58 for 99%)

// puzzle characteristics
#define MAX_WIDTH			30
#define MAX_HEIGHT			16
#define MAX_MINES			99
#define MAX_NEIGHBORS		8
#define	SCREEN_WIDTH		(MAX_WIDTH * TILE_WIDTH)
#define	SCREEN_HEIGHT		(MAX_HEIGHT * TILE_HEIGHT)
#define EXPOSE_MINE			9						// code in exposed[] for mine
#define EXPOSE_EXPLODE		10						// code in exposed[] for expoding mine

// perimeter build paaramters
#define	MAX_LISTS			20						// number of lists for the perimeter
#define MAX_COUNTS			16						// how many different counts for listArray[] counts
#define MAX_LIST_LENGTH		192						// max length of listArray 
#define MAX_NEIGHBORS_LIST	192						// max length of neighbor array

// guessing thresholds
#define GUESS_THRESHOLD		.96						// default threshold for super locations
#define CLEAR_MULTIPLIER	1.0						// defualt for super location clears better than best choice per 1% difference

// super location parameters
#define MAX_SUPERLOCATIONS	48						// max number of super location candidates
#define END_PLAY_COUNT		64						// max end play count
#define MAX_LINKS			8						// max linked clears for each super location

// gametree paramaters
#define MAX_GAMETREELOCATIONS	32					// max number of gametree best candidates
#define MAX_DEPTH			24						// max gametree search depth
#define MAX_SOLUTIONS		4000					// max number of solutions for game tree processing
#define DEFAULT_SOLUTIONS	1000					// default number of solutions for game tree processing
#define	SMALL_SOLUTIONS		400						// number of solutions for stats purposes to see if most are small
#define MAX_SOLUTION_VALUES	(MAX_SOLUTIONS * END_PLAY_COUNT + 100)	// max number of entries for saved solutions in game tree 
#define HASH_ARRAY			500000

// default starting positions
#define start9x9X			2
#define start9x9Y			2
#define start16x16X			2
#define start16x16Y			2
#define start30x16X			3
#define start30x16Y			3

enum RULES
{
	RULE_1 = 0,										// add mines
	RULE_2,											// clear or expose entries
	DEDUCE,
	GUESS_0,
	GAMETREE,
	SUPERLOC,
	GUESS_1,
	GUESS_2,
	MAX_RULES										// number of rules to use to solve puzzle
};							


struct Neighbor
{
	int		x, y;							// position
	int		needed;							// number of mines needed
	int		unexposed;						// number of unexposed tiles around
	int		count;							// entries used below
	struct List * entries[MAX_NEIGHBORS];	// listArray entries using this neighbor
};

struct Link
{
	int		index;							// index into solutionValues (-1 if none)
	Link *	next;							// next link entry (NULL if none)
};

struct Solutions
{
	int		index;							// index into listArray for these solutions
	int		total;							// total number of solutions
	int		max;							// largest set of solutions below
	int		min;							// number of solutions that will result in immediate clears
	int		order;							// to stablize qsort
	bool	dead;							// if this entry is dead 
	Link	solutions[MAX_NEIGHBORS + 1];	// count and links to solutions for each count of neighbor mines
};

struct List
{
	int		x, y;							// position
	int		list;							// list number that this belongs to (before end play combines them)
	int		merged;							// list number if this merges with another list
	int		tried;							// < 0 if not tried yet, 0 if exposed, 1 if mine (see CreateSolutions())
	bool	dead;							// if this entry is dead 
	bool	clear;							// this location is cleared (see fastSolve.cpp)
	int		adjacentLists;					// number of adjacentlists
	int		minValue;						// min value for this location
	int		maxValue;						// max values for this location
	int		numNeighbors;					// number of neighbors below
	int		numSimilar;						// number of similar entries
	struct List * similar;					// first list entries with the same neighbors
	struct List * next;						// next entry in the list (loops back to start)
	// counts[] required to be accurate for prob calculation
	int		counts[MAX_COUNTS];				// number of possible counts of mines (index using listMinMines[list])
	double	prob;							// probability of mine here 
	Neighbor * neighbors[MAX_NEIGHBORS];	// pointer into neighbor array
};


struct Result
{
	double	odds;							
	int		exposed;
};

struct SuperLoc
{
	List *	list;							// list entry (NULL for off perimeter entries)
	int		x, y;							// position
	int		conflicts;						// number of conflicts found when searching for solutions
	int		solutions;						// number of solutions found
	int		order;							// to stablize qsort to give the same results
	int		numResults;						// number of results
	int		numLinks;						// number of links below
	int		numEdges;						// number of new edges created used to calculate the clears in linked lists
	Result	results[MAX_NEIGHBORS];			// results for each value (list minValue to list maxValue)
	List *	links[MAX_LINKS];				// linked clears
	// NOTE linkMask[] assumes the max list length is 128 or less
	U64		linkMask[MAX_NEIGHBORS][2];		// mask of list indices that are linked for each result
	char	edges[MAX_NEIGHBORS][2];		// x,y values for new edges created (for minValue only)
	double	prob;							// probability of a win
	double	progress;						// probability of progress
	double	exposed;						// how many free entries do we expose if we play here
	double	time;							// time top process this super location
};

struct GameTree
{
	int		x, y;							// position of gametree entry
};


// hash.cpp definitions
void HashInit();
void HashClear();
bool HashGet(int & key, char * string, int & best, int & wins);
bool HashPut(int key, char * string, int best, int wins);
bool HashReplace(int key, char * string, int best, int wins);
void HashStats(int & used, int & hits);
void HashDump(FILE * file);

// file.cpp definitions
#define MAX_WINS_WITH_GUESSES	11
extern int		maxHashUsed;
extern int		maxHashHits;
extern int		winsWithGuesses[MAX_WINS_WITH_GUESSES];
extern double	winProb;								// sum of prob for all wins
extern double	lossProb;
extern double	guess0Prob;								// sum of all guess0 prob 
extern double	guess1Prob;								// sum of all guess1 prob 
extern double	guess2Prob;								// sum of all guess2 prob 
extern double	gameTreeProb;							// sum of all game tree prob 
extern double	superLocProb;							// sum of all super lcoation prob
extern int		guessesPerWin;							// number of guesses per win
extern int		guessesPerLoss;
extern int		winTotal;								// total number of wins
extern int		testTotal;								// total number of tests (1-100)
extern int		winAllTotal;							// total number of wins over all seeds
extern int		guess0Failed;
extern int		guess1Failed;
extern int		guess2Failed;
extern int		gameTreeFailed;
extern int		superLocFailed;
extern int		totalWins;								// number of wins and tests
extern int		totalTests;								// total tests completed (not including testTotal)
extern int		totalGuess0;							// total guess0, guess1, guess2
extern int		totalGuess1;							
extern int		totalGuess2;
extern int		totalGameTree;
extern int		totalSuperLoc;
extern int		totalGuess0Failed;						// number of failed guesses
extern int		totalGuess1Failed;						
extern int		totalGuess2Failed;
extern int		totalGameTreeFailed;
extern int		totalSuperLocFailed;
extern int		totalGuessesPerWin;						// number of guesses per win
extern int		totalGuessesPerLoss;
extern int		totalWinsWithGuesses[MAX_WINS_WITH_GUESSES];
extern int		totalMaxHashUsed;
extern int		totalMaxHashHits;
extern double	totalWinProb;							// sum of prob for all wins
extern double	totalLossProb;
extern double	totalGuess0Prob;						// sum of all guess0 prob 
extern double	totalGuess1Prob;						// sum of all guess1 prob 
extern double	totalGuess2Prob;						// sum of all guess2 prob
extern double	totalGameTreeProb;						// sum of all gametree prob
extern double	totalSuperLocProb;						// sum of all super loc prob
void ClearTotalStats();									// clear total stats
void SaveTotalResults(FILE * file);						// save total results
void SaveResults(FILE * file);							// save results of the puzzle including errors
void SaveSetResults(FILE * file, int set);				// save results of the puzzle including errors
void ShowTestResults(FILE * file, int tests, U64 seed);	// display stats on remaining count and mines (min, max, avg)
bool SavePuzzle(char * fname, bool saveProb = true);
bool LoadPuzzle(char * fname);
bool SaveFileName(HWND hwnd, char * fname, char * filterString, char * defaultExt);
bool LoadFileName(HWND hwnd, char * fname, char * filterString, char * defaultExt);
bool SaveGameFile();
bool LoadGameFile();

// game.c definitions
extern int	puzzle[MAX_HEIGHT][MAX_WIDTH];			// 1 for mine and 0 otherwise
extern int	exposed[MAX_HEIGHT][MAX_WIDTH];			// -1 if not exposed, 0-8 for numbers, 9 for mine
extern int	lastExposed[MAX_HEIGHT][MAX_WIDTH];		// used for backup
extern int	last[MAX_HEIGHT][MAX_WIDTH];			// used for backing up last puzzle
extern bool	started;								// puzzle started
extern bool	solving;								// puzzle solving in progress
extern bool	automatic;								// automatic solving in progress
extern bool	gameOver;								// true when game is over
extern bool	backup;									// if backup available
extern bool	cheating;								// show mines when cheating
extern int	rulesCount[MAX_RULES];					// count of rules used
extern int	rulesCountTotal[MAX_RULES];				// total count of rules used for all puzzles tested
extern int	guesses;								// number of guesses so far this puzzle
extern int	lastRuleUsed;							// last rule used
extern int	mineCount;								// mines left to find
extern int	remainingCount;							// number left to expose
extern int	lastMineCount;							// mines left to find on backup
extern int	lastRemainingCount;						// number left to expose on backup
extern int	maxSolutions;							// default value for gametree solutions 
extern double	prob;								// current probability of getting this right
extern double	lastProb;							// last value of prob
extern int	unexposed[MAX_HEIGHT][MAX_WIDTH];
extern int	mines[MAX_HEIGHT][MAX_WIDTH];
extern int	needed[MAX_HEIGHT][MAX_WIDTH];
extern int	neighbors[MAX_HEIGHT][MAX_WIDTH];
void	TimeInit();
void	TimeStart();
double	TimeEnd();
void	TimeStart2();
double	TimeEnd2();
void	TimeStartOverall();
double	TimeEndOverall();
void Count(int x, int y, int & unexposed, int & mines, int & needed, int & neighbors);
void SaveBackUp();									// save move for backup
bool BackUp();										// back up to the last move
int CountMines(int x, int y);						// count mines around (x,y) in puzzle[][]
void AddMine(int x, int y);							// add mine at (x,y)
void Expose(int x, int y);							// expose area at (x,y)
void NewGame(int x, int y);							// start new game at (x,y)
bool MakeMove();									// try to find one move to make

// fastSolve.cpp definitions
void FastCalculateSolutions(SuperLoc * superLoc);
void FastCalculateLinkedClears(SuperLoc * superLoc);

// tank.cpp definitions
extern int	bestx, besty;							// best move
extern int nonPerimeter;							// number of non-perimeter entries (not in any sequence)
extern double nonPerimeterProb;						// non-perimeter prob
extern int conflictsInSolutions;					// number of times we found conflicts when creating solutions
extern int lists;									// number of lists found
extern int listNext;								// next entry into the listArray
extern List	listArray[MAX_LIST_LENGTH];				// list entries
extern int listUsed[MAX_HEIGHT][MAX_WIDTH];			// index into listArray (-1 if none)
extern int listMinMines[MAX_LISTS];					// min number of mines used
extern int listStart[MAX_LISTS];					// starting index for a list 
extern int listEnd[MAX_LISTS];						// last entry for a list
extern int listMaxMines[MAX_LISTS];					// max number of mines used
extern int listSolutions[MAX_LISTS][MAX_COUNTS];	// number of possible choices for each list and each count of mines
extern int superNext;								// number of super locations
extern double	bestSuperLocProgress;				// best progress we found
extern SuperLoc *	bestSuperLoc;					// best super location found so far (maybe NULL)
extern SuperLoc	SuperLocArray[MAX_SUPERLOCATIONS];
extern int gameTreeNext;							// number of game tree locations
extern GameTree	GameTreeArray[MAX_GAMETREELOCATIONS];
extern int maxCounts;								// max counts for solutions (see MAX_COUNTS)
extern int maxLists;								// max number of sequence lists (see MAX_LISTS)
extern int maxPerimeter;							// max number of entries in listArray (see MAX_LIST_LENGTH)
extern int maxSinglePerimeter;						// longest single perimeter
extern int maxNeighbors;							// max number of entries in neighborArray 
extern int numEndPlays;								// number of times we use end play
extern int numMissedEndPlays;						// number of missed end plays with <= maxSolutions
extern int maxEndPlayLength;						// length of max end play
extern int numEndPlaySmall;							// count small end plays (< 400)
extern int numEndPlayLarge;							// count large end plays (> maxSolutions)
extern int maxListSolutions;						// max number of perimeter solutions
extern int maxCompletedSolutions;					// max number of perimeter solutions completed
extern int gametreeUsed;							// number of times we used a gametree
extern int gametreeUsedEarly;						// number of times we used a gametree early
extern int gametreeEntriesUsed;						// max number of entries we used for a gametree
extern int gametreeHelped;							// number of times gametree improved odds
extern int gametreeLinksNeeded;						// number of game tree links needed 
extern int gametreeMaxDepth;						// max depth of the gamet tree search
extern int superLocMaxCandidates;					// max candidates found
extern int superLocMultiplierUsed;
extern int superLocMultiplierImproved;
extern int superLocMultiplierFailed;
extern int superLocMultiplyUsed;
extern int totalMaxCounts;							// max counts for solutions (see MAX_COUNTS)
extern int totalMaxLists;							// max number of sequence lists (see MAX_LISTS)
extern int totalMaxPerimeter;						// max number of entries in listArray (see MAX_LIST_LENGTH)
extern int totalMaxSinglePerimeter;					// longest single perimeter
extern int totalMaxNeighbors;						// max number of entries in neighborArray 
extern int totalNumMissedEndPlays;					// number of missed end plays with <= maxSolutions
extern int totalMaxEndPlayLength;					// length of max end play
extern int totalNumEndPlays;						// number of times we use end play
extern int totalNumEndPlaySmall;					// count small end plays (< 400)
extern int totalNumEndPlayLarge;					// count large end plays (> maxSolutions)
extern int totalMaxListSolutions;					// max number of perimeter solutions
extern int totalMaxCompletedSolutions;				// max number of perimeter solutions completed
extern int totalGametreeUsed;						// number of times we used a gametree
extern int totalGametreeUsedEarly;					// number of times we used a gametree early
extern int totalGametreeEntriesUsed;				// max number of entries we used for a gametree
extern int totalGametreeHelped;						// number of times gametree improved odds
extern int totalGametreeLinksNeeded;				// number of game tree links needed 
extern int totalGametreeMaxDepth;					// max depth of the gamet tree search
extern int totalSuperLocMaxCandidates;				// max candidates found
extern int totalSuperLocMultiplierUsed;
extern int totalSuperLocMultiplierImproved;
extern int totalSuperLocMultiplierFailed;
extern int totalSuperLocMultiplyUsed;
extern double bestProb;								// best probability for the perimeter
extern double totalSolutions;						// total number of solutions to the puzzle
extern int	Combinations[MAX_NEIGHBORS + 1][MAX_NEIGHBORS + 2];
extern double Comb(int n, int m);
extern int	neighborNext;							// next neighbor entry to use
extern Neighbor neighborArray[MAX_NEIGHBORS_LIST];	// list of all neighbors
extern int	adjacentCount[MAX_HEIGHT][MAX_WIDTH];	// count of adjacent to perimeter or adjacent to free entries
int GetSolutionCount();								// number of solutions found
void UpdatePerimeterStats();						// update stats from the perimeter (after Guess0)
void CreatePerimeterLists();						// create lists for the perimeter
void CalculateProb();								// calculate probability for each entry and set bestProb
int CreatePerimeterSolutions(int start, int end, int maxMines, bool saveSolutions);	// find all possible solutions on the lists	
bool FindEasySolution();							// find obvious move
bool PickGuess0();									// make any guess0
bool UseGameTree();									// gametree processing
bool UseSuperLoc();									// try super locations
bool UseGuess1();									// make this guess

// MineSweeper.cpp definitions
extern	int		width, height, minesStart;			// size of puzzle and number of mines to start
extern	char	testFilePath[MAX_PATH];				// path if testing
extern  char 	prefixString[MAX_PATH];				// path for highest folder if testing
extern	double	guessThreshold;						// threshold for guessing
extern	double	clearMultiplier;					// super location multiplier for secondary entries
extern	bool	topToBottomPriority;				// guess priority from top to bottom vs left to right first
extern	bool	saveSetResults;						// save results for each set of 100 puzzles
extern	bool	saveTiming;							// save puzzles that take too long when testing
extern	bool	saveInteresting;					// save interestig puzzles when testing
extern	bool	saveGuess0Fails;					// save guess0 fails
extern	bool	saveGameTreeFails;					// save gametree fails
extern	bool	saveSuperLocFails;					// save guess1 and superLoc fails
extern	bool	saveSuperLocMultiplierChanges;		// save puzzles when superLoc threshold is used
extern	int		saveGameTreeResults;				// save results of the current gametree search
extern	int		saveSuperLocResults;				// save results of the current super location search
extern	int		useNewSuperLocMethod;				// -1=jar method, 0=v1.2c method, 1=new method for calculating superLoc clears
extern	bool	useSuperLocMultiplier;				// use multiply method to use second best superLoc if clears is better
extern	U64		saveSeed;							// save seed used for testing
extern	U64		startSeed;							// starting seed to use for starting multiple tests
extern	int		testing;							// puzzle testing remaining
extern	int		testEndPlay;						// what end play (index) are we testing
extern	int		startX, startY;						// starting location for new games
extern	FILE *	testResults;						// file for test results
extern	FILE *	testAllResults;						// file for all test results
extern	FILE *	gametreeResults;					// list game tree results
extern	FILE *	superLocResults;					// list superLoc results
extern	FILE *	gametreeTimes;						// list game tree solution times
