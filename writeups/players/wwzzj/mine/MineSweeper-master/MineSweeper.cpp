// MineSweeper.cpp : Defines the entry point for the application.
//

#include "stdafx.h"


#define MAX_LOADSTRING 100						// max size of app title and class name

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR	szTitle[MAX_LOADSTRING];                // The title bar text
TCHAR	szWindowClass[MAX_LOADSTRING];          // the main window class name
HBITMAP hBackgndExpert = NULL;
HBITMAP hBackgndMedium = NULL;
HBITMAP hBackgndSmall = NULL;
HBITMAP hMine = NULL;
HBITMAP hNumbers = NULL;
HBITMAP hDead = NULL;
HBITMAP hSimilar = NULL;
HBITMAP hSuper = NULL;
HBITMAP hBest = NULL;
FILE *	testResults = NULL;						// for testing stats
FILE *	testAllResults = NULL;					// for testing starting, endplay, or offsets possibilities
FILE *	gametreeResults = NULL;					// listing the game tree
FILE *	superLocResults = NULL;					// listing the game tree
FILE *	gametreeTimes;							// list game tree solution times
char	sizeString[16];
char	startString[16];
char	endPlayString[32];
char	thresholdString[32];
char	seedString[32];
int		width, height, minesStart;				// size of puzzle and number of mines to start
int		testing;								// current puzzle testing (1 to testingMax)
int		testingMax;								// max number of puzzles to test for each set
int		testingAllTotal;						// expected total number of puzzles for testing (actual may be less)
char 	prefixString[MAX_PATH];
char	testFilePath[MAX_PATH];
char	testFileName[MAX_PATH];
char	testAllFileName[MAX_PATH];
int		startX, startY;							// starting location for new games
int		saveStartX, saveStartY;
int		testRunCount;							// number of 100 sets to run
int		setsUsed;								// current set number
int		setStart;								// starting set number
int		setEnd;									// number of sets to test
double	guessThreshold;							// threshold for guessing
double	clearMultiplier;						// super location multiplier for secondary entries
bool	topToBottomPriority;					// guess priority from top to bottom vs left to right first
bool	saveSetResults;							// save results for each set of 100 puzzles
bool	saveTiming;								// save puzzles that take too long when testing
bool	saveInteresting;						// save interestig puzzles when testing
bool	saveGuess0Fails;						// save guess0 fails
bool	saveGameTreeFails;						// save gametree fails
bool	saveSuperLocFails;						// save guess1 and superLoc fails
bool	saveSuperLocMultiplierChanges;			// save puzzles when superLoc threshold is used
int		saveGameTreeResults;					// 0 = no results, 1 = final results, 2 = top level results, 3 = full tree
int		saveSuperLocResults;					// 0 = no results, 1 = finish results, 2 = all results, 3 = include all solutions
int		useNewSuperLocMethod;					// -1=jar method, 0=v1.2c method, 1=new method for calculating superLoc clears
bool	useSuperLocMultiplier;					// use multiply method to use second best superLoc if clears is better
U64		seed;									// seed for random number generator
U64		saveSeed;								// save seed used for testing
U64		startSeed;								// starting seed to use for starting multiple tests


static	RECT Interior = { WIDTH_OFFSET, 0, WIDTH_OFFSET + SCREEN_WIDTH, 343 }; // include time and count area below
static	RECT TimeRect = { 114, 329, 145, 343 };
static	RECT TestRect = { 200, 4, 400, 343 };
static	RECT RuleRect = { 200, 4, 400, 20 };
static	RECT GameOverRect = { 200, 329, 400, 343 };
static	RECT CountRect = { 456, 329, 487, 343 };
static	RECT ProbRect = { 100, 329, 160, 343 };

static char * RuleText[MAX_RULES] =
{
	"Mine",
	"Clear",
	"Deduced",
	"Do or Die",
	"GameTree",
	"SuperLoc",
	"Guess 1",
	"Guess 2",
};


void ClearStats()
{
	for (int i = 0; i < MAX_RULES; i++)
		rulesCountTotal[i] = 0;
	winTotal = 0;
	testTotal = 0;
	guess0Failed = 0;
	guess1Failed = 0;
	guess2Failed = 0;
	gameTreeFailed = 0;
	superLocFailed = 0;
	maxHashUsed = 0;
	maxHashHits = 0;
	winProb = 0;
	lossProb = 0;
	guess0Prob = 0;
	guess1Prob = 0;
	guess2Prob = 0;
	gameTreeProb = 0;
	superLocProb = 0;
	guessesPerWin = 0;
	guessesPerLoss = 0;
	maxCounts = 1;										// see MAX_COUNTS in tank.cpp
	maxLists = 1;										// see MAX_LISTS in tank.cpp
	maxPerimeter = 0;
	maxSinglePerimeter = 0;
	maxNeighbors = 0;
	numEndPlays = 0;
	numMissedEndPlays = 0;
	maxEndPlayLength = 0;
	numEndPlaySmall = 0;
	numEndPlayLarge = 0;
	maxListSolutions = 0;
	maxCompletedSolutions = 0;
	gametreeUsed = 0;
	gametreeUsedEarly = 0;
	gametreeEntriesUsed = 0;
	gametreeHelped = 0;
	gametreeLinksNeeded = 0;
	gametreeMaxDepth = 0;
	superLocMaxCandidates = 0;
	superLocMultiplierUsed = 0;
	superLocMultiplierImproved = 0;
	superLocMultiplierFailed = 0;
	superLocMultiplyUsed = 0;
}


void SetTestingFiles(int wmId)
{
	time_t now;
	struct tm * date;

	prefixString[0] = '.';
	prefixString[1] = 0;
#if SHOW_GAMETREE_TIMES
	gametreeTimes = fopen("gametreeTimes.txt", "w");
#endif
	if (wmId == IDM_TESTRUN)
	{
		TimeStartOverall();
		if (seed == 0)								// use default seed if one is not provided
			seed = 1;
		startSeed = seed;
		setsUsed = 1;
		winAllTotal = 0;
		sprintf(sizeString, "%dx%d", width, height);
		sprintf(startString, "start=%d,%d", startX, startY);
		sprintf(endPlayString, "maxSolutions=%d", maxSolutions);
		if (useSuperLocMultiplier)
			sprintf(thresholdString, "threshold=%.3fx%.2f", guessThreshold, clearMultiplier);
		else
			sprintf(thresholdString, "threshold=%.3f", guessThreshold);
		now = time(NULL);
		date = localtime(&now);
		testingMax = 100;							// number of puzzles to test
		RandomSeed2(seed);
		testingAllTotal = setEnd * testingMax;
		sprintf(seedString, "seed=0x%llx", seed);
		char * method;
		if (useNewSuperLocMethod < 0)
			method = "jar";
		else if (useNewSuperLocMethod == 0)
			method = "old";
		else
			method = "new";
		char * direction;
		if (topToBottomPriority)
			direction = "tb";
		else
			direction = "lr";
		sprintf(prefixString, ".\\v%s  %s %s%s %s %dx100 %s %s %s %s %d.%d.%d %d.%d.%d",
			VERSION, sizeString, method,
			(useSuperLocMultiplier ? "+" : ""), direction,
			setEnd, startString, endPlayString, thresholdString, seedString,
			1900 + date->tm_year, date->tm_mon + 1, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);
		CreateDirectory(prefixString, NULL);	// create test directory
		sprintf(testAllFileName, "%s\\results.txt", prefixString);
		testAllResults = fopen(testAllFileName, "w");
		if (testAllResults != NULL)
		{
			fprintf(testAllResults, "Ed Logg's Minesweeper version %s\n", VERSION);
			fprintf(testAllResults, "set\twins\n------------\n");
		}
	}
}


void StartTest()
{
	time_t now;
	struct tm * date;

	testing = 1;								// start with this
	ClearStats();
	for (int i = 0; i < MAX_WINS_WITH_GUESSES; i++)
		winsWithGuesses[i] = 0;
	sprintf(seedString, "set=%d", setsUsed);
	now = time(NULL);
	date = localtime(&now);
	if (saveSetResults)
	{
		sprintf(testFilePath, "%s\\Test %s", prefixString, seedString);
		CreateDirectory(testFilePath, NULL);	// create test directory
		if (prefixString[1] == 0)					
			strncpy(prefixString, testFilePath, MAX_PATH);
		sprintf(testFileName, "%s\\results.txt", testFilePath);
		testResults = fopen(testFileName, "w");
	}
}


bool	GetNextTest()
{
	if (testAllResults == NULL)
		return false;
	++setsUsed;
	if (setsUsed > setEnd)						// we are done
	{
		setStart++;								// next set to use
		seed = 0;								// clear seed we just set
		SaveTotalResults(testAllResults);		// display all stats
		return false;
	}
	StartTest();
	return true;
}


void Draw(HWND hWnd, HDC hdc, PAINTSTRUCT ps)
{
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmOld;
	int offset;

	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT logfont;
	GetObject(hFont, sizeof(LOGFONT), &logfont);
	logfont.lfHeight = 16;
	logfont.lfWeight = 1000;		// 400=normal 700=bold 1000=max
	HFONT hNewFont = CreateFontIndirect(&logfont);

	if (testing
	&&  ps.rcPaint.top == TestRect.top
	&&	ps.rcPaint.left == TestRect.left
	&&	ps.rcPaint.right == TestRect.right
	&&	ps.rcPaint.bottom == TestRect.bottom)
	{
		SelectObject(hdcMem, hBackgndExpert);							// clear game over area in case it was used last frame
		BitBlt(hdc, GameOverRect.left, GameOverRect.top, GameOverRect.right - GameOverRect.left, GameOverRect.bottom - GameOverRect.top, hdcMem, GameOverRect.left, GameOverRect.top, SRCCOPY);
		BitBlt(hdc, RuleRect.left, RuleRect.top, RuleRect.right - RuleRect.left, RuleRect.bottom - RuleRect.top, hdcMem, RuleRect.left, RuleRect.top, SRCCOPY);
		HFONT hOldFont = (HFONT)SelectObject(hdc, hNewFont);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(0, 0, 0));
		char string[24];
		int test = totalTests + testTotal;
		sprintf(string, "%d/%d", test, testingAllTotal);
		DrawText(hdc, string, -1, &GameOverRect, DT_CENTER | DT_TOP);
		int wins = winAllTotal + winTotal;
		int total = totalTests + testTotal;
		if (total > 0)
		{
			char string[24];
			double winrate = (double)wins / total;
			double moe = MARGIN_OF_ERROR * sqrt((1.0 - winrate) * winrate / total);
			sprintf(string, "%.3f%% +- %.3f%%", winrate * 100.0, moe * 100.0);
			DrawText(hdc, string, -1, &RuleRect, DT_CENTER | DT_TOP);
		}
		SelectObject(hdc, hOldFont);								// reset font
		DeleteObject(hNewFont);
		DeleteDC(hdcMem);											// delete 												
		return;
	}

	// draw background
	if (width == MAX_WIDTH)
	{
		offset = WIDTH_OFFSET;
		hbmOld = (HBITMAP)SelectObject(hdcMem, hBackgndExpert);		// select new object and save the old
	}
	else if (width == 9)
	{
		offset = WIDTH_OFFSET + 10 * TILE_WIDTH;
		hbmOld = (HBITMAP)SelectObject(hdcMem, hBackgndSmall);		// select new object and save the old
	}
	else
	{
		offset = WIDTH_OFFSET + 7 * TILE_WIDTH;
		hbmOld = (HBITMAP)SelectObject(hdcMem, hBackgndMedium);		// select new object and save the old
	}
	if (ps.rcPaint.left == 0										// redraw the whole screen
	|| ps.rcPaint.right == BACKGND_WIDTH
	|| ps.rcPaint.bottom == BACKGND_HEIGHT)
	{
		BitBlt(hdc, 0, 0, BACKGND_WIDTH, BACKGND_HEIGHT, hdcMem, 0, 0, SRCCOPY);
	}
	else if (ps.rcPaint.left != 0									// not drawing the whole screen
	&& ps.rcPaint.left != Interior.left)							// or the interior
	{
		BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, hdcMem, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);
		SelectObject(hdcMem, hbmOld);								// restore the old object
		DeleteDC(hdcMem);											// delete 
		return;
	}
	else															// erase the rule selection
	{
		BitBlt(hdc, RuleRect.left, RuleRect.top, RuleRect.right - RuleRect.left, RuleRect.bottom - RuleRect.top, hdcMem, RuleRect.left, RuleRect.top, SRCCOPY);
	}

	// draw mines
	SelectObject(hdcMem, hMine);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (exposed[y][x] == EXPOSE_EXPLODE)
				BitBlt(hdc, offset + x * TILE_WIDTH, HEIGHT_OFFSET + y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, hdcMem, TILE_WIDTH, 0, SRCCOPY);
			else if (exposed[y][x] == EXPOSE_MINE)
			{
				if ((gameOver || cheating)
				&& puzzle[y][x] == 0)			// mine should not be there
					BitBlt(hdc, offset + x * TILE_WIDTH, HEIGHT_OFFSET + y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, hdcMem, 2*TILE_WIDTH, 0, SRCCOPY);
				else
					BitBlt(hdc, offset + x * TILE_WIDTH, HEIGHT_OFFSET + y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, hdcMem, 0, 0, SRCCOPY);
			}
			else if (cheating					// show mines when cheating
			&&  exposed[y][x] < 0)
			{
				if (puzzle[y][x] != 0)			// mine should not be there
					BitBlt(hdc, offset + x * TILE_WIDTH, HEIGHT_OFFSET + y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, hdcMem, 0, 0, SRCCOPY);
			}
		}
	}

	// draw numbers
	SelectObject(hdcMem, hNumbers);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (exposed[y][x] >= 0
			&& exposed[y][x] <= 8)
				BitBlt(hdc, offset + x * TILE_WIDTH, HEIGHT_OFFSET + y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, hdcMem, exposed[y][x] * TILE_WIDTH, 0, SRCCOPY);
		}
	}

	// draw dead, similar and super locations
	if (listNext > 0)							// lists are valid
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (listUsed[y][x] < 0)
					continue;
				int artOffset = 0;
				if (cheating && puzzle[y][x] != 0)
					artOffset = TILE_WIDTH;
				if (listArray[listUsed[y][x]].dead)
				{
					SelectObject(hdcMem, hDead);
					BitBlt(hdc, offset + x * TILE_WIDTH, HEIGHT_OFFSET + y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, hdcMem, artOffset, 0, SRCCOPY);
				}
				else if (listArray[listUsed[y][x]].similar)
				{
					SelectObject(hdcMem, hSimilar);
					BitBlt(hdc, offset + x * TILE_WIDTH, HEIGHT_OFFSET + y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, hdcMem, artOffset, 0, SRCCOPY);
				}
			}
		}
		SelectObject(hdcMem, hSuper);
		for (int i = 0; i < superNext; i++)
		{		
			int artOffset = 0;
			if (cheating && puzzle[SuperLocArray[i].y][SuperLocArray[i].x] != 0)
				artOffset = TILE_WIDTH;
			BitBlt(hdc, offset + SuperLocArray[i].x * TILE_WIDTH, HEIGHT_OFFSET + SuperLocArray[i].y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, hdcMem, artOffset, 0, SRCCOPY);
		}
		if (gameTreeNext > 0)
		{
			SelectObject(hdcMem, hSuper);
			for (int i = 0; i < gameTreeNext; i++)
			{
				int artOffset = 0;
				if (cheating && puzzle[GameTreeArray[i].y][GameTreeArray[i].x] != 0)
					artOffset = TILE_WIDTH;
				BitBlt(hdc, offset + GameTreeArray[i].x * TILE_WIDTH, HEIGHT_OFFSET + GameTreeArray[i].y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, hdcMem, artOffset, 0, SRCCOPY);
			}
		}
	}
	if (bestx >= 0)
	{
		SelectObject(hdcMem, hBest);
		int artOffset = 0;
		if (cheating && puzzle[besty][bestx] != 0)
			artOffset = TILE_WIDTH;
		BitBlt(hdc, offset + bestx * TILE_WIDTH, HEIGHT_OFFSET + besty * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, hdcMem, artOffset, 0, SRCCOPY);
	}

	// draw number of mines remaining
	SelectObject(hdcMem, hBackgndExpert);
	BitBlt(hdc, CountRect.left, CountRect.top, CountRect.right - CountRect.left, CountRect.bottom - CountRect.top, hdcMem, CountRect.left, CountRect.top, SRCCOPY);
	BitBlt(hdc, ProbRect.left, ProbRect.top, ProbRect.right - ProbRect.left, ProbRect.bottom - ProbRect.top, hdcMem, ProbRect.left, ProbRect.top, SRCCOPY);
	HFONT hOldFont = (HFONT)SelectObject(hdc, hNewFont);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(255, 255, 255));
	char string[20];
//	if (cheating)
//		sprintf(string, "%d", remainingCount);
//	else
		sprintf(string, "%d", mineCount);
	DrawText(hdc, string, -1, &CountRect, DT_CENTER | DT_TOP);

	// draw probability 
	sprintf(string, "%d%%", (int)(100 * prob));
	DrawText(hdc, string, -1, &ProbRect, DT_CENTER | DT_TOP);
	SelectObject(hdc, hOldFont);										// reset font

	// show game over or progress in test run
	if (testing)
	{
		hOldFont = (HFONT)SelectObject(hdc, hNewFont);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(0, 0, 0));
		char string[24];
		int test = totalTests + testTotal;
		sprintf(string, "%d/%d", test, testingAllTotal);
		DrawText(hdc, string, -1, &GameOverRect, DT_CENTER | DT_TOP);
		SelectObject(hdc, hOldFont);								// reset font
	}
	else
	{
		SelectObject(hdcMem, hBackgndExpert);							// clear game over area in case it was used last frame
		BitBlt(hdc, GameOverRect.left, GameOverRect.top, GameOverRect.right - GameOverRect.left, GameOverRect.bottom - GameOverRect.top, hdcMem, GameOverRect.left, GameOverRect.top, SRCCOPY);
		if (gameOver)
		{
			hOldFont = (HFONT)SelectObject(hdc, hNewFont);
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(0, 0, 0));
			DrawText(hdc, "Game Over", -1, &GameOverRect, DT_CENTER | DT_TOP);
			SelectObject(hdc, hOldFont);								// reset font
		}
	}

	// show last rule used or testing results
	hOldFont = (HFONT)SelectObject(hdc, hNewFont);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, RGB(0, 0, 0));
	if (testing)
	{
		int wins = winAllTotal + winTotal;
		int total = totalTests + testTotal;
		if (total > 0)
		{
			char string[24];
			double winrate = (double)wins / total;
			double moe = MARGIN_OF_ERROR * sqrt((1.0 - winrate) * winrate / total);
			sprintf(string, "%.3f%% +- %.3f%%", winrate * 100.0, moe * 100.0);
			DrawText(hdc, string, -1, &RuleRect, DT_CENTER | DT_TOP);
		}
	}
	else if (lastRuleUsed >= 0)
	{
		DrawText(hdc, RuleText[lastRuleUsed], -1, &RuleRect, DT_CENTER | DT_TOP);
	}
	SelectObject(hdc, hOldFont);										// reset font

	SelectObject(hdcMem, hbmOld);										// restore the old object
	DeleteObject(hNewFont);
	DeleteDC(hdcMem);													// delete 												
}


bool FindPosition(HWND hWnd, int h, int v, int & x, int & y)
{
	int offset;
	if (width == MAX_WIDTH)
		offset = WIDTH_OFFSET;
	else if (width == 9)
		offset = WIDTH_OFFSET + 10 * TILE_WIDTH;
	else 
		offset = WIDTH_OFFSET + 7 * TILE_WIDTH;
	x = (h - offset) / TILE_WIDTH;
	y = (v - HEIGHT_OFFSET) / TILE_HEIGHT;
	if (0 <= x && x < width
	&&	0 <= y && y < height)
	{
		return true;
	}
	return false;
}


// Message handler for getting a random seed
INT_PTR CALLBACK Statistics(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND hwndOwner;
	RECT rcOwner, rcWindow;
	int width, widthOwner;
	HWND msg;
	char string[128];
	double winrate = 0.0;

	switch (message)
	{
	case WM_INITDIALOG:
		if ((hwndOwner = GetParent(hDlg)) == NULL)
			hwndOwner = GetDesktopWindow();
		GetWindowRect(hwndOwner, &rcOwner);
		widthOwner = rcOwner.right - rcOwner.left;
		GetWindowRect(hDlg, &rcWindow);
		width = rcWindow.right - rcWindow.left;
		SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (widthOwner - width) / 2, rcOwner.top - 10, 0, 0, SWP_NOSIZE);
		msg = GetDlgItem(hDlg, IDC_STATS0);
		sprintf(string, "%d", totalTests);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS1);
		sprintf(string, "%d", totalWins);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS2);
		if (totalTests != 0)
		{
			winrate = (double)totalWins / totalTests;
			sprintf(string, "%.3f%%", 100.0 * winrate);
		}
		else
			sprintf(string, "0.0%%");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSMOE);
		if (totalTests != 0)
		{
			double moe = MARGIN_OF_ERROR * sqrt((1.0 - winrate) * winrate / totalTests);
			sprintf(string, "%.3f%%", 100.0 * moe);
		}
		else
			sprintf(string, "0.0%%");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS3);
		sprintf(string, "%d", totalGuess0);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS4);
		sprintf(string, "%d", totalGuess1);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS5);
		sprintf(string, "%d", totalGuess2);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS6);
		sprintf(string, "%d", totalGameTree);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS6x);
		sprintf(string, "%d", totalSuperLoc);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS7);
		if (totalGuess0 != 0)
			sprintf(string, "%3.1f%%", (100.0 * totalGuess0Failed) / totalGuess0);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS8);
		if (totalGuess1 != 0)
			sprintf(string, "%3.1f%%", (100.0 * totalGuess1Failed) / totalGuess1);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS9);
		if (totalGuess2 != 0)
			sprintf(string, "%3.1f%%", (100.0 * totalGuess2Failed) / totalGuess2);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS10);
		if (totalGameTree != 0)
			sprintf(string, "%3.1f%%", (100.0 * totalGameTreeFailed) / totalGameTree);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS10x);
		if (totalSuperLoc != 0)
			sprintf(string, "%3.1f%%", (100.0 * totalSuperLocFailed) / totalSuperLoc);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS11);
		if (totalWins != 0)
			sprintf(string, "%4.2f", (double)totalGuessesPerWin / totalWins);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS12);
		if ((totalTests - totalWins) != 0)
			sprintf(string, "%4.2f", (double)totalGuessesPerLoss / (totalTests - totalWins));
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS13);
		if (totalWins > 0)
			sprintf(string, "%4.1f%%", 100.0 * totalWinProb / totalWins);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS14);
		if ((totalTests - totalWins) > 0)
			sprintf(string, "%4.1f%%", 100.0 * totalLossProb / (totalTests - totalWins));
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS15);
		if (totalGuess0 != 0)
			sprintf(string, "%4.1f%%", (100.0 * totalGuess0Prob) / totalGuess0);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS16);
		if (totalGuess1 != 0)
			sprintf(string, "%4.1f%%", (100.0 * totalGuess1Prob) / totalGuess1);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS17);
		if (totalGuess2 != 0)
			sprintf(string, "%4.1f%%", (100.0 * totalGuess2Prob) / totalGuess2);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS18);
		if (totalGameTree != 0)
			sprintf(string, "%4.1f%%", (100.0 * totalGameTreeProb) / totalGameTree);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATS18x);
		if (totalSuperLoc != 0)
			sprintf(string, "%4.1f%%", (100.0 * totalSuperLocProb) / totalSuperLoc);
		else
			sprintf(string, "0.0");
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS0);
		sprintf(string, " 0 : %-4d", totalWinsWithGuesses[0]);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS1);
		sprintf(string, " 1 : %-4d", totalWinsWithGuesses[1]);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS2);
		sprintf(string, " 2 : %-4d", totalWinsWithGuesses[2]);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS3);
		sprintf(string, " 3 : %-4d", totalWinsWithGuesses[3]);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS4);
		sprintf(string, " 4 : %-4d", totalWinsWithGuesses[4]);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS5);
		sprintf(string, " 5 : %-4d", totalWinsWithGuesses[5]);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS6);
		sprintf(string, " 6 : %-4d", totalWinsWithGuesses[6]);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS7);
		sprintf(string, " 7 : %-4d", totalWinsWithGuesses[7]);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS8);
		sprintf(string, " 8 : %-4d", totalWinsWithGuesses[8]);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS9);
		sprintf(string, " 9 : %-4d", totalWinsWithGuesses[9]);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_STATSWINS10);
		sprintf(string, "10+: %-4d", totalWinsWithGuesses[10]);
		SetWindowText(msg, string);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK
		||	LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDIGNORE)
		{
			ClearTotalStats();
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


// Message handler for testing runs
INT_PTR CALLBACK GetTestParameters(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND hwndOwner;
	RECT rcOwner, rcWindow;
	int widthW, widthOwner;
	HWND msg;
	char string[32];

	switch (message)
	{
	case WM_INITDIALOG:
		if ((hwndOwner = GetParent(hDlg)) == NULL)
			hwndOwner = GetDesktopWindow();
		GetWindowRect(hwndOwner, &rcOwner);
		widthOwner = rcOwner.right - rcOwner.left;
		GetWindowRect(hDlg, &rcWindow);
		widthW = rcWindow.right - rcWindow.left;
		SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (widthOwner - widthW) / 2, rcOwner.top + 50, 0, 0, SWP_NOSIZE);	
		msg = GetDlgItem(hDlg, IDC_TESTSETS);
		sprintf(string, "%d", testRunCount);		// test count
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_TESTSEED);
		sprintf(string, "%d", setStart);			// default to next set
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_TESTSTARTX);
		sprintf(string, "%d", startX);
		SetWindowText(msg, string);
		msg = GetDlgItem(hDlg, IDC_TESTSTARTY);
		sprintf(string, "%d", startY);
		SetWindowText(msg, string);
		CheckDlgButton(hDlg, IDC_TESTEXPERT, (width == 30 ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTMEDIUM, (width == 16 ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTSMALL, (width == 9 ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTSETRESULTS, (saveSetResults ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTSAVETIMING, (saveTiming ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTSAVEINTERESTING, (saveInteresting ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTSAVEGUESS0FAILS, (saveGuess0Fails ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTSAVEGAMETREEFAILS, (saveGameTreeFails ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTSAVESUPERLOCFAILS, (saveSuperLocFails ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTSAVESUPERLOCTHRESHOLD, (saveSuperLocMultiplierChanges ? BST_CHECKED : BST_UNCHECKED));
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_TESTEXPERT)
		{
			CheckDlgButton(hDlg, IDC_TESTEXPERT, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_TESTMEDIUM, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_TESTSMALL, BST_UNCHECKED);
			msg = GetDlgItem(hDlg, IDC_TESTSTARTX);
			sprintf(string, "%d", start30x16X);
			SetWindowText(msg, string);
			msg = GetDlgItem(hDlg, IDC_TESTSTARTY);
			sprintf(string, "%d", start30x16Y);
			SetWindowText(msg, string);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTMEDIUM)
		{
			CheckDlgButton(hDlg, IDC_TESTEXPERT, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_TESTMEDIUM, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_TESTSMALL, BST_UNCHECKED);
			msg = GetDlgItem(hDlg, IDC_TESTSTARTX);
			sprintf(string, "%d", start16x16X);
			SetWindowText(msg, string);
			msg = GetDlgItem(hDlg, IDC_TESTSTARTY);
			sprintf(string, "%d", start16x16Y);
			SetWindowText(msg, string);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTSMALL)
		{
			CheckDlgButton(hDlg, IDC_TESTEXPERT, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_TESTMEDIUM, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_TESTSMALL, BST_CHECKED);
			msg = GetDlgItem(hDlg, IDC_TESTSTARTX);
			sprintf(string, "%d", start9x9X);
			SetWindowText(msg, string);
			msg = GetDlgItem(hDlg, IDC_TESTSTARTY);
			sprintf(string, "%d", start9x9Y);
			SetWindowText(msg, string);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTSETRESULTS)
		{
			CheckDlgButton(hDlg, IDC_TESTSETRESULTS, (IsDlgButtonChecked(hDlg, IDC_TESTSETRESULTS) ? BST_UNCHECKED : BST_CHECKED));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTSAVETIMING)
		{
			CheckDlgButton(hDlg, IDC_TESTSAVETIMING, (IsDlgButtonChecked(hDlg, IDC_TESTSAVETIMING) ? BST_UNCHECKED : BST_CHECKED));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTSAVEINTERESTING)
		{
			CheckDlgButton(hDlg, IDC_TESTSAVEINTERESTING, (IsDlgButtonChecked(hDlg, IDC_TESTSAVEINTERESTING) ? BST_UNCHECKED : BST_CHECKED));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTSAVEGUESS0FAILS)
		{
			CheckDlgButton(hDlg, IDC_TESTSAVEGUESS0FAILS, (IsDlgButtonChecked(hDlg, IDC_TESTSAVEGUESS0FAILS) ? BST_UNCHECKED : BST_CHECKED));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTSAVEGAMETREEFAILS)
		{
			CheckDlgButton(hDlg, IDC_TESTSAVEGAMETREEFAILS, (IsDlgButtonChecked(hDlg, IDC_TESTSAVEGAMETREEFAILS) ? BST_UNCHECKED : BST_CHECKED));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTSAVESUPERLOCFAILS)
		{
			CheckDlgButton(hDlg, IDC_TESTSAVESUPERLOCFAILS, (IsDlgButtonChecked(hDlg, IDC_TESTSAVESUPERLOCFAILS) ? BST_UNCHECKED : BST_CHECKED));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTSAVESUPERLOCTHRESHOLD)
		{
			CheckDlgButton(hDlg, IDC_TESTSAVESUPERLOCTHRESHOLD, (IsDlgButtonChecked(hDlg, IDC_TESTSAVESUPERLOCTHRESHOLD) ? BST_UNCHECKED : BST_CHECKED));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDOK)
		{
			if (IsDlgButtonChecked(hDlg, IDC_TESTEXPERT))
			{
				width = 30;
				height = 16;
				minesStart = 99;				
			}
			else if (IsDlgButtonChecked(hDlg, IDC_TESTMEDIUM))
			{
				width = 16;
				height = 16;
				minesStart = 40;
			}
			else 
			{
				width = 9;
				height = 9;
				minesStart = 10;
			}
			if (IsDlgButtonChecked(hDlg, IDC_TESTSETRESULTS))
				saveSetResults = true;
			else
				saveSetResults = false;
			if (IsDlgButtonChecked(hDlg, IDC_TESTSAVETIMING))
				saveTiming = true;
			else
				saveTiming = false;
			if (IsDlgButtonChecked(hDlg, IDC_TESTSAVEINTERESTING))
				saveInteresting = true;
			else
				saveInteresting = false;
			if (IsDlgButtonChecked(hDlg, IDC_TESTSAVEGUESS0FAILS))
				saveGuess0Fails = true;
			else
				saveGuess0Fails = false;
			if (IsDlgButtonChecked(hDlg, IDC_TESTSAVEGAMETREEFAILS))
				saveGameTreeFails = true;
			else
				saveGameTreeFails = false;
			if (IsDlgButtonChecked(hDlg, IDC_TESTSAVESUPERLOCFAILS))
				saveSuperLocFails = true;
			else
				saveSuperLocFails = false;
			if (IsDlgButtonChecked(hDlg, IDC_TESTSAVESUPERLOCTHRESHOLD))
				saveSuperLocMultiplierChanges = true;
			else
				saveSuperLocMultiplierChanges = false;
			msg = GetDlgItem(hDlg, IDC_TESTSETS);
			GetWindowText(msg, string, sizeof(string));
			testRunCount = setEnd = atoi(string);		
			msg = GetDlgItem(hDlg, IDC_TESTSEED);
			int base = 10;
			GetWindowText(msg, string, sizeof(string));
			if (string[0] == '0' && string[1] == 'x')
				base = 16;
			seed = strtoull(string, NULL, base);
			msg = GetDlgItem(hDlg, IDC_TESTSTARTX);
			GetWindowText(msg, string, sizeof(string));
			startX = (U32)atoi(string);
			if (startX < 0)
				startX = 0;
			else if (startX >= width)
				startX = width - 1;
			msg = GetDlgItem(hDlg, IDC_TESTSTARTY);
			GetWindowText(msg, string, sizeof(string));
			startY = (U32)atoi(string);
			if (startY < 0)
				startY = 0;
			else if (startY >= height)
				startY = height - 1;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			setEnd = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


// Message handler for getting a random seed
INT_PTR CALLBACK GetRandomSeed(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND hwndOwner;
	RECT rcOwner, rcWindow;
	int width, widthOwner;
	HWND msg;
	char string[32];

	switch (message)
	{
	case WM_INITDIALOG:
		if ((hwndOwner = GetParent(hDlg)) == NULL)
			hwndOwner = GetDesktopWindow();
		GetWindowRect(hwndOwner, &rcOwner);
		widthOwner = rcOwner.right - rcOwner.left;
		GetWindowRect(hDlg, &rcWindow);
		width = rcWindow.right - rcWindow.left;
		SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (widthOwner - width) / 2, rcOwner.top + 50, 0, 0, SWP_NOSIZE);
		msg = GetDlgItem(hDlg, IDC_RANDOMSEED);
		SetWindowText(msg, "");
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			int base = 10;
			msg = GetDlgItem(hDlg, IDC_RANDOMSEED);
			GetWindowText(msg, string, sizeof(string));
			if (string[0] == '0' && string[1] == 'x')
				base = 16;
			seed = strtoull(string, NULL, base);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


// Message handler for getting the starting position
INT_PTR CALLBACK GetThreshold(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND hwndOwner;
	RECT rcOwner, rcWindow;
	int widthW, widthOwner;
	HWND msg;
	char string[16];

	switch (message)
	{
	case WM_INITDIALOG:
		if ((hwndOwner = GetParent(hDlg)) == NULL)
			hwndOwner = GetDesktopWindow();
		GetWindowRect(hwndOwner, &rcOwner);
		widthOwner = rcOwner.right - rcOwner.left;
		GetWindowRect(hDlg, &rcWindow);
		widthW = rcWindow.right - rcWindow.left;
		SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (widthOwner - widthW) / 2, rcOwner.top + 50, 0, 0, SWP_NOSIZE);
		msg = GetDlgItem(hDlg, IDC_THRESHOLD);
		sprintf(string, "%.3f", guessThreshold);
		SetWindowText(msg, string);
		CheckDlgButton(hDlg, IDC_USEJARSUPERLOCMETHOD, (useNewSuperLocMethod < 0 ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_USEOLDSUPERLOCMETHOD, (useNewSuperLocMethod == 0 ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_USENEWSUPERLOCMETHOD, (useNewSuperLocMethod > 0 ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_USESUPERLOCTHRESHOLD, (useSuperLocMultiplier ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTTOPTOBOTTOM, (topToBottomPriority  ? BST_CHECKED : BST_UNCHECKED));
		CheckDlgButton(hDlg, IDC_TESTLEFTTORIGHT, (!topToBottomPriority ? BST_CHECKED : BST_UNCHECKED));
		msg = GetDlgItem(hDlg, IDC_MULIPLIER);
		sprintf(string, "%.1f", clearMultiplier);
		SetWindowText(msg, string);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_USEJARSUPERLOCMETHOD)
		{
			useNewSuperLocMethod = -1;
			CheckDlgButton(hDlg, IDC_USEJARSUPERLOCMETHOD, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_USEOLDSUPERLOCMETHOD, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_USENEWSUPERLOCMETHOD, BST_UNCHECKED);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_USEOLDSUPERLOCMETHOD)
		{
			useNewSuperLocMethod = 0;
			CheckDlgButton(hDlg, IDC_USEJARSUPERLOCMETHOD, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_USEOLDSUPERLOCMETHOD, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_USENEWSUPERLOCMETHOD, BST_UNCHECKED);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_USENEWSUPERLOCMETHOD)
		{
			useNewSuperLocMethod = 1;
			CheckDlgButton(hDlg, IDC_USEJARSUPERLOCMETHOD, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_USEOLDSUPERLOCMETHOD, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_USENEWSUPERLOCMETHOD, BST_CHECKED);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_USESUPERLOCTHRESHOLD)
		{
			useSuperLocMultiplier = !useSuperLocMultiplier;
			CheckDlgButton(hDlg, IDC_USESUPERLOCTHRESHOLD, (useSuperLocMultiplier ? BST_CHECKED : BST_UNCHECKED));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTTOPTOBOTTOM)
		{
			topToBottomPriority = true;
			CheckDlgButton(hDlg, IDC_TESTTOPTOBOTTOM, (topToBottomPriority ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hDlg, IDC_TESTLEFTTORIGHT, (!topToBottomPriority ? BST_CHECKED : BST_UNCHECKED));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_TESTLEFTTORIGHT)
		{
			topToBottomPriority = false;
			CheckDlgButton(hDlg, IDC_TESTTOPTOBOTTOM, (topToBottomPriority ? BST_CHECKED : BST_UNCHECKED));
			CheckDlgButton(hDlg, IDC_TESTLEFTTORIGHT, (!topToBottomPriority ? BST_CHECKED : BST_UNCHECKED));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDOK)
		{
			msg = GetDlgItem(hDlg, IDC_THRESHOLD);
			GetWindowText(msg, string, sizeof(string));
			guessThreshold = atof(string);
			if (guessThreshold < 0.90)
				guessThreshold = 0.90;
			else if (guessThreshold >= 1,0)
				guessThreshold = 1.0;
			msg = GetDlgItem(hDlg, IDC_MULIPLIER);
			GetWindowText(msg, string, sizeof(string));
			clearMultiplier = atof(string);
			if (clearMultiplier < 0.0)
				clearMultiplier = 0.0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


// Message handler for getting maxSolutions
INT_PTR CALLBACK GetMaxSolutions(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND hwndOwner;
	RECT rcOwner, rcWindow;
	int width, widthOwner;
	HWND msg;
	char string[16];

	switch (message)
	{
	case WM_INITDIALOG:
		if ((hwndOwner = GetParent(hDlg)) == NULL)
			hwndOwner = GetDesktopWindow();
		GetWindowRect(hwndOwner, &rcOwner);
		widthOwner = rcOwner.right - rcOwner.left;
		GetWindowRect(hDlg, &rcWindow);
		width = rcWindow.right - rcWindow.left;
		SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (widthOwner - width) / 2, rcOwner.top + 50, 0, 0, SWP_NOSIZE);
		msg = GetDlgItem(hDlg, IDC_SETMAXSOLUTIONS);
		sprintf(string, "%d", maxSolutions);
		SetWindowText(msg, string);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			msg = GetDlgItem(hDlg, IDC_SETMAXSOLUTIONS);
			GetWindowText(msg, string, sizeof(string));
			maxSolutions = (U32)atoi(string);
			if (maxSolutions < SMALL_SOLUTIONS)
				maxSolutions = SMALL_SOLUTIONS;
			else if (maxSolutions > MAX_SOLUTIONS)
				maxSolutions = MAX_SOLUTIONS;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	HWND hwndOwner;
	RECT rcOwner, rcWindow;
	int width, widthOwner;

	switch (message)
	{
	case WM_INITDIALOG:
		if ((hwndOwner = GetParent(hDlg)) == NULL)
			hwndOwner = GetDesktopWindow();
		GetWindowRect(hwndOwner, &rcOwner);
		widthOwner = rcOwner.right - rcOwner.left;
		GetWindowRect(hDlg, &rcWindow);
		width = rcWindow.right - rcWindow.left;
		SetWindowPos(hDlg, HWND_TOP, rcOwner.left + (widthOwner - width) / 2, rcOwner.top + 50, 0, 0, SWP_NOSIZE);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char fname[MAX_PATH];
	HMENU hMenu = GetMenu(hWnd);							
	HMENU hSubMenu, hSubMenu2;

	switch (message)
	{
	case WM_CREATE:
		hBackgndExpert = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGNDEXPERT));
		hBackgndMedium = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGNDMEDIUM));
		hBackgndSmall = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGNDSMALL));
		hMine = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_MINE));
		hNumbers = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_NUMBERS));
		hDead = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_DEAD));
		hSimilar = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SIMILAR));
		hSuper = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_SUPER));
		hBest = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BEST));
		if (hBackgndExpert == NULL
		|| hBackgndMedium == NULL
		|| hBackgndSmall == NULL
		|| hMine == NULL
		|| hNumbers == NULL
		|| hDead == NULL
		|| hSimilar == NULL
		|| hSuper == NULL
		|| hBest == NULL)
			MessageBox(hWnd, "Could not load art!", "Error", MB_OK | MB_ICONEXCLAMATION);
		RandomInit();										// init random numbers
		hSubMenu = GetSubMenu(hMenu, SUBMENU_PLAY);
		TimeInit();
		HashInit();
		maxSolutions = DEFAULT_SOLUTIONS;
		ClearStats();
		ClearTotalStats();
		setStart = 1;
		setEnd = 0;
		bestx = -1;
		guessThreshold = GUESS_THRESHOLD;
		clearMultiplier = CLEAR_MULTIPLIER;
		topToBottomPriority = true;
		saveSetResults = false;		
		saveTiming = false;
		saveInteresting = false;
		saveGuess0Fails = false;
		saveGameTreeFails = false;
		saveSuperLocFails = false;
		saveSuperLocMultiplierChanges = false;
		useNewSuperLocMethod = -1;						// default ot jar method
		useSuperLocMultiplier = true;
		hSubMenu = GetSubMenu(hMenu, SUBMENU_PLAY);
		saveGameTreeResults = 0;
		saveSuperLocResults = 0;				
		hSubMenu = GetSubMenu(hMenu, SUBMENU_PLAY);
		hSubMenu2 = GetSubMenu(hSubMenu, SUBSUBMENU_GAMETREE);
		CheckMenuItem(hSubMenu2, saveGameTreeResults, MF_BYPOSITION | MF_CHECKED);
		hSubMenu2 = GetSubMenu(hSubMenu, SUBSUBMENU_SUPERLOC);
		CheckMenuItem(hSubMenu2, saveSuperLocResults, MF_BYPOSITION | MF_CHECKED);
		testRunCount = 1;
		testing = 0;
		seed = 0;										// no seed set
		cheating = false;
		CheckMenuItem(hSubMenu, SUBMENU_CHEAT, MF_BYPOSITION | MF_UNCHECKED);
		backup = false;
		EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_GRAYED);
		started = false;
		width = MAX_WIDTH;
		height = MAX_HEIGHT;
		startX = start30x16X;
		startY = start30x16Y;
		minesStart = 99;
		listNext = 0;
		if (LoadGameFile())								// load previous game if possible
		{
			if (width == 30)							// set default starting positions
			{
				startX = start30x16X;
				startY = start30x16Y;
			}
			else if (width == 16)
			{
				startX = start16x16X;
				startY = start16x16Y;
			}
			else
			{
				startX = start9x9X;
				startY = start9x9Y;
			}
		}
		else
			NewGame(startX, startY);						// start new game
		if (remainingCount == 0)
		{
			gameOver = true;
			EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_GRAYED);
		}
		else
		{
			EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_ENABLED);
		}
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case IDT_TESTTIMER:
			if (solving)									// not done wait until we are
				break;
			KillTimer(hWnd, IDT_TESTTIMER);
NEXTSET:	saveSeed = Random2(0);
			RandomSeed(saveSeed);
			NewGame(startX, startY);						// start next puzzle
			testTotal++;
			solving = true;
			while (solving && gameOver == false)
			{
				backup = false;
				MakeMove();									// make some move until done
				if (backup == false)						// no move found 
					break;
			}
			solving = false;
			SaveResults(testResults);						// save results of the puzzle including errors
			for (int i = 0; i < MAX_RULES; i++)
				rulesCountTotal[i] += rulesCount[i];		// save totals
			RedrawWindow(hWnd, &TestRect, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			if (++testing <= testingMax)
			{
				goto NEXTSET;
			}
			else
			{
				ShowTestResults(testResults, testingMax, saveSeed);	// save stats 
				SaveSetResults(testAllResults, setsUsed);	// save results of the current set
				if (GetNextTest())							// set up for next set of tests
				{
					SetTimer(hWnd, IDT_TESTTIMER, SPEED, (TIMERPROC)NULL);
					break;
				}
#if SHOW_GAMETREE_TIMES
				fclose(gametreeTimes);
				gametreeTimes = NULL;
#endif
				testing = 0;								// end testing
				startX = saveStartX;
				startY = saveStartY;
				if (testAllResults != NULL)
					fclose(testAllResults);
				testAllResults = NULL;
				if (gameOver)
				{
					EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_GRAYED);
				}
				else
				{
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_ENABLED);
				}
				if (backup)
					EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_ENABLED);
				else
					EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_GRAYED);
			}
			break;
		case IDT_TIMER:
			MakeMove();										// make some move
			if (solving == false)
			{
				KillTimer(hWnd, IDT_TIMER);
				hSubMenu = GetSubMenu(hMenu, SUBMENU_PLAY);
				CheckMenuItem(hSubMenu, SUBMENU_SOLVE, MF_BYPOSITION | MF_UNCHECKED);
				if (gameOver)
				{
					EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_GRAYED);
				}
				else
				{
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_ENABLED);
				}
				if (backup)
					EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_ENABLED);
				else
					EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_GRAYED);
			}
			RedrawWindow(hWnd, &Interior, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		if (gameOver == false)
		{
			bestx = -1;									// turn off automatic detail
			superNext = 0;								// no SuperLocArray entries
			gameTreeNext = 0;							// no game tree entries		
			int x, y;
			if (FindPosition(hWnd, (int)(lParam & 0xffff), (int)(lParam >> 16), x, y))
			{
				if (exposed[y][x] == EXPOSE_MINE)		// remove mine
				{
					mineCount++;
					exposed[y][x] = -1;
				}
				else if (exposed[y][x] >= 0)			// do nothing if already exposed
					break;
				lastRuleUsed = -1;
				SaveBackUp();							// backup before exposing tile
				Expose(x, y);							// expose it now
				if (gameOver)
				{
					EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_GRAYED);
				}
				else
				{
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_ENABLED);
				}
				EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_ENABLED);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
			}
		}
		break;
	case WM_RBUTTONDOWN:
		if (gameOver == false)
		{
			bestx = -1;									// turn off automatic detail
			superNext = 0;								// no SuperLocArray entries
			gameTreeNext = 0;							// no game tree entries		
			int x, y;
			if (FindPosition(hWnd, (int)(lParam & 0xffff), (int)(lParam >> 16), x, y))
			{
				if (exposed[y][x] == EXPOSE_MINE)		// remove mine
				{
					lastRuleUsed = -1;
					SaveBackUp();						// backup before exposing tile
					mineCount++;
					exposed[y][x] = -1;
					RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				}
				else if (exposed[y][x] < 0)
				{
					lastRuleUsed = -1;
					SaveBackUp();						// backup before exposing tile
					mineCount--;
					exposed[y][x] = EXPOSE_MINE;
					RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				}
				EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_ENABLED);
			}
		}
		break;
	case WM_COMMAND:
	{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_NEW_EXPERT:
				if (testing)
					break;
				width = MAX_WIDTH;
				height = MAX_HEIGHT;
				minesStart = 99;
				startX = start30x16X;
				startY = start30x16Y;
				goto NEW;
			case IDM_NEW_MEDIUM:
				if (testing)
					break;
				width = 16;
				height = 16;
				minesStart = 40;
				startX = start16x16X;
				startY = start16x16Y;
				goto NEW;
			case IDM_NEW_SMALL:
				if (testing)
					break;
				width = 9;
				height = 9;
				minesStart = 10;
				startX = start9x9X;
				startY = start9x9Y;
NEW:			NewGame(startX, startY);					// start new game
LOAD:			lastRuleUsed = -1;
				listNext = 0;								// remove similar and dead 
				EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);		// enable save game now
				seed = 0;									// clear seed
				if (remainingCount == 0)
				{
					gameOver = true;
					EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_GRAYED);
				}
				else
				{
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_ENABLED);
				}
				EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_GRAYED);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				break;
			case IDM_LOAD:
				if (testing)
					break;
				solving = false;
				automatic = false;
				bestx = -1;
				listNext = 0;							// do not draw automatic data
				if (LoadFileName(hWnd, fname, "Text file", "txt"))
				{
					if (LoadPuzzle(fname))
						goto LOAD;
					else
					{
						MessageBox(hWnd, "Could not load puzzle!", "Data Error", MB_OK | MB_ICONEXCLAMATION);
						goto NEW;						// minesStart, width and height not changed
					}
				}
				break;
			case IDM_SAVE:
				if (testing)
					break;
				if (started == false
				|| gameOver == true)
					break;
				fname[0] = 0;
				if (SaveFileName(hWnd, fname, "Text file", "txt"))
				{
					SavePuzzle(fname, true);
				}
				break;
			case IDM_AUTOMATIC:
				if (testing)
					break;
				if (solving == false)
					automatic = true;
			case IDM_SOLVE:
				if (testing)
					break;
				if (solving)
				{
					solving = false;
					KillTimer(hWnd, IDT_TIMER);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_ENABLED);
					hSubMenu = GetSubMenu(hMenu, SUBMENU_PLAY);
					CheckMenuItem(hSubMenu, SUBMENU_SOLVE, MF_BYPOSITION | MF_UNCHECKED);
					if (backup)
						EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_ENABLED);
					else
						EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_GRAYED);
				}
				else
				{
					solving = true;
					SetTimer(hWnd, IDT_TIMER, SPEED, (TIMERPROC)NULL);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_GRAYED);
					hSubMenu = GetSubMenu(hMenu, SUBMENU_PLAY);
					CheckMenuItem(hSubMenu, SUBMENU_SOLVE, MF_BYPOSITION | MF_CHECKED);
				}
				break;
			case IDM_NEXTMOVE:
				if (testing)
					break;
				if (started == false						// just in case
				|| gameOver)
					break;
				if (saveGameTreeResults > 0)
				{
					char string[64];
					sprintf(string, "gameTreeResult v%s.txt", VERSION);
					gametreeResults = fopen(string, "w");
				}
				if (saveSuperLocResults > 0)
				{
					char string[64];
					char * method;
					if (useNewSuperLocMethod < 0)
						method = "jar";
					else if (useNewSuperLocMethod == 0)
						method = "old";
					else
						method = "new";
					sprintf(string, "superLocResults v%s %s%s.txt",
						VERSION, method,
						(useSuperLocMultiplier ? "+" : ""));
					superLocResults = fopen(string, "w");
				}
				solving = true;								// stop game if we make a mistake
				MakeMove();									// make some move
				solving = false;
				if (gameOver)
				{
					EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_GRAYED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_GRAYED);
				}
				else
				{
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_ENABLED);
				}
				if (backup)
					EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_ENABLED);
				else
					EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_GRAYED);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				if (gametreeResults != NULL)
				{
					fclose(gametreeResults);
					gametreeResults = NULL;
				}
				if (superLocResults != NULL)
				{
					fclose(superLocResults);
					superLocResults = NULL;
				}
				break;
			case IDM_STATS:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_STATSBOX), hWnd, Statistics);
				break;
			case IDM_SETSEED:
				seed = 0;
				DialogBox(hInst, MAKEINTRESOURCE(IDD_RANDOMSEEDBOX), hWnd, GetRandomSeed);
				if (seed != 0)
					RandomSeed(seed);
				break;
			case IDM_SETTHRESHOLD:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTHRESHOLDBOX), hWnd, GetThreshold);
				break;
			case IDM_SETMAXSOLUTIONS:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_SETMAXSOLUTIONSBOX), hWnd, GetMaxSolutions);
				break;
			case IDM_TESTRUN:
				if (testing != 0)							// skip if testing progress	
				{
					testingMax = testing;					// stop current test
					setEnd = setsUsed;
					break;
				}
				DialogBox(hInst, MAKEINTRESOURCE(IDD_TESTRUNBOX), hWnd, GetTestParameters);
				if (setEnd == 0)							// cancelled test
					break;
				setStart = (int)seed;						// reset starting set number
				saveStartX = startX;
				saveStartY = startY;
				solving = false;							// turn off any solving
				SetTestingFiles(wmId);						// set up test files and paths
				if (testAllResults == NULL)					// file open failed
				{
					MessageBox(hWnd, "Could not open results file!", "Error", MB_OK | MB_ICONEXCLAMATION);
					break;
				}
				ClearTotalStats();							// clear all total stats
				saveSeed = seed;							// used later in the results file
				StartTest();								// set up test files and start test
				SetTimer(hWnd, IDT_TESTTIMER, SPEED, (TIMERPROC)NULL);
				break;
			case IDM_BACKUP:
				if (testing)
					break;
				if (BackUp())								// backup occurred
				{
					EnableMenuItem(hMenu, IDM_SAVE, MF_BYCOMMAND | MF_ENABLED);		// game over could be cleared
					EnableMenuItem(hMenu, IDM_SOLVE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_AUTOMATIC, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_NEXTMOVE, MF_BYCOMMAND | MF_ENABLED);
					EnableMenuItem(hMenu, IDM_BACKUP, MF_BYCOMMAND | MF_GRAYED);
					RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				}
				break;
			case IDM_HFLIP:
				if (testing)
					break;
				listNext = 0;								// turn off dead, similar and super
				for (int y = 0; y < height; y++)
				{
					for (int x = 0; x < width/2; x++)
					{
						int temp = puzzle[y][width - 1 - x];
						puzzle[y][width - 1 - x] = puzzle[y][x];
						puzzle[y][x] = temp;
						temp = exposed[y][width - 1 - x];
						exposed[y][width - 1 - x] = exposed[y][x];
						exposed[y][x] = temp;
						temp = lastExposed[y][width - 1 - x];
						lastExposed[y][width - 1 - x] = lastExposed[y][x];
						lastExposed[y][x] = temp;
					}
				}
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				break;
			case IDM_VFLIP:
				if (testing)
					break;
				listNext = 0;								// turn off dead, similar and super
				for (int y = 0; y < height/2; y++)
				{
					for (int x = 0; x < width; x++)
					{
						int temp = puzzle[height - 1 - y][x];
						puzzle[height - 1 - y][x] = puzzle[y][x];
						puzzle[y][x] = temp;
						temp = exposed[height - 1 - y][x];
						exposed[height - 1 - y][x] = exposed[y][x];
						exposed[y][x] = temp;
						temp = lastExposed[height - 1 - y][x];
						lastExposed[height - 1 - y][x] = lastExposed[y][x];
						lastExposed[y][x] = temp;
					}
				}
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				break;
			case IDM_CHEAT:
				cheating = !cheating;						// turn on/off cheats
				hSubMenu = GetSubMenu(hMenu, SUBMENU_PLAY);
				if (cheating)
					CheckMenuItem(hSubMenu, SUBMENU_CHEAT, MF_BYPOSITION | MF_CHECKED);
				else
					CheckMenuItem(hSubMenu, SUBMENU_CHEAT, MF_BYPOSITION | MF_UNCHECKED);
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				break;
			case IDM_ANALYZEGAMETREE0:
			case IDM_ANALYZEGAMETREE1:
			case IDM_ANALYZEGAMETREE2:
			case IDM_ANALYZEGAMETREE3:
				hSubMenu = GetSubMenu(hMenu, SUBMENU_PLAY);
				hSubMenu2 = GetSubMenu(hSubMenu, SUBSUBMENU_GAMETREE);
				CheckMenuItem(hSubMenu2, saveGameTreeResults, MF_BYPOSITION | MF_UNCHECKED);
				saveGameTreeResults = wmId - IDM_ANALYZEGAMETREE0;
				CheckMenuItem(hSubMenu2, saveGameTreeResults, MF_BYPOSITION | MF_CHECKED);
				break;
			case IDM_ANALYZESUPERLOC0:
			case IDM_ANALYZESUPERLOC1:
			case IDM_ANALYZESUPERLOC2:
			case IDM_ANALYZESUPERLOC3:
				hSubMenu = GetSubMenu(hMenu, SUBMENU_PLAY);
				hSubMenu2 = GetSubMenu(hSubMenu, SUBSUBMENU_SUPERLOC);
				CheckMenuItem(hSubMenu2, saveSuperLocResults, MF_BYPOSITION | MF_UNCHECKED);
				saveSuperLocResults = wmId - IDM_ANALYZESUPERLOC0;
				CheckMenuItem(hSubMenu2, saveSuperLocResults, MF_BYPOSITION | MF_CHECKED);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			Draw(hWnd, hdc, ps);
			EndPaint(hWnd, &ps);
			DeleteDC(hdc);
		}
		break;
	case WM_DESTROY:
		if (testResults != NULL)
			fclose(testResults);
		testResults = NULL;
		if (testAllResults != NULL)
			fclose(testAllResults);
		testAllResults = NULL;
		if (gametreeResults != NULL)
			fclose(gametreeResults);
		gametreeResults = NULL;
		if (superLocResults != NULL)
			fclose(superLocResults);
		superLocResults = NULL;
		if (gametreeTimes != NULL)
			fclose(gametreeTimes);
		gametreeTimes = NULL;
		if (gameOver == false)
			SaveGameFile();
		DeleteObject(hBackgndExpert);
		DeleteObject(hBackgndMedium);
		DeleteObject(hBackgndSmall);
		DeleteObject(hMine);
		DeleteObject(hNumbers);
		DeleteObject(hDead);
		DeleteObject(hSimilar);
		DeleteObject(hSuper);
		DeleteObject(hBest);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINESWEEPER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MINESWEEPER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


//
//	 Center the window in relation to the desktop 
//
void CenterWindow(HWND CW_h2)
{
	RECT CW_rect1, CW_rect2;
	int CW_midx, CW_midy, CW_wx, CW_wy;
	HWND CW_h1;

	CW_h1 = GetDesktopWindow();
	GetWindowRect(CW_h1, &CW_rect1);
	GetWindowRect(CW_h2, &CW_rect2);
	CW_midx = (CW_rect1.right + CW_rect1.left) >> 1;
	CW_midy = (CW_rect1.bottom + CW_rect1.top) >> 1;
	CW_wx = CW_rect2.right - CW_rect2.left;
	CW_wy = CW_rect2.bottom - CW_rect2.top;
	MoveWindow(CW_h2, CW_midx - (CW_wx >> 1), CW_midy - (CW_wy >> 1), CW_wx, CW_wy, false);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_SYSMENU,
		0, 0, BACKGND_WIDTH + 16, BACKGND_HEIGHT + 59, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}
	CenterWindow(hWnd);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_MINESWEEPER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINESWEEPER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
