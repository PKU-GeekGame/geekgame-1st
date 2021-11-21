#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <vector>
#include <ctime>
#include <random>
using namespace std;
const int MAXN = 500;
vector<int> q;
int ans[MAXN][MAXN]={0};
int dx[]={0, 0,1,-1,1,-1,-1, 1};
int dy[]={1,-1,1,-1,0, 0, 1,-1};
int main(int argc, char const *argv[])
{
	int n=5,m=5;
	int num_mine = 5;
	if (argc!=1)
	{
		if (argc!=4) {printf("Wrong arg, n  m  mine_num\n");exit(0);}
		n = atoi(argv[1]);
		m = atoi(argv[2]);
		num_mine = atoi(argv[3]);
	}
	int cnt=0;
	srand((unsigned int)time(NULL));
	while (cnt<num_mine)
	{
		int x=rand()%n+1;
		int y=rand()%m+1;
		bool b = 0;
		for (int i=0;i<q.size();i+=2) 
			if (q[i]==x&&q[i+1]==y) {b=1;break;}
		if (b==0) {q.push_back(x);q.push_back(y);cnt++;}
	}
	for (int i=0;i<q.size();i+=2) 
	{
		int x = q[i],y=q[i+1];
		ans[x][y]=-1;
		for (int j=0;j<8;j++)
			if (ans[x+dx[j]][y+dy[j]]!=-1)
				ans[x+dx[j]][y+dy[j]]++;
	}
	printf("%d %d %d\n",n,m,num_mine);
	for (int i=1;i<=n;i++)
	{
		for (int j=1;j<=m;j++)
			printf("%d ",ans[i][j]);
		printf("\n");
	}
	return 0;
}