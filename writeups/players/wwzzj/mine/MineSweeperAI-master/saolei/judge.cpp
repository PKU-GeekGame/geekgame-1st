#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>
using namespace std;
const int MAXN = 305;
const int MINE = -1;
const int NOT_SCAN = -2;
int ans[MAXN][MAXN],mp[MAXN][MAXN],n,m;
char input_for_program[] = "input_for_program.in";

void write_down_input()
{
	if(freopen(input_for_program, "w", stdout)) {
		printf("%d %d",n,m);
		for (int i=1;i<=n;i++)
		{
			for (int j=1;j<=m;j++) printf("%d ",mp[i][j]);
			printf("\n");
		}
        fclose(stdout);
    }
}
void run_the_program(char minesweeper_path[])
{
	char c;
	c=getchar();
	pair<int,int> point;
	char output_ans[] = "point.out";
	int sys = 0;
	while (c!=EOF)
	{
		while (c!='\n'){c=getchar();if (c==EOF) exit(0);} c=1;
		char x[400];
		strcpy(x,"./"); strcat(x,minesweeper_path); strcat(x," > "); strcat(x,output_ans);
		sys = system(x);
		if (sys != 0) {printf("SysError\n"); exit(0);}

		if(freopen(output_ans, "r", stdin)) 
		{
			int x,y;
			scanf("%d%d",&x,&y);
			fclose(stdin);
			if (x>n||x<0||y>m||y<0) {printf("Wrong output\n");exit(0);}
			if (ans[x][y]==MINE) 
			{
				printf("DIED!\n");
				exit(0);
			}
			mp[x][y]=ans[x][y];
			//if (ans[x][y]==0) larger(x,y);
			write_down_input();
			printf("Next!\n");
		}
		else{printf("No output\n"); exit(0);}

	}
}

void init(char test_path[],char ans_path[])
{
	if(freopen(test_path, "r", stdin)) 
	{
		scanf("%d %d",&n,&m);
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++) scanf("%d",&mp[i][j]);
        fclose(stdin);
    }
    else {printf("No test case\n");exit(0);}

    if(freopen(ans_path, "r", stdin)) 
    {
    	int x,y;
		scanf("%d %d",&x,&y);
		if (x!=n||y!=m) {fclose(stdout);printf("ans and test is diff\n");exit(0);}
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++) scanf("%d",&ans[i][j]);
        fclose(stdin);
    }
    else{ printf("No ans case\n");exit(0); }

    write_down_input();
}

int main(int argc, char const *argv[])
{
	if (argc != 4) 
	{
		printf("Wrong argv\n");
		printf("Your_minesweeper_path\nYour_testcase_path\nYour_testans_path\n");
		return 0;
	}
	char minesweeper_path[100]="" ;
	strcpy(minesweeper_path,argv[1]);

	char test_path[100]="" ;
	strcpy(test_path,argv[2]);

	char ans_path[100]="";
	strcpy(ans_path,argv[3]);

	init(test_path,ans_path);
	run_the_program(minesweeper_path);
	return 0;
}