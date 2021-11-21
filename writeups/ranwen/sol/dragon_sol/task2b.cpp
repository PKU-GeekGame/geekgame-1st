#include<iostream>
#include<cstdio>
#include<cstring>
#include<algorithm>
using namespace std;
template<typename __T>
inline void read(__T &x)
{
    x=0;
    int f=1;char c=getchar();
    while(!isdigit(c)){if(c=='-')   f=-1;c=getchar();}
    while(isdigit(c))   {x=x*10+c-'0';c=getchar();}
    x*=f;
}
typedef unsigned long long ull;
typedef long long ll;
//typedef pair<int,int>pi;
//pi mp(int a,int b)
//{
//	return make_pair(a,b);
//}
const int mod=1000000007;
long long qpow(long long a,long long b=mod-2)
{
	long long ans=1;
	while(b)
	{
		if(b&1)	ans=ans*a%mod;
		b>>=1;
		a=a*a%mod;
	}
	return ans;
}
struct prandom
{
	ull multip=0x5DEECE66D;
	ull addp=0xB;
	ull last=0;
	ull mask=(1ll<<48)-1;
	void init(ull sed)
	{
		last=(sed^multip)&mask;
	}
	int next()
	{
		last=(last*multip+addp)&mask;
		return last>>(48-31);
	}
	int nextint(int x)
	{
		return next()%x;
	}
	prandom(ull sed=0)
	{
		init(sed);
	}
};
int n,m;
long long mjw[50];
ll checkix[6666],checkiy[6666],checkox[6666],checkoy[6666];
int mjp[50];
int zmj=0;
ull basev=0;
int main()
{
    read(n);
    for(int i=0;i<n;i++)
    {
        read(mjw[i]);
        if(mjw[i]==-1)
        {
            mjp[zmj]=i;
            zmj++;
        }
        else
        {
            basev|=mjw[i]<<i;
        }
    }
    read(m);
    for(int i=0;i<m;i++)
    {
        read(checkix[i]);
        read(checkiy[i]);
        read(checkox[i]);
        read(checkoy[i]);
    }

    int xd=1<<zmj;
    for(int st=0;st<xd;st++)
    {
        ull myb=basev;
        for(int i=0;i<zmj;i++)
        {
            myb|=((st>>i)&1ull)<<mjp[i];
        }
        // cout<<myb<<endl;
        bool flag=true;
        for(int j=0;j<m;j++)
        {
            prandom rng(myb+0x2e01*checkix[j]+0x76b5*checkiy[j]);
            int x=rng.nextint(29);
            int y=rng.nextint(29);
            if(x!=checkox[j]||y!=checkoy[j])
            {
                flag=false;
                break;
            }
        }
        if(flag)
        {
            cout<<myb<<endl;
        }
    }
    return 0;
}
