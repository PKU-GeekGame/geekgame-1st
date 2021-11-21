This game will create and try to solve a Minesweeper puzzles.
It will always start at the default location given the puzzle size.  
The starting location has been freed of all neighboring mines to give 
the player a free starting area (i.e. zero starting state).  We use 
a 64 bit KISS pseudo random number generator to create the puzzles.

The screen layout is somewhat different than the normal Window MineSweeper.
Besides the number of mines remaining, a percentage is listed instead of 
the usual time.  This percentage is the probability of success.  If no 
guess is involved the percentage is 100%.  Otherwise the probability of 
each guess is multiplied by the current probability.

Terminology:
tile/entries - one of the entries in the puzzle
exposed - when a tile is shown to be a number 0-8 or a mine
unexposed/hidden - when a tile is not known
clear - a tile is fae to be exposed 
deduce - is used to indicate a tile has been analyzed to always be a mine or clear
neighbor/witness - number of exposed tiles that are not mines next to this tile
needed - number of mines needed next to this tile
perimeter - edge of the exposed tiles (not counting mines)
list - list of perimeter tiles that are connected (i.e. share neighbors)
adjacent - unexposed tiles adjacent to the perimeter 
free/open - free tiles are unexposed tiles not adjacent or on the perimeter
linked - a linked tile is an adjacent perimeter tile that will be clear if this entry is
brute force - means look at all solutions for the best results
similar/boxes - entries with only one neighbor which we exclude from solution creation
gametree - method used to solve brute force positions
end play - when brute force can be used - limited by number of solutions 
	and number of entries remaining
super locations - those locations that seem so much better than others
dead entries - those with the same value for all solutions
	They are ignored as possible best moves because they will not help us in any way.
superset - Tile A is a Superset when there is another revealed tile whose adjacent tiles 
	are a subset of A's adjacent tiles.  I do not use this concept but the jar program 
	does to speed up analysis but it skips some important information.

There ar three sizes of puzzles:
Beginner or small: 9x9 tiles with 10 mines - one mine per 8.1 entries
Intermediate or medium: 16x16 tiles with 40 mines - one mine per 6.4 entries
Expert: 30x16 tiles with 99 mines - one mine per 4.85 entries


File Menu:
1. You can select the size of the puzzle as well as load and save puzzles.
2. Note the last puzzle is saved whatever you exit.  It is reloaded when
you restart the game.

Game Menu:
1. Solve will solve the puzzle the best it can, even guessing when it has to.
2. Automatic will solve the puzzle and stop when a guess is required.
It will show all the dead (black tiles), similar (red tiles), good moves (yellow)
as well as the best move (green).
3. Next move allows you to see one move at a time to see what it is doing.
4. Backup allows a one move backup, especially useful when undoing the last 
fatal move.
5. H and V Flip are provided to test some of the rules.
6. Cheat shows or hides all the mines.

Test Menu:
1. You can select a number of puzzles to create and solve with the current
settings of threshold, and max solutions for brute force.  It will 
create a folder name using the current date and any settings.  In this folder 
will be a results file for the whole test run as well as a folder for each
set of 100 puzzles tested.
2. The statistics are the results of the last test run.
3. You can set the random seed used for the test you are about to select.  
This is very useful to repeat the same puzzles with different parameters.
If no random seed is used, one is used based the time of day and processor clock.
4. Setting the threshold will allow the program to select tiles close to the best
probability of success.  In many cases entries with a good probability of success
will result in better progress than the best probability.  
5. Setting the max solutions sets the limits for using brute force.  The higher
the number the more time it takes.  It defaults to 1000 and the upper limit is 
4000.  I have set the max number of entries in end play to be 64.  If there are 
more entries than this brute force cannot be used.


David Hill using the Reddit name BinaryChop has the most interesting solver and the best 
analyzer I have seen.  Our discussion on Reddit can be found here:
https://www.reddit.com/r/Minesweeper/comments/8b3b30/odds_of_winning_at_minesweeper/
The analyzer can be downloaded from:
https://onedrive.live.com/?authkey=%21AF5%5FruylVZgH1Ig&id=8AE16DCF19DB24EB%213410&cid=8AE16DCF19DB24EB
The source code can be downloaded here:
https://github.com/DavidNHill/Minesweeper
A discussion of his algorithm can be found here:
https://github.com/DavidNHill/Minesweeper/wiki


The algorithm used to solve puzzles is:
Rule 1 "MINE": 
if the number of empty squares is equal to the number of missing mines, add the mines.
Rule 2 "CLEAR": 
If an entry has the right number of mines surrounding it, then expose any unexposed neighbors.
	
If the one of the above rules does create any obvious play, them I build a set of connected 
entries along the edge or perimeter into what I call a list.  This list will also have an 
associated list of neighbors or witnesses which control the probability of mines surrounding it.
See https://luckytoilet.wordpress.com/2012/12/23/2125/ for a visual description.
Often there will be more than one list.  Next we test all possible placements of mines in 
each list to get the number of mines used and the number of solutions for each count
of mines.  This article uses the wrong calculation for the probability of a mine being
at any particular location.  The correct calculation is described below.  This 
probability is used to determine the best plays as described below.

I have made an extension to the algorithm above.  First of all I check to make 
sure the number of mines never exceeds the remaining number.  For example, if there
are 6 mines left and 3 lists, the max allowed is 4 for each of the lists.
So we can discard any possibilities that do not match these conditions.
Second, if the total maximums for the sequences exceed the remaining mines,
I remove the solutions which have too many mines.  For example, suppose
there are 3 lists with min and max number of mines of 1-2, 3-4, and 2-2 
with 6 mines remaining.  Now we know the second sequences can have only 3 mines 
and the first sequence only 1 mine.  So we discard solutions that are impossible. 

I also use another method to reduce the number of solutions because it is often 
the case that the number of solutions is in the millions and this would take too 
long to create them all.  So I have what I call similar entries, those with only
one neighbor.  I remove these from the creation of the solutions.  Then I multiply
the probability of mines in these areas to calculate the number of solutions.
This reduces the solution count down to the thousands which is much more manageable.
In one example I reduced the solution count from 11,224,096 down to just above 2000.
There another method used by David Hill to reduce this number even further to 432 but
I have not implemented it.


Rule 3 "DEDUCE": 
if there are any perimeter entries where mines cannot occur, expose it.
If there are entries where a mine must always occur, place a mine there. 
If neither of the cases above occur we must guess.  

The correct calculation of probability comes from this article:
https://research.wsulibs.wsu.edu/xmlui/bitstream/handle/2376/2460/Fowler%2C%20A.%20and%20Young%20A.%20%20Minesweeper%20a%20statistical%20and%20computational.pdf?sequence=1&isAllowed=y
On pages 11 and 12 they give an example using just one sequence.  Using the following definitions:
NP = number of non-perimeter entries
M = number of mines remaining
m1,m2... = number of mines used in solutions
Comb(m,n) = combinations of m items with n places = m/1*(n-1)/2*...(m+1-n)/n
T = total number of solutions to the puzzle
T = SolutionCount(m1)*Comb(NP,M-m1) + SolutionCount(m2)*Comb(NP,M-m2) + ...
So in the example in the article above we have:
	M = 11 mines remaining
	4 solutions using m1 = 3 mines
	15 solutions using m2 = 4 mines
	NP = 21 unknown non-perimeter squares
	M = 11 mines remaining
	T = 4*Comb(21,8) + 15*Comb(21,7) = 2,558,160
The probability of a mine at x,y in the sequence is exactly:
P(x,y) = (SolutionCount(x,y,m1)*Comb(NP,M-m1) + SolutionCount(x,y,m2)*Comb(NP,M-m2)) / T
So in our example at (x,y) say we have 3 solutions using 3 mines and 9 solutions using 4 mines
P(x,y) = (3*Comb(21,8) + 9*Comb(21,7)) / 2,558,160 = (3*203,490 + 9*116,280) / 2,558,160 = 64.77%

The probablility of a mine in the non-perimeter area PNP can be calculated by:
TNP = total weighted count of a mine in the non-perimeter area
TNP = (M-m1)*SolutionCount(m1)*Comb(NP,M-m1) * (M-m2)*SolutionCount(m2)*Comb(NP,M-m2) + ...
PNP = TNP / (T * NP)
In the above example
PNP = (8*4*Comb(21,8) + 7*15*Comb(21,7)) / (T * 21) = 18,721,080 / 53,721,360 = 34.848%

When there are more than one sequence or perimeter area the calculations get more complicated.  
Assume SolutionCount(i,mi) = solution count using sequence i and mi mines.
T = Sum for all sequences i and all possible count of mines mi
= Sum of all SolutionCount(0,m0)*SolutionCount(1,m1)*...*Comb(NP,M-m0-m1-...)
If we define Tnoti = similar to T above except it excludes the term SolutionCount(i,mi)
P(x,y) = (Sum for all mi SolutionCount(x,y,mi)*Tnoti) / T
TNP = Sum of all mi ((M-m0-m1-...)*SolutionCount(0,m0)*SolutionCount(1,m1)*...*Comb(NP,M-m0-m1-...) 
PNP = TNP / (T * NP)


Rule 4 "Do or Die":
If we get situations where an area is completed surrounded and cannot be affected by any 
other neighbors, then we must guess.  The number of mines must always be fixed.  In most 
cases this is a 50-50 proposition.  We make this guess now instead of waiting.  However, 
there are cases when it is not a 50% guess such as the 66% chance or the 66% as shown here:
	MM3MM	MM4MM
	M---M	M---M
There are also cases when an area is dead meaning all entries have only one possible value.
Normally the brute force method will tell us any place is as good as another so we just guess.
In the case below the last entry has a 66% chance of success but we must guess again reducing
our odds to 33% that we will succeed.  Guessing the first location has a 33% chance of success
but will guarantee we always succeed.
	MM33M3
	M-----
There is another non-obvious special case that requires a little thought:
	XXMMM
	323*-
	-^1*-
You can guess to play at either unexposed entrymarked by *.  The reason is, if the 
unexposed entry marked by ^ is a mine then both entries marked by * are safe to expose.
If ^ is not a mine then we have our 50% guess.


Rule 5 "Brute Force" or "GameTree":
If we have a limited number of solutions remaining and a limited number of entries,
then we can use brute force to go through all solutions to get the best chance of success.
Currently the maximum size of area is limited to 64 entries.  The max number of solutions
can be set depending on how much CPU time you can afford.  The default is 1000 solutions.
We build a game tree to find the maximum number of solutions for exposing certain
entries.  Once the gametree is built we do not need to build it again, however, I 
do not reuse it.  There are often more than one solution but I pick the first one I 
found.  Even with pruning this method often takes a considerable amount of time.  I do use
a sort at each step to help pick the best choice first.  I start with the est odds of winning.
In case of a tie I use entry with the most solutions that will create some exposed tiles, 
this is a value that will expose all other tiles around it.  In case of a tie I use the
lowest value for the counts of solutions using each count of mines.  The later is used
to help reduce the search depth.  In case of ties I use the first entry encountered
start from left to right, top to bottom.

There is a short cut I also use which I call "early brute force" when we have an area 
where we have a fixed number of mines.  I can preform the brute force method on this 
portion of the puzzle without considering the rest of the puzzle.


Rule 6 "Super Locations":
We know the probability in the non-perimeter area as well as the entries along the
perimeter.  I use a threshold (default is 96%) from the best probability to select 
only the best entries for consideration.  I break entries into three classes: perimeter,
adjacent to perimeter, and open entries.  Some times all three classes of entries are
considered.  Other times only the adjacent and open areas are considered.  Any open
entry we can quickly calculate the odds of this entry being a zero by using the formulas 
above.  We can discard any open areas that are not the best resulting in the corners
or the edges being used.  Some adjacent entries it is possible to calculate the
odds because the perimeter it is adjcent to forces a fixed number of mines.  Otherwise
for the other adjacent and perimeter entries, we try the values we found when we determined 
all solutions originally and solve to see how many solutions remain.  The percentage of new
solutions divided by the number of solutions we calculated before gives us a percentage
which will call the "progress".  We will pick the entry with the highest chance of 
progress.  We also calculate the number of clears found in the new solutions.  This number
we use to calculate a "clear" value.  So in case of a tie in progress we pick the entry
with the highest clear value.  Some times entries are linked, meaning if one entry is exposed
then this forces other entries to be exposed.  This is ideal because it increases the clear
number.  Now if the best super location is in the open area the program will display
"Guess 1".  If there is only one solution along the perimeter the game will display
"Guess 2".  Otherwise "SuperLoc" will be displayed.  In all cases of a tie I use the first
entry I found starting from left to right, top to bottom.

Calculating Linked Clears:
There are three method of calculating linked clears in super locations.  The method used by 
the jar version of the game uses the nuber of exposed tiles.  It recurses through all linked 
entries using the probability for each value and the total number of clears.  The problem with
this method is it counts some clears twice when we create new perimeter entries.  The formula is:
	clears = SumForEachValue(ProductForEachLink(prob/progress) * (count-1)) + prob of success
	prob = probability for each possible value 
	count = count of clears
	we subrtract one from the count for the original entry
Since it is possible for each link to duplicate the same clears, I believe this calculation is not right.
So another method is to create a mask of all the clears and count the number to reduce clears that 
are created when we create new perimeterfor some super location values.  The "new" method does this 
and correctly counts the actual clear count.  However, I needed to add a little extra for linked
clears otherwise it would create poor results.  The "old" method (used in v1.2c) but it does not 
correctly count the number of new perimeter entries.  In fact it seems to downgrade these entries
but generate good results, so I left this method in but I do not use it.  The results are shown below.

Super Location Multiplier:
There are cases where the best super location progress is better but the probability of success
is a little less than another position so I try to determine if the best choice based on these
two percentages.  So I calculate the difference between the product of the two.  If the product
is higher and the clear count is higher use this entry.  If on the other hand the product is a 
little less but the clears are higher I may chose the alternative based on a multiplier.  
So for every percent difference I expect N times better for the alternate to be chosen.  
I designate this by adding a "+" to the method to show this has been enabled.  This method seems
to add just a small improvement.  The results are shown below.


Rule 7 "Guess 1":
It is possible we cannot build any lists because all free entries are surrounded
by mines.  In every case I have seen Brute Force will be used but it is possible
that it cannot.  I have not seen any such case yet but I leave this rule here just
in case we run into it.  


Margin or error calculation:
To calculate the marginof error I use the following formula:
MOE = K * sqrt(P * (1-P) / N)
N = number of samples
P = probabilty of success
K = constant used to determine the percentage
	99% = 2.58
	98% = 2.33
	95% = 1.96 (I use this value)
	90% = 1.645


STARTING POSITIONS:
Different starting locations generate different win percentages.  The results 
below used several seeds using 10,000 puzzles with different starting positions
and threshold=.96 on version 1.1 of the program.  Note, that the best starting 
position depends on the size of the puzzle. 

Beginner 9x9x10 using v1.1 (start=2,2 is best)
seed	1,1		2,1		3,1		2,2		3,2		3,3		MOE
-----------------------------------------------------------
1		96.83%	97.19%	97.12%	97.38%	97.26%	96.96%
2		96.96%	97.38%	97.17%	97.38%	97.20%	97.03%
3		96.78%	97.03%	97.09%	97.21%	97.18%	97.19%
4		96.63%	97.12%	97.17%	97.28%	97.09%	96.88%
5		96.82%	96.99%	96.94%	97.16%	97.07%	96.91%
-------------------------------------------------------
Avg		96.804%	97.114%	97.098%	97.282%	97.160%	96.994%	+-.145%


Intermediate 16x16x40 using v1.1 (start=2,2 is best)
seed	2,1		3,1		4,1		2,2		3,2		4,2		3,3		4,3		MOE
---------------------------------------------------------------------------
1		88.48%	88.41%	87.80%	88.70%	88.67%	87.92%	88.51%	87.89%
2		88.93%	89.00%	88.06%	89.33%	89.18%	88.37%	89.22%	88.51%
3		89.37%	89.49%	88.62%	89.77%	89.67%	89.14%	89.69%	89.09%
4		88.86%	88.95%	88.28%	89.25%	89.36%	88.39%	89.18%	88.36%
5		88.61%	88.63%	87.72%	89.14%	88.81%	87.72%	88.84%	87.91%
-------------------------------------------------------------------------------
Avg		88.850%	88.896%	88.096%	89.238%	89.138%	88.308% 89.088%	88.352%	+-.273%

Running 100,000 samples each to refine the starting position:
Seed  Jar1.05 3,2  jar1.05 2,2	start=3,2	start=2,2	MOE
1		88.976%		89.177%		89.023%		89.186%		+-.194%
2		89.175%		89.291%		89.253%		89.360%
3		89.149%		89.224%		89.166%		89.256%
4		89.065%		89.110%		89.085%		89.115%
5		89.200%		89.215%		89.192%		89.211%
6		89.082%		89.116%		89.095%		89.105%
7		89.096%		89.234%		89.125%		89.260%
8		88.890%		88.958%		88.922%		88.926%
9		88.981%		89.104%		89.082%		89.163%
10		89.191%		89.329%		89.226%		89.389%
---------------------------------------------------------------------------
Avg		89.081%		89.176%		89.117%		89.197%		+-.061% 


Expert 30x16x99 using v1.1 (start=3,3 best)
seed	2,2		3,2		4,2		2,3		3,3		4,3		2,4		3,4		4,4		MOE 
-----------------------------------------------------------------------------------
1		52.90%	52.97%	52.46%	53.17%	53.37%	52.63%	52.28%	52.48%	51.76%
2		54.00%	54.02%	52.97%	53.97%	54.00%	53.12%	53.00%	53.00%	52.67%
3		55.25%	55.55%	54.67%	55.36%	55.48%	54.39%	54.44%	54.73%	53.87%
4		53.33%	53.27%	52.74%	53.51%	53.49%	52.78%	52.60%	52.88%	52.19%
5		54.81%	54.44%	54.03%	54.62%	54.75%	53.67%	53.64%	53.88%	52.86%
---------------------------------------------------------------------------------------
Avg		54.058%	53.650%	53.374%	54.126%	54.218%	53.318%	53.192% 53.394%	52.670%	+-.436%

Using v1.1 of the program I ran 100,000 samples each to refine the starting position:
Seed	start=3,3	start=2,2   start=2,3	MOE		
1		54.118%		53.715%		53.962%		+-.309%	
2		53.955%		53.575%		53.846%		
3		53.966%		53.725%		53.870%		
4		53.900%		53.525%		53.857%		
5		53.926%		53.572%		53.799%		
---------------------------------------------------------------------------
Avg		54.007%		53.622%		53.867%		+-.138%


THRESHOLD:
Using v1.2c old+ of the program I used the expert game and several samples of 100,000 
puzzles using Expert 30x16x99 starting at 3,3 using 1000 solutions for brute force (BF)
with the following values for the super location threshold (.96 seems best):
Seed	0.965		0.960		0.955		0.950		MOE		
1		54.064%		54.129%		54.133%		54.132%		+-.309%	
2		53.982%		54.067%		54.064%		54.027%
3		53.880%		53.893%		53.886%		53.876%
4		53.954%		53.944%		53.931%		53.937%
5		53.857%		53.865%		53.853%		53.862%
6		54.120%		54.142%		54.110%		54.110%
7		54.239%		54.174%		54.180%		54.182%
8		54.051%		54.047%		54.034%		54.017%
9		54.112%		54.142%		54.091%		54.101%
10		53.926%		53.933%		53.958%		53.981%
---------------------------------------------------------------------------------------------------
		53.9134%	54.0336%	54.0240%	54.0225%	+-.098%

Super Location Clear Multiplier:
Using v1.2c old+ of the program I used the expert game and several samples of 100,000 
puzzles using Expert 30x16x99 starting at 3,3 using 1000 solutions for brute force (BF).
A value of 0 would always pick the best number of clears.  We do not want this.  A value 
of 1.0 would basically require 100% improvement in clears per 1% difference in the product 
of the probability of success times the probability of progress.  (1.0 seems best)
Seed	0.5			0.75		1.0			1.25		1.5			MOE
1		54.119%		54.124%		54.129%		54.127%		54.126%		+-.309%	
2		54.069%		54.065%		54.067%		54.068%		54.063%		
3		53.891%		53.895%		53.893%		53.890%		53.895%		
4		53.952%		53.945%		53.944%		53.943%		53.943%		
5		53.871%		53.872%		53.865%		53.862%		53.864%		
6		54.147%		54.142%		54.142%		54.140%		54.136%		
7		54.171%		54.169%		54.174%		54.171%		54.169%		
8		54.037%		54.041%		54.047%		54.046%		54.043%		
9		54.135%		54.141%		54.142%		54.141%		54.145%		
10		53.928%		53.931%		53.933%		53.933%		53.933%		
---------------------------------------------------------------------------
		54.0320%	54.0325%	54.0336%	54.0321%	54.0317%	+-.098%
		
Super Location Multiplier and Super Location Method:
Using v1.2d old+ of the program I used the expert game and several samples of 100,000 
puzzles using Expert 30x16x99 starting at 3,3 using 1000 solutions for brute force (BF) and
1.0 for clear multiplier and 0.96 threshold.  It appears it is always advantagous to use
the multiplier method.  I am not sure why old does better considering it only estimates
the new perimeter added and it has bugs which cause over counting linked entries and
over counting when two perimeters are merged.  I will default to the jar method just to 
do somewhat equal comparisions with the jar program.
Seed	Jar1.05		v1.2d jar	v1.2d jar+	v1.2d old	v1.2d old+	v1.2d new	v1.2d new+	v1.2e new+	MOE		
1		53.837%		54.112%		54.109%		54.129%		54.125%		54.106%		54.097%		54.097%		+-.309%	
2		53.887%		54.012%		54.050%		54.033%		54.063%		54.000%		54.039%		54.041%
3		53.897%		53.886%		53.900%		53.873%		53.888%		53.881%		53.894%		53.903%
4		53.890%		53.920%		53.923%		53.936%		53.939%		53.921%		53.925%		53.934%
5		53.745%		53.862%		53.870%		53.867%		53.874%		53.868%		53.872%		53.871%
6		53.935%		54.139%		54.149%		54.134%		54.144%		54.136%		54.144%		54.131%
7		53.906%		54.163%		54.165%		54.178%		54.181%		54.173%		54.175%		54.184%
8		54.059%		54.043%		54.052%		54.044%		54.053%		54.037%		54.049%		54.057%
9		54.182%		54.173%		54.170%		54.155%		54.153%		54.172%		54.170%		54.162%
10		53.890%		53.928%		53.925%		53.936%		53.937%		53.921%		53.921%		53.929%
---------------------------------------------------------------------------------------------------
		53.9228%	54.0238%	54.0313%	54.0285		54.0357%	54.0215%	54.0286%	54.0309%	+-.098%
---------------------------------------------------------------------------------------------------


GUESS PRIORITY:
When guessing and there is a tie which direction should we pick.  It may be left to right and top
to bottom (lr).  I see the jar program uses top to bottom left to right (tb).  I thought it might 
make a difference with the expert game since it is not symetrical but it turns out that is not 
the case.  It seems to help the 9x9x10 and 16x16x40 and hurts the 30x16x99 but it is all within 
the margin of error.  So I chose the default as top to bottom to match the jar program.  This can
be changes in the Set Super LOcation Parameters menu by selecting the "Left to right priority".  
I also tried using bottom to top right to left and this looked worse so I did not add an option 
for this.


FINAL RESULTS:
The results using the Bulk run option on the jar analyzer version 1.05 are shown below along 
with results out version v1.2c and v1.2d.  In all cases the KISS Random Number Generator (RNG) 
is used as well as Easy Game Type (0 on start).  I ran multiple 100,000 samples with seeds using 
his method of seeding (he creates the seeds from a second copy of the RNG for each puzzle).

NOTE: My results use brute force (BF) at 1000 solutions or less where the jar1.05 program uses brute 
force at 400 solutions or less in bulk run mode.

Beginner 9x9x10 starting at 2,2 using 1000 solutions for brute force (BF) and use
top to bottom priority for guesses unless otherwise shown:
Seed	Jar1.05		lr jar+		jar+		400 BF		lr old+		old+		new+		MOE
1		97.245%		97.220%		97.229%		97.228%		97.221%		97.230%		97.225%		+-.102%
2		97.143%		97.178%		97.189%		97.190%		97.179%		97.193%		97.191%
3		97.234%		97.142%		97.154%		97.152%		97.147%		97.153%		97.154%
4		97.105%		97.100%		97.109%		97.110%		97.102%		97.115%		97.110%
5		97.181%		97.132%		97.143%		97.145%		97.142%		97.148%		97.144%
6		97.240%		97.174%		97.181%		97.178%		97.166%		97.181%		97.185%	
7		97.254%		97.270%		97.273%		97.272%		97.275%		97.279%		97.271%	
8		97.216%		97.274%		97.275%		97.276%		97.270%		97.273%		97.271%	
9		97.200%		97.180%		97.181%		97.178%		97.187%		97.188%		97.180%	
10		97.113%		97.104%		97.110%		97.109%		97.100%		97.106%		97.109%	
----------------------------------------------------------------------------------------
		97.1931%	97.1774%	97.1844%	97.1838%	97.1789%	97.1866%	97.1840%	+-.032
----------------------------------------------------------------------------------------
11		97.159%		97.138%		97.143%		97.146%		97.136%		97.136%		97.145%	
12		97.117%		97.170%		97.166%		97.162%		97.174%		97.166%		97.166%	
13		97.108%		97.141%		97.145%		97.147%		97.138%		97.146%		97.145%	
14		97.240%		97.256%		97.276%		97.280%		97.259%		97.275%		97.277%	
15		97.220%		97.232%		97.234%		97.234%		97.229%		97.230%		97.233%	
16		97.157%		97.178%		97.200%		97.196%		97.184%		97.200%		97.200%	
17		97.233%		97.208%		97.215%		97.217%		97.216%		97.216%		97.212%	
18		97.148%		97.126%		97.142%		97.140%		97.129%		97.140%		97.141%	
19		97.132%		97.109%		97.099%		97.099%		97.108%		97.100%		97.099%	
20		97.177%		97.176%		97.179%		97.175%		97.183%		97.186%		97.179%	
----------------------------------------------------------------------------------------
		97.1691%	97.1734%	97.1799%	97.1796%	97.1756%	97.1655%	97.1797%	+-.023%
----------------------------------------------------------------------------------------
21		97.170%		97.184%		97.185%		97.182%		97.181%
22		97.207%		97.205%		97.215%		97.213%		97.208%
23		97.315%		97.247%		97.250%		97.248%		97.245%
24		97.233%		97.188%		97.182%		97.187%		97.182%
25		97.111%		97.092%		97.100%		97.098%		97.088%
26		97.229%		97.222%		97.224%		97.223%		97.221%
27		97.214%		97.190%		97.204%		97.203%		97.187%
28		97.186%		97.154%		97.162%		97.165%		97.157%
29		97.249%		97.258%		97.247%		97.253%		97.253%
30		97.229%		97.227%		97.224%		97.222%		97.223%
----------------------------------------------------------------
		97.2143%	97.1967%	97.1993%	97.1994%	97.1945%				+-.019%
----------------------------------------------------------------
31		97.163%		97.125%		97.127%		97.126%		97.130%
32		97.283%		97.266%		97.266%		97.262%		97.261%
33		97.033%		97.063%		97.071%		97.070%		97.067%
34		97.206%		97.251%		97.248%		97.243%		97.255%
35		97.225%		97.300%		97.293%		97.294%		97.295%
36		97.166%		97.173%		97.167%		97.164%		97.181%
37		97.166%		97.192%		97.171%		97.168%		97.198%
38		97.214%		97.226%		97.243%		97.242%		97.232%
39		97.092%		97.075%		97.072%		97.075%		97.073%
40		97.190%		97.156%		97.157%		97.159%		97.155%
----------------------------------------------------------------
		97.1738%	97.1827%	97.1815%	97.1803%	97.1847%				+-.016%
----------------------------------------------------------------
Avg		97.1876%	97.1826%	97.1863%	97.1858%	97.1834%	
				

Intermediate 16x16x40 starting 2,2 using 1000 solutions for brute force (BF) and use
top to bottom priority for guesses unless otherwise shown:
Seed	jar1.05		lr jar+		jar+		400 BF		lr old+		old+		new+		MOE
1		89.177%		89.194%		89.216%		89.216%		89.188%		89.211%		89.209%		+-.194%
2		89.291%		89.365%		89.397%		89.377%		89.368%		89.395%		89.393%
3		89.224%		89.231%		89.218%		89.202%		89.250%		89.223%		89.212%
4		89.110%		89.117%		89.147%		89.138%		89.109%		89.146%		89.145%
5		89.215%		89.212%		89.211%		89.209%		89.218%		89.211%		89.210%
6		89.116%		89.127%		89.130%		89.126%		89.110%		89.117%		89.121%
7		89.234%		89.233%		89.244%		89.242%		89.235%		89.235%		89.235%
8		88.958%		88.985%		89.004%		88.999%		88.982%		88.994%		89.001%
9		89.104%		89.170%		89.180%		89.171%		89.175%		89.179%		89.180%
10		89.329%		89.372%		89.377%		89.379%		89.374%		89.379%		89.370%
----------------------------------------------------------------------------------------
		89.1758%	89.2006%	89.2124%	89.2059%	89.2009%	89.2090%	89.2076%	+-.061%
----------------------------------------------------------------------------------------
11		89.218%		89.182%		89.198%		89.194%		89.184%		89.203%		89.196%
12		89.231%		89.214%		89.225%		89.214%		89.212%		89.215%		89.221%
13		89.240%		89.227%		89.259%		89.254%		89.221%		89.263%		89.258%
14		89.127%		89.121%		89.161%		89.157%		89.111%		89.150%		89.162%		
15		89.100%		89.175%		89.210%		89.201%		89.187%		89.208%		89.207%		
16		89.261%		89.316%		89.334%		89.334%		89.317%		89.343%		89.334%
17		89.206%		89.213%		89.217%		89.222%		89.216%		89.221%		89.218%
18		89.172%		89.143%		89.157%		89.150%		89.156%		89.169%		89.157%
19		89.311%		89.338%		89.372%		89.359%		89.345%		89.379%		89.372%
20		89.293%		89.252%		89.242%		89.227%		89.258%		89.244%		89.232%
----------------------------------------------------------------------------------------
		89.2159%	89.2181%	89.2375%	89.2312%	89.2207%	89.2395%	89.2357%	+-.043%
----------------------------------------------------------------------------------------
21		89.101%		89.187%		89.223%		89.221%
22		89.202%		89.240%		89.227%		89.217%
23		88.998%		89.100%		89.123%		89.115%
24		89.057%		89.163%		89.118%		89.109%
25		89.139%		89.118%		89.136%		89.127%
26		89.117%		89.167%		89.142%		89.134%
27		89.281%		89.296%		89.302%		89.298%
28		89.240%		89.209%		89.221%		89.207%
29		89.191%		89.172%		89.177%		89.167%
30		89.203%		89.302%		89.285%		89.278%
----------------------------------------------------
		89.1529%	89.1954%	89.1954%	89.1873%				+-.035% 
----------------------------------------------------
31		89.138%		89.202%		89.210%		89.204%
32		89.173%		89.144%		89.148%		89.146%
33		89.015%		89.117%		89.110%		89.108%
34		89.202%		89.279%		89.293%		89.296%
35		89.343%		89.279%		89.257%		89.250%
36		89.146%		89.161%		89.183%		89.184%
37		89.171%		89.085%		89.069%		89.070%
38		89.115%		89.101%		89.109%		89.090%
39		89.198%		89.180%		89.160%		89.149%
40		89.033%		89.135%		89.118%		89.110%
----------------------------------------------------------------
		89.1534%	89.1683%	89.1657%	89.1607%				+-.031%
----------------------------------------------------------------
Avg		89.1745%	89.1956%	89.2028%	89.1963%	


Expert 30x16x99 starting at 3,3 using 1000 solutions for brute force and use
top to bottom priority for guesses unless otherwise shown:
Seed	Jar1.05		lr jar+		jar+		lr 400 BF	lr 2000 BF	new+		old+		lr old+		MOE		
1		53.837%		54.108%		54.081%		54.066%		54.145%		54.070%		54.075%		54.125%		+-.309%	
2		53.887%		54.049%		54.029%		53.984%		54.082%		54.023%		54.037%		54.063%
3		53.897%		53.908%		53.886%		53.846%		53.960%		53.878%		53.889%		53.888%
4		53.890%		53.925%		53.887%		53.882%		53.924%		53.891%		53.897%		53.939%
5		53.745%		53.871%		53.867%		53.835%		53.899%		53.862%		53.871%		53.874%
6		53.935%		54.148%		54.150%		54.073%		54.202%		54.126%		54.145%		54.144%
7		53.906%		54.166%		54.142%		54.099%		54.176%		54.158%		54.168%		54.181%
8		54.059%		54.053%		54.078%		54.029%		54.103%		54.078%		54.069%		54.053%
9		54.182%		54.174%		54.152%		54.113%		54.197%		54.136%		54.142%		54.153%
10		53.890%		53.922%		53.890%		53.852%		53.928%		53.891%		53.914%		53.937%
---------------------------------------------------------------------------------------
		53.9228%	54.0324%	54.0162%	53.9779%	54.0616%	54.0285%	54.0209%	54.0357%	+-.098% 
---------------------------------------------------------------------------------------
11		54.055%		54.017%		54.005%		53.979%		54.065%		54.010%		53.998%		54.028%
12		54.099%		54.124%		54.140%		54.075%		54.177%		54.137%		54.144%		54.137%
13		54.187%		54.114%		54.117%		54.050%		54.134%		54.110%		54.109%		54.125%
14		54.086%		54.255%		54.252%		54.205%		54.283%		54.258%		54.246%		54.229%
15		53.592%		53.797%		53.785%		53.740%		53.826%		53.799%		53.796%		53.793%
16		53.860%		53.934%		53.972%		53.867%		53.953%		53.967%		53.978%		53.927%
17		54.012%		54.051%		54.080%		53.983%		54.124%		54.082%		54.077%		54.054%
18		53.808%		54.127%		54.059%		54.039%		54.132%		54.057%		54.084%		54.149%
19		53.875%		53.823%		53.848%		53.802%		53.849%		53.839%		53.852%		53.826%
20		53.739%		53.915%		53.902%		53.854%		53.971%		53.896%		53.911%		53.899%
---------------------------------------------------------------------------------------
		53.9313%	54.0157%	54.0160%	53.9594%	54.0514%	54.0155%	54.0195%	54.0167%	+-.069% 
---------------------------------------------------------------------------------------
21		53.936%		54.076%		54.074%		54.005%											54.093%
22		53.767%		53.805%		53.777%		53.744%											53.790%
23		53.757%		53.944%		53.950%		53.864%											53.928%
24		53.694%		53.568%		53.587%		53.558%											53.589%
25		53.961%		53.904%		53.907%		53.853%											53.909%
26		53.874%		53.986%		54.000%		53.894%											53.991%
27		53.971%		53.980%		53.926%		53.941%											53.989%
28		53.535%		53.666%		53.660%		53.624%											53.646%
29		53.994%		53.956%		54.031%		53.912%											53.948%
30		53.942%		54.025%		53.981%		53.973%											54.038%
---------------------------------------------------------------------------------------
		53.8431%	53.8910%	53.8893%	53.8367%										53.8921%	+-.056%
---------------------------------------------------------------------------------------
31		53.714%		53.686%		53.695%		53.648%											53.693%
32		53.969%		53.946%		53.932%		53.842%											53.917%
33		53.998%		54.189%		54.205%		54.113%											54.169%
34		53.755%		53.855%		53.909%		53.826%											53.857%
35		54.095%		54.233%		54.183%		54.222%											54.197%
36		53.727%		53.875%		53.896%		53.818%											53.869%
37		53.471%		53.667%		53.656%		53.616%											53.647%
38		54.019%		54.109%		54.084%		54.054%											54.096%
39		53.849%		53.974%		53.931%		53.873%											53.962%
40		53.722%		53.793%		53.786%		53.745%											53.799%
---------------------------------------------------------------------------------------
		53.8319%	53.9327%	53.9277%	53.8757%										53.9206%	+-.049%					
---------------------------------------------------------------------------------------
Avg		53.8823%	53.9680%	53.9623%	53.9124%										53.9663%	


When a test run is made the results will appear in the result.txt file in the
folder created with all the results.  It will look something like this:
-----------------------
Starting seed=15
start=3,3
gametree<=1000 solutions
threshold=0.960
end play<=64
-----------------------
Samples: 100000
Won: 53784
Percent: 53.784%
Margin of error: 0.309%
Guess0 used: 41493
Guess0 failed: 20301 (48.93%)
Guess1 used: 10160
Guess1 failed: 1874 (18.44%)
Guess2 used: 27119
Guess2 failed: 2256 (8.32%)
GameTree used: 35060
GameTree failed: 9037 (25.78%)
Super Location used: 114732
Super Location failed: 12748 (11.11%)
Average guesses per win: 2.01
Average guesses per loss: 2.58
Average win prob: 60.94%
Average loss prob: 47.29%
Average Guess0 prob: 51.11%
Average Guess1 prob: 81.35%
Average Guess2 prob: 91.40%
Average GameTree prob: 74.67%
Average Super Location prob: 88.92%
Wins with guesses:
     0     1     2     3     4     5     6     7     8     9    10+
 16055 12719  8628  5623  3745  2527  1610  1075   650   409   743
 16.1% 12.7%  8.6%  5.6%  3.7%  2.5%  1.6%  1.1%  0.7%  0.4%  0.7%

maxCounts=11 (allocated=16)
maxLists=11 (allocated=20)
maxPerimeter=89 (allocated=128)
maxSinglePerimeter=89
maxNeighbors=93 (allocated=200)
max number of perimeter solutions=6635520 (30240 computed)
number of end plays=60488
largest end play=62 (64 allocated) (0 missed)
number of end plays with > 1000 solutions=27046 (44.71%)
number of end plays with <= 400 solutions=30196 (49.92%)
game tree code used=35060
game tree code used early=1618
game tree code helped=8687 (24.78%)
game tree hash table used=91447 (1000000 allocated)
game tree hash table hits=199258
game tree solution entries used=53136 (256100 allocated)
game tree solutions saved=1000 (4000 allocated)
game tree max search depth=18 (24 allocated)
super locations max candidates=31 (48 allocated)

Most of the stats above are self explanatory, however, there are a few that 
need explanations or may be of some interest. 
* The average number of guesses per win is close to two means we generally 
need to guess twice before winning.
* The average number of guesses per loss is often close to one more than
a win which is what I would expect.
* Notice we can win about 16.4% of games without guessing and only another 12+%
guessing only once.  
* The average win/loss probability is the product of all guesses so far up
until we win or lose.
* The average guess probability is average probability of succes for that guess
* Notice that almost half end play situations are under 400 solutions, however, 
a significant portion of end plays cannot use brute force.
* maxCounts is the number of different counts of mines used in the solutions
* game tree is considered helped if the best play is not an entry with the 
highest probability of success.


NOTE: algorithm used in super location solving (FastSolve) is not really any faster
when used in the normal createSolutions code in tank.cpp so I did not implement it.

Please excuse the messy program.  It started out in one direction and when I got some 
new ideas I hacked the ideas into the existing frame.  I did this many times and deleted
most of my origial code.  Now it needs a rewrite to make it clean but I have other projects 
I want to work on.  I did do what I set out to do (what is the probability of winning) and 
more (where to start to get the best chance of a win).


Since I originally wrote the program to do one step at a time without any idea I would
need to run large test runs, the program is not very fast.  I can speed up my program by:
1. Only updating the screen every 100 hands during test runs - reduces time by 15%
2. Removing the file IO - does not help much
3. Do not recompute all the array information about the puzzle 
but just update it from the current move - unknown help 
4. The gametree can just save the moves required - unknown help but not likely much
5. Speed up the create solutions using the web idea that BinaryChop using in the jar version. 


Version 1.2c updates:
1. Issue an error if the hash table gets full.
2. Change test folder name to include the version number (v1.2x).
3. Stop testing if we cannot open the test results file.
4. Give the correct probability when we lose in the results file
not the probability after we backup.
5. Added code to test threshold on super location progress.  
6. Add means to save failed puzzled during test runs.
7. Add means to show gametree and super location results.
8. Redo the qsort for game tree to add missing compare for max value.
9. Dead entries in end play adjacent to open areas are now correct
which affects the game tree code.
10. Fix crash in the game tree code caused by early gametree check
on a list that is not first.
11. Change order in gametree to check those entries with best chance 
of progress first.
12. Add menu items to enable super location threshold code.
13. Increase size of listArray because we have a test case with more 
than 128 members (see testing results using seed=17).  I also increased
the max number of neighbors to match.
14. fix bug in super location code for n < 0 causing hangs

Version 1.2d updates:
1. Change calculation of linked clears to use three methods: jar method, 
old v1.2c method and a new v1.2d method.  For the old method I fixed some 
bugs that would incorrectly count the clears.  For the new method I now 
use the exact count of the number of clears for each value in linked pairs 
because sometimes they share the same clears and sometimes they don't.  
The old method overestimated the clears.  
However the old method gave better results so the program still defaults to the old method.
2. Add menu items to enable jar vs old vs new method of calculating clears.
3. Redo naming of test folders to include jar/old/new ("jar" or "old" or "new") 
and super location threshold code enables ("+");
4. Remove SUPER_THRESHOLD and just use any progress and probability from the 
calculation for the best super location.  This actually helps a little.

Version 1.2e updates:
1. Found bug in old method that would incorrectly count clears for some entries.
This would show up as an incorrect mask.  I did not fix this because old does 
better without the fix.
2. Set default to the jar+ method.
3. Fix bug that would prevent saving puzzles if we were not saving each set when
running tests (bulk mode).
4. Fix bug in jar method when we have more than two lined entries.  The method assumed
there was only one link entry.
5. Add means to build a version to compare two different methods.

Version 1.2f updates:
1. Add means to prioritize guesses from top to bottom vs left to right.  This seems
to help 9x9 and 16x16 but not 30x16!!
2. I added "tb" or "lr" to the folder name for test results to indicate direction of 
super location results.
3. Modified new method for calculating linked clears.

Version 1.2g updates:
1. Made the program 5 times faster when testing by not redrawing the screen for each puzzle
and not waiting 10 ms between puzzles.

