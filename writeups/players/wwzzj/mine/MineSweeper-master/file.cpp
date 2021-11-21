#include "stdafx.h"


int		winTotal;								// total number of wins in our run of 100
int		testTotal;								// total number of samples in our win of 100
int		winAllTotal;							// wins across all seeds
int		guess0Failed;
int		guess1Failed;
int		guess2Failed;
int		gameTreeFailed;
int		superLocFailed;
int		winsWithGuesses[MAX_WINS_WITH_GUESSES];
int		maxHashUsed;
int		maxHashHits;
double	winProb;								// sum of prob for all wins
double	lossProb;
double	guess0Prob;								// sum of all guess0 prob 
double	guess1Prob;								// sum of all guess1 prob 
double	guess2Prob;								// sum of all guess2 prob 
double	gameTreeProb;							// sum of all game tree prob 
double	superLocProb;							// sum of all super location prob
int		guessesPerWin;							// number of guesses per win
int		guessesPerLoss;
int		totalWins;								// number of wins and tests
int		totalTests;
int		totalGuess0;							// total guess0, guess1 guess2 used
int		totalGuess1;						
int		totalGuess2;
int		totalGameTree;
int		totalSuperLoc;
int		totalGuess0Failed;						// number of failed guesses
int		totalGuess1Failed;						
int		totalGuess2Failed;
int		totalGameTreeFailed;
int		totalSuperLocFailed;
int		totalGuessesPerWin;						// number of guesses per win
int		totalGuessesPerLoss;
int		totalWinsWithGuesses[MAX_WINS_WITH_GUESSES];
int		totalMaxHashUsed;
int		totalMaxHashHits;
double	totalWinProb;							// sum of prob for all wins
double	totalLossProb;
double	totalGuess0Prob;						// sum of all guess0 prob 
double	totalGuess1Prob;						// sum of all guess1 prob 
double	totalGuess2Prob;						// sum of all guess2 prob 
double	totalGameTreeProb;						// sum of all game tree prob
double	totalSuperLocProb;						// sum of all super location prob

static char * RuleText[MAX_RULES] = 
{
	"***** Mines *****",
	"***** Clears *****",
	"***** Deduced *****",
	"Guess 0",
	"GameTree",
	"SuperLocation",
	"Guess 1",
	"Guess 2",
};


void ClearTotalStats()
{
	totalWins = 0;
	totalTests = 0;
	totalGuess0 = 0;
	totalGuess1 = 0;
	totalGuess2 = 0;
	totalGameTree = 0;
	totalSuperLoc = 0;
	totalGuess0Failed = 0;
	totalGuess1Failed = 0;
	totalGuess2Failed = 0;
	totalGameTreeFailed = 0;
	totalSuperLocFailed = 0;
	totalGuessesPerWin = 0;
	totalGuessesPerLoss = 0;
	totalMaxHashUsed = 0;
	totalMaxHashHits = 0;
	totalWinProb = 0.0;
	totalLossProb = 0.0;
	totalGuess0Prob = 0.0;
	totalGuess1Prob = 0.0;
	totalGuess2Prob = 0.0;
	totalGameTreeProb = 0.0;
	totalSuperLocProb = 0.0;
	for (int i = 0; i < MAX_WINS_WITH_GUESSES; i++)
		totalWinsWithGuesses[i] = 0;
	totalMaxCounts = 0;							// max counts for solutions (see MAX_COUNTS)
	totalMaxPerimeter = 0;
	totalMaxSinglePerimeter = 0;
	totalMaxNeighbors = 0;
	totalMaxNeighbors;							// max number of entries in neighborArray 
	totalMaxLists = 0;							// max number of sequence lists (see MAX_LISTS)
	totalNumEndPlays = 0;						// number of times we use end play
	totalNumMissedEndPlays = 0;
	totalMaxEndPlayLength = 0;					// length of max end play
	totalNumEndPlaySmall = 0;					// count small end plays 
	totalNumEndPlayLarge = 0;					// count large end plays 
	totalMaxListSolutions = 0;					// max number of perimeter solutions
	totalMaxCompletedSolutions = 0;				// max number of perimeter solutions
	totalGametreeUsed = 0;						// number of times we used a gametree
	totalGametreeUsedEarly = 0;					// number of times we used a gametree early
	totalGametreeEntriesUsed = 0;				// max number of entries we used for a gametree
	totalGametreeHelped = 0;					// number of times gametree improved odds
	totalGametreeLinksNeeded = 0;				// number of game tree links needed 
	totalGametreeMaxDepth = 0;					// max depth of the gamet tree search
	totalSuperLocMaxCandidates = 0;				// max candidates found
	totalSuperLocMultiplierUsed = 0;
	totalSuperLocMultiplierImproved = 0;
	totalSuperLocMultiplierFailed = 0;
	totalSuperLocMultiplyUsed = 0;
}


void UpdateTotalStats(int tests)
{
	totalWins += winTotal;
	totalTests += tests;
	totalGuess0 += rulesCountTotal[GUESS_0];
	totalGuess1 += rulesCountTotal[GUESS_1];
	totalGuess2 += rulesCountTotal[GUESS_2];
	totalGameTree += rulesCountTotal[GAMETREE];
	totalSuperLoc += rulesCountTotal[SUPERLOC];
	totalGuess0Failed += guess0Failed;
	totalGuess1Failed += guess1Failed;
	totalGuess2Failed += guess2Failed;
	totalGameTreeFailed += gameTreeFailed;
	totalSuperLocFailed += superLocFailed;
	totalGuessesPerWin += guessesPerWin;
	totalGuessesPerLoss += guessesPerLoss;
	if (totalMaxHashUsed < maxHashUsed)
		totalMaxHashUsed = maxHashUsed;
	if (totalMaxHashHits < maxHashHits)
		totalMaxHashHits = maxHashHits;
	totalWinProb += winProb;
	totalLossProb += lossProb;
	totalGuess0Prob += guess0Prob;
	totalGuess1Prob += guess1Prob;
	totalGuess2Prob += guess2Prob;
	totalGameTreeProb += gameTreeProb;
	totalSuperLocProb += superLocProb;
	for (int i = 0; i < MAX_WINS_WITH_GUESSES; i++)
		totalWinsWithGuesses[i] += winsWithGuesses[i];
	if (totalMaxCounts < maxCounts)
		totalMaxCounts = maxCounts;
	if (totalMaxLists < maxLists)
		totalMaxLists = maxLists;
	if (totalMaxPerimeter < maxPerimeter)
		totalMaxPerimeter = maxPerimeter;
	if (totalMaxSinglePerimeter < maxSinglePerimeter)
		totalMaxSinglePerimeter = maxSinglePerimeter;
	if (totalMaxNeighbors < maxNeighbors)
		totalMaxNeighbors = maxNeighbors;
	totalNumEndPlays += numEndPlays;
	totalNumMissedEndPlays += numMissedEndPlays;
	if (totalMaxEndPlayLength < maxEndPlayLength)
		totalMaxEndPlayLength = maxEndPlayLength;
	totalNumEndPlaySmall += numEndPlaySmall;
	totalNumEndPlayLarge += numEndPlayLarge;
	if (totalMaxListSolutions < maxListSolutions)
		totalMaxListSolutions = maxListSolutions;
	if (totalMaxCompletedSolutions < maxCompletedSolutions)
		totalMaxCompletedSolutions = maxCompletedSolutions;
	totalGametreeUsed += gametreeUsed;
	totalGametreeUsedEarly += gametreeUsedEarly;
	if (totalGametreeEntriesUsed < gametreeEntriesUsed)
		totalGametreeEntriesUsed = gametreeEntriesUsed;
	if (totalGametreeLinksNeeded < gametreeLinksNeeded)
		totalGametreeLinksNeeded = gametreeLinksNeeded;
	totalGametreeHelped += gametreeHelped;
	if (totalGametreeMaxDepth < gametreeMaxDepth)
		totalGametreeMaxDepth = gametreeMaxDepth;
	if (totalSuperLocMaxCandidates < superLocMaxCandidates)
		totalSuperLocMaxCandidates = superLocMaxCandidates;
	totalSuperLocMultiplierUsed += superLocMultiplierUsed;
	totalSuperLocMultiplierImproved += superLocMultiplierImproved;
	totalSuperLocMultiplierFailed += superLocMultiplierFailed;
	totalSuperLocMultiplyUsed += superLocMultiplyUsed;
}


void SaveTotalResults(FILE * file)
{
	if (totalTests == 0)
		return;
	fprintf(file, "-----------------------\nStarting seed=%lld\n", startSeed);
	fprintf(file, "start=%d,%d\ngametree<=%d solutions\n", startX, startY, maxSolutions);
	fprintf(file, "threshold=%.3f\n", guessThreshold);
	fprintf(file, "end play<=%d\n", END_PLAY_COUNT);
	if (useNewSuperLocMethod < 0)
		fprintf(file, "Using jar method for linked clears\n");
	else if (useNewSuperLocMethod == 0)
		fprintf(file, "Using old method for linked clears\n");
	else
		fprintf(file, "Using new method for linked clears\n");
	if (useSuperLocMultiplier)
		fprintf(file, "SuperLoc clear multiplier=%.2f\n", clearMultiplier);
	else
		fprintf(file, "Not using the SuperLoc multiplier method!\n");
	if (topToBottomPriority)
		fprintf(file, "Using top to bottom then left to right for guesses!\n");
	else
		fprintf(file, "Using left to right then top to bottom for guesses!\n");
	fprintf(file, "-----------------------\nSamples: %d\nWon: %d\n", totalTests, totalWins);
	double win = (double)totalWins / totalTests;
	fprintf(file, "Percent: %.3f%%\n", 100.0 * win);
	double error = MARGIN_OF_ERROR * sqrt((1.0 - win) * win / totalTests);
	fprintf(file, "Margin of error: %.3f%%\n", 100.0 * error);
	if (totalGuess0 != 0)
	{
		fprintf(file, "Guess0 used: %d\n", totalGuess0);
		fprintf(file, "Guess0 failed: %d (%.2f%%)\n", totalGuess0Failed, (100.0 * totalGuess0Failed) / totalGuess0);
	}
	if (totalGuess1 != 0)
	{
		fprintf(file, "Guess1 used: %d\n", totalGuess1);
		fprintf(file, "Guess1 failed: %d (%.2f%%)\n", totalGuess1Failed, (100.0 * totalGuess1Failed) / totalGuess1);
	}
	if (totalGuess2 != 0)
	{
		fprintf(file, "Guess2 used: %d\n", totalGuess2);
		fprintf(file, "Guess2 failed: %d (%.2f%%)\n", totalGuess2Failed, (100.0 * totalGuess2Failed) / totalGuess2);
	}
	if (totalGameTree != 0)
	{
		fprintf(file, "GameTree used: %d\n", totalGameTree);
		fprintf(file, "GameTree failed: %d (%.2f%%)\n", totalGameTreeFailed, (100.0 * totalGameTreeFailed) / totalGameTree);
	}
	if (totalSuperLoc != 0)
	{
		fprintf(file, "Super Location used: %d\n", totalSuperLoc);
		fprintf(file, "Super Location failed: %d (%.2f%%)\n", totalSuperLocFailed, (100.0 * totalSuperLocFailed) / totalSuperLoc);
	}
	if (totalWins != 0)
		fprintf(file, "Average guesses per win: %.2f\n", (double)totalGuessesPerWin / totalWins);
	if ((totalTests - totalWins) != 0)
		fprintf(file, "Average guesses per loss: %.2f\n", (double)totalGuessesPerLoss / (totalTests - totalWins));
	if (totalWins != 0)
		fprintf(file, "Average win prob: %.2f%%\n", 100.0 * totalWinProb / totalWins);
	if ((totalTests - totalWins) > 0)
		fprintf(file, "Average loss prob: %.2f%%\n", 100.0 * totalLossProb / (totalTests - totalWins));
	if (totalGuess0 != 0)
		fprintf(file, "Average Guess0 prob: %.2f%%\n", (100.0 * totalGuess0Prob) / totalGuess0);
	if (totalGuess1 != 0)
		fprintf(file, "Average Guess1 prob: %.2f%%\n", (100.0 * totalGuess1Prob) / totalGuess1);
	if (totalGuess2 != 0)
		fprintf(file, "Average Guess2 prob: %.2f%%\n", (100.0 * totalGuess2Prob) / totalGuess2);
	if (totalGameTree != 0)
		fprintf(file, "Average GameTree prob: %.2f%%\n", (100.0 * totalGameTreeProb) / totalGameTree);
	if (totalSuperLoc != 0)
		fprintf(file, "Average Super Location prob: %.2f%%\n", (100.0 * totalSuperLocProb) / totalSuperLoc);
	fprintf(file, "Wins with guesses:\n     0     1     2     3     4     5     6     7     8     9    10+\n");
	for (int i = 0; i < MAX_WINS_WITH_GUESSES; i++)
	{
		fprintf(file, "%6d", totalWinsWithGuesses[i]);
	}
	fprintf(file, "\n");
	for (int i = 0; i < MAX_WINS_WITH_GUESSES; i++)
	{
		fprintf(file, " %4.1f%%", 100.0 * totalWinsWithGuesses[i] / totalTests);
	}
	fprintf(file, "\n\n");
	fprintf(file, "maxCounts=%d (allocated=%d)\nmaxLists=%d (allocated=%d)\n", totalMaxCounts, MAX_COUNTS, totalMaxLists, MAX_LISTS);
	fprintf(file, "maxPerimeter=%d (allocated=%d)\n", totalMaxPerimeter, MAX_LIST_LENGTH);
	fprintf(file, "maxSinglePerimeter=%d\n", totalMaxSinglePerimeter);
	fprintf(file, "maxNeighbors=%d (allocated=%d)\n", totalMaxNeighbors, MAX_NEIGHBORS_LIST);
	fprintf(file, "max number of perimeter solutions=%d (%d computed)\n", totalMaxListSolutions, totalMaxCompletedSolutions);
	fprintf(file, "number of end plays=%d\n", totalNumEndPlays);
	fprintf(file, "largest end play=%d (%d allocated) (%d missed)\n", totalMaxEndPlayLength, END_PLAY_COUNT, totalNumMissedEndPlays);
	fprintf(file, "number of end plays with > %d solutions=%d (%.2f%%)\n",
		maxSolutions, totalNumEndPlayLarge, (totalNumEndPlays > 0 ? 100.0 * totalNumEndPlayLarge / totalNumEndPlays : 0.0));
	fprintf(file, "number of end plays with <= %d solutions=%d (%.2f%%)\n",
		SMALL_SOLUTIONS, totalNumEndPlaySmall, (totalNumEndPlays > 0 ? 100.0 * totalNumEndPlaySmall / totalNumEndPlays : 0.0));
	fprintf(file, "game tree code used=%d\n", totalGametreeUsed);
	fprintf(file, "game tree code used early=%d\n", totalGametreeUsedEarly);
	fprintf(file, "game tree code helped=%d (%.2f%%)\n",
		totalGametreeHelped, (totalGametreeUsed > 0 ? 100.0 * totalGametreeHelped / totalGametreeUsed : 0.0));
	fprintf(file, "game tree hash table used=%d (%d allocated)\n", totalMaxHashUsed, HASH_ARRAY);
	fprintf(file, "game tree hash table hits=%d\n", totalMaxHashHits);
	fprintf(file, "game tree solution entries used=%d (%d allocated)\n", totalGametreeEntriesUsed, MAX_SOLUTION_VALUES);
	fprintf(file, "game tree solutions saved=%d (%d allocated)\n", totalGametreeLinksNeeded, MAX_SOLUTIONS);
	fprintf(file, "game tree max search depth=%d (%d allocated)\n", totalGametreeMaxDepth, MAX_DEPTH);
	fprintf(file, "super locations max candidates=%d (%d allocated)\n",
		totalSuperLocMaxCandidates, MAX_SUPERLOCATIONS);
	if (useSuperLocMultiplier)
	{
		fprintf(file, "super location multiplier method used=%d (improved=%d failed=%d)\n",
			totalSuperLocMultiplierUsed, totalSuperLocMultiplierImproved, totalSuperLocMultiplierFailed);
		fprintf(file, "super location multiply used=%d\n", totalSuperLocMultiplyUsed);
	}
	double time = TimeEndOverall();
	fprintf(file, "\ntime=%.3f seconds\n", time);
}


void SaveResults(FILE * file)
{
	if (gameOver)
	{
		if (remainingCount == 0)
		{
			int guesses = rulesCount[GUESS_0] + rulesCount[GUESS_1] + rulesCount[GUESS_2] 
				+ rulesCount[GAMETREE] + rulesCount[SUPERLOC];
			if (guesses >= MAX_WINS_WITH_GUESSES)
				guesses = MAX_WINS_WITH_GUESSES - 1;
			winsWithGuesses[guesses]++;
			winProb += prob;
			winTotal++;
			guessesPerWin += guesses;
			if (file != NULL)
				fprintf(file, "Puzzle%d prob=%3d%% Win!\n", testing, (int)(100 * prob));
		}
		else
		{
			int lastRule = lastRuleUsed;
			double finalProb = prob;
			lossProb += prob;
			BackUp();									// backup past error
			guessesPerLoss += guesses;
			if (lastRule == GUESS_0)
			{
				guess0Failed++;
				// ignore 2x1x1 case
				if (saveGuess0Fails
				&&  testAllResults != NULL
				&&  (mineCount != 1
				  || remainingCount != 1))
				{
					char testFileName[MAX_PATH];
					sprintf(testFileName, "%s\\Guess0 failed seed=0x%llx.txt", prefixString, saveSeed);
					SavePuzzle(testFileName);
				}
			}
			else if (lastRule == GUESS_1)
			{
				guess1Failed++;
				if (saveSuperLocFails
				&&  testAllResults != NULL)
				{
					char testFileName[MAX_PATH];
					sprintf(testFileName, "%s\\Guess1 failed seed=0x%llx.txt", prefixString, saveSeed);
					SavePuzzle(testFileName);
				}
			}
			else if (lastRule == GUESS_2)
			{
				guess2Failed++;
			}
			else if (lastRule == GAMETREE)
			{
				gameTreeFailed++;
				// ignore 2x2x2 case
				if (saveGameTreeFails
				&&  testAllResults != NULL
				&& (mineCount != 2
				  || remainingCount != 2))
				{
					char testFileName[MAX_PATH];
					sprintf(testFileName, "%s\\GameTree failed seed=0x%llx.txt", prefixString, saveSeed);
					SavePuzzle(testFileName);
				}
			}
			else if (lastRule == SUPERLOC)
			{
				superLocFailed++;
				if (saveSuperLocFails
				&&  testAllResults != NULL)
				{
					char testFileName[MAX_PATH];
					sprintf(testFileName, "%s\\SuperLoc failed seed=0x%llx.txt", prefixString, saveSeed);
					SavePuzzle(testFileName);
				}
			}
			if (file != NULL)
				fprintf(file, "Puzzle%d prob=%3d%% remaining=%d %s Failed\n",
					testing, (int)(100 * finalProb), remainingCount, RuleText[lastRule]);
		}
	}
	else if (file != NULL)
		fprintf(file, "ERROR: Puzzle%d prob=%3d%% ***** Missing Rule *****\n", testing, (int)(100 * prob));
	if (file == NULL)
		return;
	fprintf(file, "seed=0x%llx (%lld)\n", saveSeed, (S64)saveSeed);
	fprintf(file, "Rules  Mines Clears Deduce Guess0 GameTree SuperLoc Guess1 Guess2 \n     ");
	for (int i = 0; i < MAX_RULES; i++)
	{
		fprintf(file, "%7d", rulesCount[i]);
	}
	fprintf(file, "\n\n");
}


void SaveSetResults(FILE * file, int set)
{
	if (file == NULL)
		return;
	winAllTotal += winTotal;
	fprintf(file, "%d\t%d\n", set, winTotal);
}


void ShowTestResults(FILE * file, int tests, U64 seed)
{
	UpdateTotalStats(tests);
	if (file == NULL)
		return;
	int total = 0, totals[MAX_RULES];
	for (int i = 0; i < MAX_RULES; i++)
		totals[i] = 0;
	fprintf(file, "start=%d,%d\n", startX, startY);
	fprintf(file, "gametree<=%d solutions\n", maxSolutions);
	fprintf(file, "threshold=%.3f\n", guessThreshold);
	fprintf(file, "end play<=%d\n", END_PLAY_COUNT);
	if (useNewSuperLocMethod < 0)
		fprintf(file, "Using jar method for linked clears\n");
	else if (useNewSuperLocMethod == 0)
		fprintf(file, "Using old method for linked clears\n");
	else
		fprintf(file, "Using new method for linked clears\n");	if (useSuperLocMultiplier)
		fprintf(file, "SuperLoc clear multiplier=%.2f\n", clearMultiplier);
	else
		fprintf(file, "Not using the SuperLoc multiplier method!\n");
	if (topToBottomPriority)
		fprintf(file, "Using top to bottom then left to right for guesses!\n\n");
	else
		fprintf(file, "Using left to right then top to bottom for guesses!\n\n");

	fprintf(file, "Rules  Mines Clears Deduce Guess0 GameTree SuperLoc Guess1 Guess2\n     ");
	for (int i = 0; i < MAX_RULES; i++)
	{
		totals[i] += rulesCountTotal[i];
		total += totals[i];
		fprintf(file, "%7d", rulesCountTotal[i]);
	}
	fprintf(file, "\n      ");
	for (int i = 0; i < MAX_RULES; i++)
	{
		fprintf(file, "%5.1f%% ", 100.0 * rulesCountTotal[i] / total);
	}
	fprintf(file, "\n\n");

	double win = (double)winTotal / tests;
	fprintf(file, "Samples: %d\nWins: %d\nPercent: %.2f%%\n", tests, winTotal, 100.0 * win);
	double error = MARGIN_OF_ERROR * sqrt((1.0 - win) * win / tests);
	fprintf(file, "Margin of error: %.3f%%\n", 100.0 * error);

	if (rulesCountTotal[GUESS_0] != 0)
	{
		fprintf(file, "Guess0 used: %d\n", rulesCountTotal[GUESS_0]);
		fprintf(file, "Guess0 failed: %d (%d%%)\n", guess0Failed, (100 * guess0Failed) / rulesCountTotal[GUESS_0]);
	}
	if (rulesCountTotal[GUESS_1] != 0)
	{
		fprintf(file, "Guess1 used: %d\n", rulesCountTotal[GUESS_1]);
		fprintf(file, "Guess1 failed: %d (%d%%)\n", guess1Failed, (100 * guess1Failed) / rulesCountTotal[GUESS_1]);
	}
	if (rulesCountTotal[GUESS_2] != 0)
	{
		fprintf(file, "Guess2 used: %d\n", rulesCountTotal[GUESS_2]);
		fprintf(file, "Guess2 failed: %d (%d%%)\n", guess2Failed, (100 * guess2Failed) / rulesCountTotal[GUESS_2]);
	}
	if (rulesCountTotal[GAMETREE] != 0)
	{
		fprintf(file, "GameTree used: %d\n", rulesCountTotal[GAMETREE]);
		fprintf(file, "GameTree failed: %d (%d%%)\n", gameTreeFailed, (100 * gameTreeFailed) / rulesCountTotal[GAMETREE]);
	}
	if (rulesCountTotal[SUPERLOC] != 0)
	{
		fprintf(file, "Super Location used: %d\n", rulesCountTotal[SUPERLOC]);
		fprintf(file, "Super Location failed: %d (%d%%)\n", superLocFailed, (100 * superLocFailed) / rulesCountTotal[SUPERLOC]);
	}
	if (winTotal > 0)
		fprintf(file, "Average guesses per win: %4.2f\n", (double)guessesPerWin / winTotal);
	if (tests - winTotal > 0)
		fprintf(file, "Average guesses per loss: %4.2f\n", (double)guessesPerLoss / (tests - winTotal));
	if (winTotal > 0)
		fprintf(file, "Average win prob: %5.2f%%\n", 100.0 * winProb / winTotal);
	if (tests - winTotal > 0)
		fprintf(file, "Average loss prob: %5.2f%%\n", 100.0 * lossProb / (tests - winTotal));
	if (rulesCountTotal[GUESS_0] != 0)
		fprintf(file, "Average Guess0 prob: %5.2f%%\n", (100 * guess0Prob) / rulesCountTotal[GUESS_0]);
	if (rulesCountTotal[GUESS_1] != 0)
		fprintf(file, "Average Guess1 prob: %5.2f%%\n", (100 * guess1Prob) / rulesCountTotal[GUESS_1]);
	if (rulesCountTotal[GUESS_2] != 0)
		fprintf(file, "Average Guess2 prob: %5.2f%%\n", (100 * guess2Prob) / rulesCountTotal[GUESS_2]);
	if (rulesCountTotal[GAMETREE] != 0)
		fprintf(file, "Average GameTree prob: %5.2f%%\n", (100 * gameTreeProb) / rulesCountTotal[GAMETREE]);
	if (rulesCountTotal[SUPERLOC] != 0)
		fprintf(file, "Average Super Location prob: %5.2f%%\n", (100 * superLocProb) / rulesCountTotal[SUPERLOC]);
	fprintf(file, "Wins with guesses\n     0     1     2     3     4     5     6     7     8     9    10+\n");
	for (int i = 0; i < MAX_WINS_WITH_GUESSES; i++)
	{
		fprintf(file, "%6d", winsWithGuesses[i]);
	}
	fprintf(file, "\n");
	for (int i = 0; i < MAX_WINS_WITH_GUESSES; i++)
	{
		fprintf(file, " %4.2f%%", 100.0 * winsWithGuesses[i] / tests);
	}
	fprintf(file, "\n\n");
	fprintf(file, "maxCounts=%d (allocated=%d)\nmaxLists=%d (allocated=%d)\n", maxCounts, MAX_COUNTS, maxLists, MAX_LISTS);
	fprintf(file, "maxPerimeter=%d (allocated=%d)\n", maxPerimeter, MAX_LIST_LENGTH);
	fprintf(file, "maxSinglePerimeter=%d\n", maxSinglePerimeter);
	fprintf(file, "maxNeighbors=%d (allocated=%d)\n", maxNeighbors, MAX_NEIGHBORS_LIST);
	fprintf(file, "max number of perimeter solutions=%d (%d computed)\n", maxListSolutions, maxCompletedSolutions);
	fprintf(file, "number of end plays=%d\n", numEndPlays);
	fprintf(file, "largest end play=%d (%d allocated) (%d missed)\n", maxEndPlayLength, END_PLAY_COUNT, numMissedEndPlays);
	fprintf(file, "number of end plays with > %d solutions=%d (%.2f%%)\n",
		maxSolutions, numEndPlayLarge, (numEndPlays > 0 ? 100.0 * numEndPlayLarge / numEndPlays : 0.0));
	fprintf(file, "number of end plays with <= %d solutions=%d (%.2f%%)\n",
		SMALL_SOLUTIONS, numEndPlaySmall, (numEndPlays > 0 ? 100.0 * numEndPlaySmall / numEndPlays : 0.0));
	fprintf(file, "game tree code used=%d\n", gametreeUsed);
	fprintf(file, "game tree code used early=%d\n", gametreeUsedEarly);
	fprintf(file, "game tree code helped=%d (%.2f%%)\n",
		gametreeHelped, (gametreeUsed > 0 ? 100.0 * gametreeHelped / gametreeUsed : 0.0));
	fprintf(file, "game tree hash table used=%d (%d allocated)\n", maxHashUsed, HASH_ARRAY);
	fprintf(file, "game tree hash table hits=%d\n", maxHashHits);
	fprintf(file, "game tree solution entries used=%d (%d allocated)\n", gametreeEntriesUsed, MAX_SOLUTION_VALUES);
	fprintf(file, "game tree solutions saved=%d (%d allocated)\n", gametreeLinksNeeded, MAX_SOLUTIONS);
	fprintf(file, "game tree max search depth=%d (%d allocated)\n", gametreeMaxDepth, MAX_DEPTH);
	fprintf(file, "super locations max candidates=%d (%d allocated)\n",
		superLocMaxCandidates, MAX_SUPERLOCATIONS);
	if (useSuperLocMultiplier)
	{
		fprintf(file, "super location multiplierthod used=%d (improved=%d failed=%d)\n",
			superLocMultiplierUsed, superLocMultiplierImproved, superLocMultiplierFailed);
		fprintf(file, "super location multiply used=%d\n", superLocMultiplyUsed);
	}
	fclose(testResults);
	testResults = NULL;
}


bool LoadPuzzle(char * fname)
{
	FILE * file;
	int x, y;

	file = fopen(fname, "r");
	if (file != NULL)
	{
		int tempWidth, tempHeight, tempMines;
		int num = fscanf(file, "%dx%dx%d\n", &tempWidth, &tempHeight, &tempMines);
		if (tempWidth == 30)
		{
			if (tempHeight != 16)							// invalid size
			{
				fclose(file);
				return false;
			}
		}
		else if (tempWidth == 16)
		{
			if (tempHeight != 16)							// invalid size
			{
				fclose(file);
				return false;
			}
		}
		else if (tempWidth == 9)
		{
			if (tempHeight != 9)							// invalid size
			{
				fclose(file);
				return false;
			}
		}
		else												// invalid size
		{
			fclose(file);
			return false;
		}
		if (num == 3)
		{
			mineCount = tempMines;
			remainingCount = 0;
			int count = 0;
			for (y = 0; y < tempHeight; y++)
			{
				for (x = 0; x < tempWidth; x++)
				{
					int c = fgetc(file);
					if (c == EOF)
					{
						fclose(file);
						return false;
					}
					if (c == 'h')
					{
						remainingCount++;
						puzzle[y][x] = 0;
						exposed[y][x] = -1;
					}
					else if (c == 'm')
					{
						count++;
						puzzle[y][x] = 1;
						exposed[y][x] = -1;
					}
					else if (c == 'M')
					{
						mineCount--;
						puzzle[y][x] = 1;
						count++;
						exposed[y][x] = EXPOSE_MINE;
					}
					else
					{
						puzzle[y][x] = 0;
						exposed[y][x] = c - '0';
					}
				}
				fgetc(file);								// skip end of line
			}
			// in case we loaded the format without all the mines
			// flag this as invalid
			if (count != tempMines)
			{
				fclose(file);
				return false;
			}
			if (fscanf(file, "%lf", &prob) == EOF)			// if prob does not exist use 100%
				prob = 1.0;
		}
		fclose(file);
		minesStart = tempMines;
		width = tempWidth;
		height = tempHeight;
		lastRuleUsed = -1;
		lastProb = prob;
		started = true;
		backup = false;
		solving = false;
		automatic = false;
		bestx = -1;
		gameOver = false;
		for (int i = 0; i < MAX_RULES; i++)
			rulesCount[i] = 0;
		return true;
	}
	return false;
}


//
//
//
bool SavePuzzle(char * fname, bool saveProb)
{
	FILE * file;

	file = fopen(fname, "w");
	if (file != NULL)
	{
		fprintf(file, "%dx%dx%d\n", width, height, minesStart);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (exposed[y][x] == EXPOSE_MINE)
					fprintf(file, "M");
				else if (exposed[y][x] >= 0)
					fprintf(file, "%d", exposed[y][x]);
				else if (puzzle[y][x] > 0)
					fprintf(file, "m");
				else
					fprintf(file, "h");
			}
			fprintf(file, "\n");
		}
		if (saveProb)
			fprintf(file, "%f\n", prob);
		fclose(file);
		return true;
	}
	return false;
}


bool LoadGameFile()
{
	PWSTR	ppszPath;
	char	path[256];

	// get path to the system app data
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &ppszPath);
	for (int i = 0; i < 256; i++)
	{
		path[i] = (char)ppszPath[i];
		if (path[i] == 0)
			break;
	}
	// this should be C:\Users\Ed Logg's\AppData\Roaming\MineSweeper\MineSweeper.txt
	strncat(path, "\\MineSweeper\\MineSweeper.txt", MAX_PATH-1);
	if (LoadPuzzle(path))
	{
//		DeleteFile(path);							// do not delete file in case game crashes
		started = true;
		return true;
	}
	return false;
}


bool SaveGameFile()
{
	PWSTR	ppszPath;
	char	path[256];
	// get path to the system app data
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &ppszPath);
	for (int i = 0; i < 256; i++)
	{
		path[i] = (char)ppszPath[i];
		if (path[i] == 0)
			break;
	}
	// create directory if it does not exist
	strncat(path, "\\MineSweeper", MAX_PATH-1);
	CreateDirectory(path, NULL);
	strncat(path, "\\MineSweeper.txt", MAX_PATH-1);
	return SavePuzzle(path);
}


//	Windows does not permit the following characters in file names:
//	<, >, :, ", /, |, \, ?, *
//
void ValidateFileName(char * fname)
{
	while (*fname != 0)
	{
		if (*fname < ' ')
			*fname = ' ';
		else if (*fname > '~')
			*fname = ' ';
		else if (*fname == '<')
			*fname = ' ';
		else if (*fname == '>')
			*fname = ' ';
		else if (*fname == ':')
			*fname = ' ';
		else if (*fname == '"')
			*fname = ' ';
		else if (*fname == '/')
			*fname = ' ';
		else if (*fname == '|')
			*fname = ' ';
		else if (*fname == '\\')
			*fname = ' ';
		else if (*fname == '?')
			*fname = ' ';
		else if (*fname == '*')
			*fname = ' ';
		++fname;
	}
}


bool SaveFileName(HWND hwnd, char * fname, char * filterString, char * defaultExt)
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));
	ValidateFileName(fname);							// insure the file is valid

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = filterString;
	ofn.lpstrFile = fname;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = defaultExt;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
		return true;
	return false;
}


bool LoadFileName(HWND hwnd, char * fname, char * filterString, char * defaultExt)
{
	OPENFILENAME ofn;
	fname[0] = 0;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = filterString;
	ofn.lpstrFile = fname;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = defaultExt;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if (GetOpenFileName(&ofn))
		return true;
	return false;
}
