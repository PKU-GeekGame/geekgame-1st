#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <vector>
#include <ctime>
#include <random>
#include <queue>
using namespace std;
const int NOT_SCAN = -2;
const int MINE = -1;
int dx[]={0, 0,1,-1,1,-1,-1, 1};
int dy[]={1,-1,0, 0,1,-1, 1,-1};
const int MAXN = 500;
namespace RejRandomMinesweeper{
	const int TOT_RANDOM_NUM = 10000000;
	int mp[MAXN][MAXN],n,m;
	int ans[MAXN][MAXN]; //testcase的答案
	int sample[MAXN][MAXN];// for random sample
	int p[MAXN][MAXN]={0}; //合法情况 是雷的次数

	void span(const int &x,const int &y)
	{
		queue<int> q;
		q.push(x);q.push(y);
		while (!q.empty())
		{
			int ox = q.front();q.pop();
			int oy = q.front();q.pop();
			int nx,ny;
			for (int i=0;i<4;i++)
			{
				nx = ox+dx[i];ny=oy+dy[i];
				if (mp[nx][ny]==NOT_SCAN && ans[nx][ny]==0)
				{
					mp[nx][ny]=0;
					q.push(nx);q.push(ny);
				}
				if (mp[nx][ny]==NOT_SCAN && ans[nx][ny]>0 && ans[nx][ny]<9)
					mp[nx][ny] = ans[nx][ny];
			}
		}
	}


	pair<int,int> get_maxp(const int &tot)
	{
		double o=0.2,u;
		pair<int,int> an;
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]==NOT_SCAN)
				{
					u = (double)(tot-p[i][j])/tot;
					if (u>o) {o=u;an.first=i;an.second=j;}
					// if (u==o)
					// 	if (!(rand()%10)) {o=u;an.first=i;an.second=j;}
				}
		return an;
	}

	void add_One()
	{
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]==NOT_SCAN)
					p[i][j]+=sample[i][j];
	}

	int calculate(const int &x,const int &y)
	{
		int cnt=0;
		for (int i=0;i<8;i++)
			if (mp[x+dx[i]][y+dy[i]]==NOT_SCAN)
				cnt+=sample[x+dx[i]][y+dy[i]];
		return cnt;
	}

	bool check()
	{
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]<=8&&mp[i][j]>=0)
					if (calculate(i,j)!=mp[i][j]) return 0;
		return 1;
	}

	void init()
	{
		scanf("%d %d",&n,&m);
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				scanf("%d",&ans[i][j]);
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				mp[i][j]=NOT_SCAN;
	}

	void get_Random_Sample(int& cnt)
	{
		int t=TOT_RANDOM_NUM;
		vector<int> q;
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]>0&&mp[i][j]<9) 
					for (int k=0;k<8;k++)
						if (i+dx[k]>=1&&i+dx[k]<=n)
							if (j+dy[k]>=1&&j+dy[k]<=m)
								if (mp[i+dx[k]][j+dy[k]]==NOT_SCAN)
									{q.push_back(i+dx[k]);q.push_back(j+dy[k]);}
		printf("q.size: %lu\n", q.size());
		srand((unsigned int)time(NULL));
		while (t>0)
		{
			int x,y;
			for (int i=0;i<q.size();i+=2)
			{
				x = q[i];
				y = q[i+1];
				sample[x][y]=rand()%2;//1
			}
			if (check()){ cnt++;add_One();}
			t--;
		}
	}

	void debug_Print()
	{
		for (int i=1;i<=n;i++)
		{
			printf("\n");
			for (int j=1;j<=m;j++)
				printf("%d ",mp[i][j]);
			printf("\n");
		}
	}

	int main()
	{
		init();
		while (1)
		{
			int cnt=0;
			get_Random_Sample(cnt);
			cout<<cnt<<endl;
			pair<int,int> choose = get_maxp(cnt);
			int x,y;
			x = choose.first;y=choose.second;
			printf("choose (%d,%d)\n",x,y);
			if (ans[x][y]!=MINE) {mp[x][y]=ans[x][y];if (mp[x][y]==0) span(x,y);}
				else {printf("DIED\n");exit(0);}
			debug_Print();
		}
		return 0;
	}
}


int main(int argc, char const *argv[])
{
	RejRandomMinesweeper::main();
	return 0;
}

