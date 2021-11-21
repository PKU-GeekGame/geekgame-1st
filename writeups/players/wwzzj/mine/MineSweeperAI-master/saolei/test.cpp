#include <iostream>
#include <cstdio>
using namespace std;
const int MAXN = 200;
int nmap[MAXN][MAXN];
double cnt1[MAXN][MAXN];
int n;
int main()
{
	srand((unsigned int)time(NULL));
	cin>>n;
	for (int i=0;i<=n;i++)
		for (int j=0;j<=n;j++)
			cnt1[i][j] = (double)(rand()%n)/(n);
	char c;
	c = getchar();
	for (int i=1;i<=n;i++)
		for (int j=1;j<=n;j++)
		{
			c = getchar();
			if (c=='\n') c=getchar();
			if (c=='-') 
			{
				c = getchar();
				nmap[i][j]='0'-c;
				continue;
			}
			nmap[i][j]=c-'0';
		}
	for (int i=1;i<=n;i++)
		for (int j=1;j<=n;j++)
		{
			if (nmap[i][j]!=-2) 
			{
				double x=0;
				if (nmap[i+1][j]==-2) x++;
				if (nmap[i-1][j]==-2) x++;
				if (nmap[i][j+1]==-2) x++;
				if (nmap[i][j-1]==-2) x++;
				if (nmap[i+1][j+1]==-2) x++;
				if (nmap[i-1][j+1]==-2) x++;
				if (nmap[i+1][j-1]==-2) x++;
				if (nmap[i-1][j-1]==-2) x++;
				x = nmap[i][j]/x;
				if (nmap[i+1][j]==-2) cnt1[i+1][j]+=x;
				if (nmap[i-1][j]==-2) cnt1[i-1][j]+=x;
				if (nmap[i][j+1]==-2) cnt1[i][j+1]+=x;
				if (nmap[i][j-1]==-2) cnt1[i][j-1]+=x;
				if (nmap[i+1][j+1]==-2) cnt1[i+1][j+1]+=x;
				if (nmap[i-1][j+1]==-2) cnt1[i-1][j+1]+=x;
				if (nmap[i+1][j-1]==-2) cnt1[i+1][j-1]+=x;
				if (nmap[i-1][j-1]==-2) cnt1[i-1][j-1]+=x;
			}
		}
	int x,y;
	double min1 = 10000;
	for (int i=1;i<=n;i++)
		for (int j=1;j<=n;j++)
		{
			if (cnt1[i][j]<min1 && cnt1[i][j]!=0 && nmap[i][j]==-2)
			{
				x=i;
				y=j;
				min1 = cnt1[i][j];
			}
		}
	printf("(%d,%d)\n",x-1,y-1);
	return 0;
}