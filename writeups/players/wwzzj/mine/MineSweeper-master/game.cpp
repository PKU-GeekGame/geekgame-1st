#include "stdafx.h"

int		puzzle[MAX_HEIGHT][MAX_WIDTH];			// 1 for mine and 0 otherwise
int		exposed[MAX_HEIGHT][MAX_WIDTH];			// -1 if not exposed, 0-8 for numbers, EXPOSE_MINE for mine...
int		lastExposed[MAX_HEIGHT][MAX_WIDTH];		// used for backup
int		rulesCount[MAX_RULES];					// count of rules used
int		rulesCountTotal[MAX_RULES];				// total count of rules used for all puzzles tested
int		guesses;								// number of guesses so far this puzzle
int		lastRuleUsed;							// last rule used
bool	started;								// puzzle started
bool	solving;								// puzzle solving in progress
bool	automatic;								// automatic solving in progress
bool	gameOver;								// true when mine has been hit
bool	backup;									// if backup available
bool	cheating;								// show mines when cheating
int		mineCount;								// mines left to find
int		remainingCount;							// number left to expose
int		lastMineCount;							// mines left to find on backup
int		lastRemainingCount;						// number left to expose on backup
int		maxSolutions;							// default value for gametree solutions 

double	prob;									// current probability of getting this right
double	lastProb;								// last value of prob

int		unexposed[MAX_HEIGHT][MAX_WIDTH];		// number of unexposed entries next to this location
int		mines[MAX_HEIGHT][MAX_WIDTH];			// number of mines next to this location
int		needed[MAX_HEIGHT][MAX_WIDTH];			// how many mines are needed at this location
int		neighbors[MAX_HEIGHT][MAX_WIDTH];		// how many exposed (not a mine) neighbors next to this location


// get the high resolution counter's accuracy	
static	LARGE_INTEGER	ticksPerSecond;
static	LARGE_INTEGER	tickStart;				// start time
static	LARGE_INTEGER	tickStart2;				// start time
static	LARGE_INTEGER	tickStartOverall;		// start overall time
																					
void	TimeInit()
{
	QueryPerformanceFrequency(&ticksPerSecond);
}

void	TimeStart()
{
	QueryPerformanceCounter(&tickStart);		// start timer
}

void	TimeStart2()
{
	QueryPerformanceCounter(&tickStart2);		// start timer
}

void	TimeStartOverall()
{
	QueryPerformanceCounter(&tickStartOverall);	// start timer
}

double	TimeEnd()
{
	LARGE_INTEGER	tickEnd;					// end time
	QueryPerformanceCounter(&tickEnd);			// end timer
	return (double)(tickEnd.QuadPart - tickStart.QuadPart) / (double)ticksPerSecond.QuadPart;
}

double	TimeEnd2()
{
	LARGE_INTEGER	tickEnd;					// end time
	QueryPerformanceCounter(&tickEnd);			// end timer
	return (double)(tickEnd.QuadPart - tickStart2.QuadPart) / (double)ticksPerSecond.QuadPart;
}

double	TimeEndOverall()
{
	LARGE_INTEGER	tickEnd;					// end time
	QueryPerformanceCounter(&tickEnd);			// end timer
	return (double)(tickEnd.QuadPart - tickStartOverall.QuadPart) / (double)ticksPerSecond.QuadPart;
}


void SaveBackUp()
{
	// save current board
	backup = true;
	listNext = 0;								// do not display any similar or dead places
	lastProb = prob;
	lastMineCount = mineCount;
	lastRemainingCount = remainingCount;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			lastExposed[y][x] = exposed[y][x];
		}
	}
}


bool BackUp()
{
	bestx = -1;									// do not display best move
	listNext = 0;								// disable display of dead and similar
	if (backup == false)
		return false;
	solving = false;
	automatic = false;
	gameOver = false;
	backup = false;
	mineCount = lastMineCount;
	remainingCount = lastRemainingCount;
	lastRuleUsed = -1;
	prob = lastProb;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			exposed[y][x] = lastExposed[y][x];
		}
	}
	return true;
}


int CountMines(int x, int y)
{
	if (exposed[y][x] >= 0)
		return exposed[y][x];
	int count = 0;
	count += (x > 0 && y > 0 && puzzle[y - 1][x - 1] != 0 ? 1 : 0);
	count += (y > 0 && puzzle[y - 1][x] != 0 ? 1 : 0);
	count += (x < width-1 && y > 0 && puzzle[y - 1][x + 1] != 0 ? 1 : 0);
	count += (x > 0 && puzzle[y][x - 1] != 0 ? 1 : 0);
	count += (puzzle[y][x] != 0 ? 1 : 0);
	count += (x < width - 1 && puzzle[y][x + 1] != 0 ? 1 : 0);
	count += (x > 0 && y < height - 1 && puzzle[y + 1][x - 1] != 0 ? 1 : 0);
	count += (y < height - 1 && puzzle[y + 1][x] != 0 ? 1 : 0);
	count += (x < width - 1 && y < height - 1 && puzzle[y + 1][x + 1] != 0 ? 1 : 0);
	return count;
}


void ExposeAll(int x, int y)
{
	if (x > 0 && y > 0 && exposed[y - 1][x - 1] < 0)
	{
		remainingCount--;
		exposed[y - 1][x - 1] = CountMines(x - 1, y - 1);
		if (exposed[y - 1][x - 1] == 0)
			ExposeAll(x - 1, y - 1);			// expose all neighbors too
	}
	if (y > 0 && exposed[y - 1][x] < 0)
	{
		remainingCount--;
		exposed[y - 1][x] = CountMines(x, y - 1);
		if (exposed[y - 1][x] == 0)
			ExposeAll(x, y - 1);				// expose all neighbors too
	}
	if (x < width - 1 && y > 0 && exposed[y - 1][x + 1] < 0)
	{
		remainingCount--;
		exposed[y - 1][x + 1] = CountMines(x + 1, y - 1);
		if (exposed[y - 1][x + 1] == 0)
			ExposeAll(x + 1, y - 1);			// expose all neighbors too
	}
	if (x > 0 && exposed[y][x - 1] < 0)
	{
		remainingCount--;
		exposed[y][x - 1] = CountMines(x - 1, y);
		if (exposed[y][x - 1] == 0)
			ExposeAll(x - 1, y);				// expose all neighbors too
	}
	if (x < width - 1 && exposed[y][x + 1] < 0)
	{
		remainingCount--;
		exposed[y][x + 1] = CountMines(x + 1, y);
		if (exposed[y][x + 1] == 0)
			ExposeAll(x + 1, y);				// expose all neighbors too
	}
	if (x > 0 && y < height - 1 && exposed[y + 1][x - 1] < 0)
	{
		remainingCount--;
		exposed[y + 1][x - 1] = CountMines(x - 1, y + 1);
		if (exposed[y + 1][x - 1] == 0)
			ExposeAll(x - 1, y + 1);			// expose all neighbors too
	}
	if (y < height - 1 && exposed[y + 1][x] < 0)
	{
		remainingCount--;
		exposed[y + 1][x] = CountMines(x, y + 1);
		if (exposed[y + 1][x] == 0)
			ExposeAll(x, y + 1);				// expose all neighbors too
	}
	if (x < width - 1 && y < height - 1 && exposed[y + 1][x + 1] < 0)
	{
		remainingCount--;
		exposed[y + 1][x + 1] = CountMines(x + 1, y + 1);
		if (exposed[y + 1][x + 1] == 0)
			ExposeAll(x + 1, y + 1);			// expose all neighbors too
	}
}


void Expose(int x, int y)
{
	if (exposed[y][x] >= 0)						// already exposed do nothing
		return;
	if (puzzle[y][x] != 0)						// we hit a mine
	{
		exposed[y][x] = EXPOSE_EXPLODE;
		solving = false;						
		automatic = false;
		bestx = -1;
		gameOver = true;
//		MessageBeep(MB_ICONWARNING);			// sound end of solving
		return;
	}
	remainingCount--;
	exposed[y][x] = CountMines(x, y);			// count mines 	
	if (exposed[y][x] == 0)						// we need to expose all neighbors too
		ExposeAll(x, y);
	if (remainingCount == 0)
	{
		solving = false;
		automatic = false;
		bestx = -1;
		gameOver = true;
		if (mineCount != 0)
		{
			mineCount = 0;
			for (int y = 0; y < height; y++)
				for (int x = 0; x < width; x++)
					if (exposed[y][x] < 0)
						exposed[y][x] = EXPOSE_MINE;
		}
//		MessageBeep(MB_ICONWARNING);			// sound end of solving
	}
}


void NewGame(int startx, int starty)
{
	int x, y;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			exposed[y][x] = -1;
			puzzle[y][x] = 0;
		}
	}
	for (int i = 0; i < minesStart; i++)
	{
		while (true)
		{
			y = (int)Random(height);
			x = (int)Random(width);
			if (puzzle[y][x] != 0)					// do not put mines on top of another
				continue;
			if (y < starty - 1 || y > starty + 1	// stay away from starting area
			|| x < startx - 1 || x > startx + 1)
				break;
		} 
		puzzle[y][x] = 1;
	}
	started = true;
	backup = false;
	solving = false;
	automatic = false;
	bestx = -1;				
	listNext = 0;									// do not draw automatic data
	gameOver = false;
	mineCount = minesStart;
	remainingCount = width * height - minesStart;
	for (int i = 0; i < MAX_RULES; i++)
		rulesCount[i] = 0;
	lastRuleUsed = -1;
	prob = 1.0;
	guesses = 0;
	Expose(startx, starty);							// expose starting area
}


void Count(int x, int y, int & unexposed, int & mines, int & needed, int & neighbors)
{
	unexposed = 0;
	mines = 0;
	needed = 0;
	neighbors = 0;
	if (x > 0 && y > 0)
	{
		if (exposed[y - 1][x - 1] == EXPOSE_MINE)
			mines++;
		else if (exposed[y - 1][x - 1] < 0)
			unexposed++;
		else
			neighbors++;
	}
	if (y > 0)
	{
		if (exposed[y - 1][x] == EXPOSE_MINE)
			mines++;
		else if (exposed[y - 1][x] < 0)
			unexposed++;
		else
			neighbors++;
	}
	if (x < width - 1 && y > 0)
	{
		if (exposed[y - 1][x + 1] == EXPOSE_MINE)
			mines++;
		else if (exposed[y - 1][x + 1] < 0)
			unexposed++;
		else
			neighbors++;
	}
	if (x > 0)
	{
		if (exposed[y][x - 1] == EXPOSE_MINE)
			mines++;
		else if (exposed[y][x - 1] < 0)
			unexposed++;
		else
			neighbors++;
	}
	if (x < width - 1)
	{
		if (exposed[y][x + 1] == EXPOSE_MINE)
			mines++;
		else if (exposed[y][x + 1] < 0)
			unexposed++;
		else
			neighbors++;
	}
	if (x > 0 && y < height - 1)
	{
		if (exposed[y + 1][x - 1] == EXPOSE_MINE)
			mines++;
		else if (exposed[y + 1][x - 1] < 0)
			unexposed++;
		else
			neighbors++;
	}
	if (y < height - 1)
	{
		if (exposed[y + 1][x] == EXPOSE_MINE)
			mines++;
		else if (exposed[y + 1][x] < 0)
			unexposed++;
		else
			neighbors++;
	}
	if (x < width - 1 && y < height - 1)
	{
		if (exposed[y + 1][x + 1] == EXPOSE_MINE)
			mines++;
		else if (exposed[y + 1][x + 1] < 0)
			unexposed++;
		else
			neighbors++;
	}
	if (exposed[y][x] > 0 && exposed[y][x] < EXPOSE_MINE)
		needed = exposed[y][x] - mines;
}


void AddMine(int x, int y)
{
	if (x < 0
	|| x >= width
	|| y < 0
	|| y >= height)
		return;
	if (exposed[y][x] < 0)
	{
		mineCount--;
		exposed[y][x] = EXPOSE_MINE;
		if (puzzle[y][x] == 0)				// mine should not be here
		{
			solving = false;
			automatic = false;
			bestx = -1;
			gameOver = true;				// stop play when we do this
//			MessageBeep(MB_ICONWARNING);	// sound end of solving
		}
	}
}

void ExposeTile(int x, int y)
{
	if (x < 0
	|| x >= width
	|| y < 0
	|| y >= height)
		return;
	Expose(x, y);							// expose this tile and neighbors
}


bool MakeMove()
{
	bestx = besty = -1;						// no best move to display
	listNext = 0;							// so that we do not really have any list data for display purposes
	lastRuleUsed = -1;
	// Rule 1 and 2 applied here
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			Count(x, y, unexposed[y][x], mines[y][x], needed[y][x], neighbors[y][x]);
			// Rule 1 - force mine
			// find exposed square with N unexposed neighbors
			// and N remaining mines around this square
			// In this case add mines to all unexposed neighbors
			if (exposed[y][x] > 0
			&& unexposed[y][x] != 0
			&& exposed[y][x] == unexposed[y][x] + mines[y][x])
			{
				lastRuleUsed = RULE_1;
				rulesCount[RULE_1]++;
				SaveBackUp();								// save current board
				AddMine(x - 1, y - 1);
				AddMine(x, y - 1);
				AddMine(x + 1, y - 1);
				AddMine(x - 1, y);
				AddMine(x + 1, y);
				AddMine(x - 1, y + 1);
				AddMine(x, y + 1);
				AddMine(x + 1, y + 1);
				return true;
			}
			// Rule 2 - expose
			// If an exposed square already has all the mines marked
			// fill all unexposed squares around it
			if (exposed[y][x] > 0
			&& unexposed[y][x] != 0
			&& exposed[y][x] == mines[y][x])
			{
				lastRuleUsed = RULE_2;
				rulesCount[RULE_2]++;
				SaveBackUp();								// save current board
				ExposeTile(x - 1, y - 1);
				ExposeTile(x, y - 1);
				ExposeTile(x + 1, y - 1);
				ExposeTile(x - 1, y);
				ExposeTile(x + 1, y);
				ExposeTile(x - 1, y + 1);
				ExposeTile(x, y + 1);
				ExposeTile(x + 1, y + 1);
				return true;
			}
		}
	}

	// create perimeter lists
	CreatePerimeterLists();										// create the lists
	TimeStart();
	int processed = CreatePerimeterSolutions(0, -1, mineCount, false);	// find all possible solutions on the lists	
	double time = TimeEnd();
	if (time > WARNING_TIME)
	{
		if (testAllResults != NULL)
		{
			if (saveTiming)
			{
				char testFileName[MAX_PATH];
				sprintf(testFileName, "%s\\CreatePerimeterTimeCheck seed=0x%llx.txt", prefixString, saveSeed);
				SavePuzzle(testFileName);
			}
			if (testResults != NULL)
			{
				int sum = GetSolutionCount();
				fprintf(testResults, "\n***** TIMING: Puzzle=%d CreatePerimeterTimeCheck time=%.3f solutions=%d (processed=%d conflicts=%d) len=%d\n",
					testing, time, sum, processed, conflictsInSolutions, listNext);
			}
		}
	}
	CalculateProb();										// calculate probability for each entry in lists
	UpdatePerimeterStats();

	// find simple solutions by deduction
	if (FindEasySolution())									// find obvious move if any
	{
		lastRuleUsed = DEDUCE;
		rulesCount[DEDUCE]++;
		return true;
	}

	lastRuleUsed = -1;
	superNext = 0;											// no SuperLocArray entries
	gameTreeNext = 0;										// no game tree entries		
	// Guess 0 - do or die
	// do it now so that we can simplify the cases where we have a real choice
	if (PickGuess0())
	{
		if (automatic)
		{
			automatic = false;
			solving = false;
		}
		return true;
	}

	// try brute force gametree analysis if possible
	if (UseGameTree())
	{
		if (automatic)
		{
			automatic = false;
			solving = false;
		}
		return true;
	}

	// look for super locations
	if (UseSuperLoc())
	{
		if (automatic)
		{
			automatic = false;
			solving = false;
		}
		return true;
	}

	// try Guess 1 if the best move is in the open area
	if (UseGuess1())
	{
		if (automatic)
		{
			automatic = false;
			solving = false;
		}
		return true;
	}

	Beep(500, 80);
	solving = false;
	automatic = false;
	bestx = -1;
	return false;
}