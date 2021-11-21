#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <vector>
#include <ctime>
#include <cmath>
#include <random>
#include <queue>
#include <set>
#include <omp.h>
#include <map>
#include <iterator>
using namespace std;
const int NOT_SCAN = -2;
const int MINE = -1;
int dx[]={0, 0,1,-1,  1,-1,-1, 1};  //前四个是上下左右
int dy[]={1,-1,0, 0,  1,-1, 1,-1};
const int MAXN = 500;

template<class ForwardIt, class T, class Compare=std::less<> >
ForwardIt binary_find(ForwardIt first, ForwardIt last, const T& value, Compare comp={})
{
   	first = std::lower_bound(first, last, value, comp);
    return first != last && !comp(value, *first) ? first : last;
}

int mp[MAXN][MAXN],n,m; //mp 看见的局面
int ans[MAXN][MAXN]; //testcase的答案
int sure1[MAXN][MAXN]; //sure1 推测一定是雷的地方
int sure0[MAXN][MAXN]; //sure0 推测一定不是雷的地方
int is_Constraint[MAXN][MAXN]={0};//判断以这个点为中心的constraint是否加入
int mine_Num;
double p_mp[MAXN][MAXN]; //是雷的概率
double qFunction[MAXN][MAXN]; //

namespace Training_Data_Recorde{

	void save_Training_Data(int i)
	{
		string file_name = "training_data"+to_string(i)+".csv";
		freopen(file_name,"w+",stdout);
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++) 
				printf("%d,",mp[i][j]);
		printf("\n");
		fclose(stdout);
	}

	void save_Training_Data_Ans(int i)
	{
		string file_name = "training_data_ans"+to_string(i)+".csv";
		freopen(file_name,"w",stdout);
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++) 
				printf("%d,",mp[i][j]);
		printf("\n");
		fclose(stdout);
	}

}




namespace CSPRandomMinesweeper{
	const int TOT_RANDOM_NUM = 10000000;
	struct Constraint
	{	
		vector<pair<int,int> > p; //等式左边元素,x,y坐标
		int v; //等式右边
	};
	vector<Constraint> all_Constraint;
	vector<set<pair<int,int> > > v_set;
	vector<vector<int> > v_num;
	vector<int> type_Constraint;
	map<pair<int,int>,int> pro_mp;
	long long get_Pro_tot;
	int have_found=0;
	long long get_Pro_legal;
	void span(const int &x,const int &y) //开出0的情况
	{
		queue<int> q;
		q.push(x);q.push(y);
		while (!q.empty())
		{
			int ox = q.front();q.pop();
			int oy = q.front();q.pop();
			sure0[ox][oy]=1;
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
				{
					mp[nx][ny] = ans[nx][ny];
				}
			}
		}
	}


	void update() //把新的constraint加入到序列中
	{
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]!=NOT_SCAN)
					if (!is_Constraint[i][j])
					{
						is_Constraint[i][j]=1;
						Constraint new_Constraint;
						for (int k=0;k<8;k++)
							new_Constraint.p.push_back(make_pair(i+dx[k],j+dy[k]));
						new_Constraint.v = mp[i][j];
						all_Constraint.push_back(new_Constraint);
						Constraint new_Constraint2;
						new_Constraint2.v = 0;
						new_Constraint2.p.push_back(make_pair(i,j));
						all_Constraint.push_back(new_Constraint2);
					}
	}

	void set_bound() //设置边界上的constraint
	{
		for (int i=0;i<=n+1;i++)
		{
			Constraint test;
			test.v = 0;
			test.p.push_back(make_pair(i,0));
			all_Constraint.push_back(test);
		}
		for (int i=1;i<=m+1;i++)
		{
			Constraint test;
			test.v = 0;
			test.p.push_back(make_pair(0,i));
			all_Constraint.push_back(test);
		}
		for (int i=1;i<=n+1;i++)
		{
			Constraint test;
			test.v = 0;
			test.p.push_back(make_pair(i,m+1));
			all_Constraint.push_back(test);
		}
		for (int i=1;i<=m;i++)
		{
			Constraint test;
			test.v = 0;
			test.p.push_back(make_pair(n+1,i));
			all_Constraint.push_back(test);
		}
	}

	void init()  //读入答案和初始化地图
	{
		// scanf("%d %d %d",&n,&m,&mine_Num);
		for (int i=0;i<=n+1;i++)
			for (int j=0;j<=m+1;j++)
			{
				mp[i][j]=sure1[i][j]=sure0[i][j]=is_Constraint[i][j]=0;
				p_mp[i][j]=0;
				all_Constraint.clear();
				v_set.clear();
				v_num.clear();
				type_Constraint.clear();
				pro_mp.clear();
				have_found=0;
				get_Pro_tot=0;
				get_Pro_legal=0;
			}
		// for (int i=1;i<=n;i++)
		// 	for (int j=1;j<=m;j++)
		// 		scanf("%d",&ans[i][j]);
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				mp[i][j]=NOT_SCAN;
		set_bound();
	}

	void debug_Print() //打印局面信息
	{
		for (int i=1;i<=n;i++)
		{
			for (int j=1;j<=m;j++)
				if (mp[i][j]<0) cout << "\u25A0 ";
					else printf("%d ",mp[i][j]);
			printf("\n");
		}
	}

	bool can_Reduce(int x,int y)  //判断两个constraint是否能相消
	{
		Constraint a = all_Constraint[x];
		Constraint b = all_Constraint[y];
		if (a.p.size()==b.p.size()) return 0;
		bool switch_ab = 0;
		if (a.p.size()>b.p.size()) swap(a,b),switch_ab = 1;
		sort(a.p.begin(), a.p.end());
		sort(b.p.begin(), b.p.end());
		Constraint ans;
		ans.v = b.v - a.v;
		int cnt=0;
		int i=0;
		while (i<b.p.size()&&cnt<a.p.size())
			if (b.p[i]!=a.p[cnt])
				ans.p.push_back(b.p[i]),i++;
			else i++,cnt++;
		while (i<b.p.size())
			ans.p.push_back(b.p[i]),i++;
		if (ans.p.size()!=b.p.size()-a.p.size()) return 0;
		if (!switch_ab) all_Constraint[y] = ans;
			else all_Constraint[x] = ans;
		return 1;
	}

	void if_All_Mine(int i) //碰到 x+y+z=3 变成 x=1 y=1 z=1
	{
		for (int j = 1;j<all_Constraint[i].p.size();j++)
		{
			Constraint test;
			test.v = 1;
			test.p.push_back(all_Constraint[i].p[j]);
			all_Constraint.push_back(test);
		}
		Constraint test;
		test.v = 1;
		test.p.push_back(all_Constraint[i].p[0]);
		all_Constraint[i] = test;
	}

	void if_All_Not_Mine(int i) //碰到 x+y+z=0 变成 x=0 y=0 z=0
	{
		for (int j = 1;j<all_Constraint[i].p.size();j++)
		{
			Constraint test;
			test.v = 0;
			test.p.push_back(all_Constraint[i].p[j]);
			all_Constraint.push_back(test);
		}
		Constraint test;
		test.v = 0;
		test.p.push_back(all_Constraint[i].p[0]);
		all_Constraint[i] = test;
	}

	bool divide() // 分解x+y+z=3 x+y+z=0
	{
		int len = all_Constraint.size();
		bool b=0;
		for (int i=0;i<len;i++)
		{
			if (all_Constraint[i].p.size()==all_Constraint[i].v && all_Constraint[i].v>1)
				if_All_Mine(i),b=1;
			if (all_Constraint[i].v == 0 && all_Constraint[i].p.size()>1)
				if_All_Not_Mine(i),b=1;
		}
		return b;
	}

	void sure()//把确定的点加入sure0 sure1
	{
		int len = all_Constraint.size();
		for (int i=0;i<len;i++)
			if (all_Constraint[i].p.size()==1)
			{
				int x,y;
				x = all_Constraint[i].p[0].first;
				y = all_Constraint[i].p[0].second;
				if (all_Constraint[i].v == 0)
					sure0[x][y]=1;
				else sure1[x][y]=1;
			}
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]!=MINE && mp[i][j]!=NOT_SCAN) sure0[i][j]=1;
	}


	void debug_P_MP_Print()
	{
		printf("\n");
		for (int i=1;i<=n;i++)
		{
			for (int j=1;j<=m;j++)
				printf("%.3lf  ",p_mp[i][j]);
			printf("\n");
		}
	}


	void check_Constraint() //检查是否有constraint能推断的
	{
		int len;
		while (1)
		{
			len = all_Constraint.size();
			bool b=0;
			for (int i=0;i<len;i++)
				for (int j=i+1;j<len;j++)
					if (can_Reduce(i,j)) b=1;
			if (divide()) b=1;
			if (b==0) break;
		}
		sure();
	}

	void print_Constraint() //打印constraint信息
	{
		for (int i=0;i<all_Constraint.size();i++)
		{
			printf("%d\n",i);
			for (int j=0;j<all_Constraint[i].p.size();j++)
				printf("(%d,%d) ",all_Constraint[i].p[j].first,all_Constraint[i].p[j].second);
			printf("%d\n",all_Constraint[i].v);
		}
	}

	bool check_Certain()  //检查是否有确定不是雷的
	{
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]==NOT_SCAN && sure0[i][j]==1)
					return 1;
		return 0;
	}

	int logic_choose() //根据逻辑推断选择
	{
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]==NOT_SCAN && sure0[i][j]==1)
				{
					mp[i][j]=ans[i][j];
					//printf("logic choose (%d, %d)\n",i,j);
					// if (ans[i][j]==MINE) 
					// {
					// 	printf("1\n");
					// 	return
					// }
					if (ans[i][j]==0) span(i,j);
					return 1;
				}
		return 0;
	}

	bool guess_choose() //根据概率模型选择
	{
		int x,y; 
		double d=1;
		int tot=0;
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]==NOT_SCAN) tot++;
		double d2=0;
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]==NOT_SCAN)
				{
					if (p_mp[i][j]<d || (p_mp[i][j]==d && qFunction[i][j]>d2)) 
					{
						d = p_mp[i][j];
						d2 = qFunction[i][j];
						x = i;y=j;
						tot = 1;
					}
					if (p_mp[i][j]==d && qFunction[i][j]==d2)
					{
						tot++;
						bool b = rand()%tot;
						if (b==0) {d = p_mp[i][j];x=i;y=j; d2 = qFunction[i][j];}
					}
				}
		mp[x][y]=ans[x][y];
		// printf("guess choose (%d, %d)\n",x,y);
		if (mp[x][y]==MINE) 
		{
			// debug_P_MP_Print();
			//printf("guess choose (%d, %d)\n",x,y);
			// printf("0\n");
			return 0;
		}
		if (mp[x][y]==0) span(x,y);
		return 1;
	}

	bool choose() //选择下一步选那个点,并根据答案判断选的点是不是雷
	{
		if (logic_choose()) {update();return 1;}
		if (guess_choose()) {update();return 1;} //如果没死，就把新的信息加进去
		return 0;
	}


	void initial_P_MP() //每一轮概率模型初始化
	{
		int num_mine=0;
		int num_not_scan=0;
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++) p_mp[i][j]=1;
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]!=NOT_SCAN && mp[i][j]!=MINE && mp[i][j]!=0)
				{
					int x = 0;
					for (int k=0;k<8;k++) 
						if (j+dy[k]>0 && j+dy[k]<=m && i+dx[k]>0 && i+dx[k]<n && mp[i+dx[k]][j+dy[k]]==NOT_SCAN) x++;
					for (int k=0;k<8;k++)
						p_mp[i+dx[k]][j+dy[k]] = min(p_mp[i+dx[k]][j+dy[k]],(double)mp[i][j]/x);
				}
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
			{
				if (sure0[i][j]) p_mp[i][j]=0;
				if (sure1[i][j]) p_mp[i][j]=1,num_mine++;
				if (mp[i][j]==NOT_SCAN) num_not_scan++;
			}
		double p=(double)(mine_Num-num_mine)/num_not_scan;
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (!sure0[i][j]&&!sure1[i][j]&&mp[i][j]==NOT_SCAN)
					p_mp[i][j]=p;
	}

	bool check_Game_Is_End() //判断是否成功结束,不判断死亡
	{
		bool b=1;
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]==NOT_SCAN && ans[i][j]!=MINE)
					b=0;
		if (b)
		{
			// printf("1\n");
			return 1;
		}
		return 0;
	}
		// ---------------------------------------------------------------------


	bool is_Connect(int x,int y) //判断两个constraint是否有公共元素
	{
		Constraint a = all_Constraint[x];
		Constraint b = all_Constraint[y];
		for (int i=0;i<a.p.size();i++)
			for (int j=0;j<b.p.size();j++)
				if (a.p[i]==b.p[j]) return 1;
		return 0;
	}

	int find_Fa(int x) //并查集
	{
		if (type_Constraint[x]!=x) type_Constraint[x]=find_Fa(type_Constraint[x]);
		return type_Constraint[x];
	}

	void conbine_Constraint(int a,int b) //合并并查集
	{
		int x = find_Fa(a),y=find_Fa(b);
		type_Constraint[x]=y;
	}

	void debug_type_Constraint()
	{
		int len = all_Constraint.size();
		for (int i=0;i<len;i++)
			if (type_Constraint[i]!=i)
				printf("%d %d\n",i,type_Constraint[i]);
		printf("\n");
	}

	void combine_Group() //将有所有公共元素的constraint的并查集合并
	{
		int len = all_Constraint.size();
		type_Constraint.clear();
		for (int i=0;i<len;i++) //并查集初始化
			type_Constraint.push_back(i);
		for (int i=0;i<len;i++)
			for (int j=i+1;j<len;j++)
				if (all_Constraint[i].p.size()!=1 && all_Constraint[j].p.size()!=1)
					if (is_Connect(i,j)) conbine_Constraint(i,j);
	}


	void find_Mine_Group() //对每个group sample他们雷的可能数
	{
		int len = all_Constraint.size();
		for (int i=0;i<len;i++) find_Fa(i);
		v_set.clear();v_num.clear();
		for (int i=0;i<len;i++) 
		{
			set<pair<int,int> > s;
			v_set.push_back(s);
			std::vector<int> v;
			v_num.push_back(v);
		}
		for (int i=0;i<len;i++)
		{
			if (all_Constraint[i].p.size()==1) continue;
			if (type_Constraint[i]!=i) continue;
			for (int j=0;j<len;j++)
				if (type_Constraint[j]==i) 
				{
					for (int k=0;k<all_Constraint[j].p.size();k++)
					{
						v_set[i].insert(all_Constraint[j].p[k]);
						v_num[i].push_back(j);
					}
				}
		}
	}


	void debug_v_set()
	{
		printf("debug_v_set ---------------\n");
		for (int i=0;i<v_set.size();i++)
			if (v_set[i].size()>1) printf("%d %lu\n",i,v_set[i].size());
		printf("\n"); 
	}


	bool check_mp(int id)
	{
		int len = v_num[id].size();		
		for (int i=0;i<len;i++)
		{
			int o = v_num[id][i];
			int x=0;
			for (int k=0;k<all_Constraint[i].p.size();k++)
				x+=pro_mp[all_Constraint[i].p[k]];
			if (x!=all_Constraint[i].v) return 0;
		}
		return 1;
	}

	void get_Pro(const int &id_num,const vector<pair<int,int> > &v)
	{
		have_found=0;
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++) if (sure1[i][j]) have_found++;
		get_Pro_legal = 1;
		int n_2f = 15;
		get_Pro_tot = (long long)1<<(long long)(min((int)v.size(),n_2f)+1);
		vector<long long> cnt1;
		for (int j=0;j<v.size();j++) cnt1.push_back(0);
		if (v.size()>n_2f)
		{
			for (long long i=0;i<get_Pro_tot;i++)
			{
				pro_mp.clear();
				int tot1=0;
				for (int j=0;j<v.size();j++)
				{
					pro_mp[v[j]] = rand()%2;
					tot1+=pro_mp[v[j]];
				}
				if (tot1<=mine_Num-have_found && check_mp(id_num))
				{
					get_Pro_legal++;
					for (int j=0;j<v.size();j++)
						cnt1[j]+=pro_mp[v[j]];
				}
				// printf("\r%lld/%lld",i,get_Pro_tot);
	   			//      	fflush(stdout);
			}
			for (int i=0;i<v.size();i++)
			{
				int x=v[i].first;
				int y=v[i].second;
				p_mp[x][y] = min(p_mp[x][y],(double)(cnt1[i]+1)/(get_Pro_legal+v.size()));
			}
			return ;
		}
		for (long long i=0;i<get_Pro_tot;i++)
		{
			pro_mp.clear();
			long long x = i;
			int tot1=0;
			for (int j=0;j<v.size();j++)
			{
				pro_mp[v[j]] = x%2;
				tot1+=pro_mp[v[j]];
				x/=2;
			}
			if (tot1<=mine_Num-have_found && check_mp(id_num))
			{
				get_Pro_legal++;
				for (int j=0;j<v.size();j++)
					cnt1[j]+=pro_mp[v[j]];
			}
			// printf("\r%lld/%lld",i,get_Pro_tot);
   			//      	fflush(stdout);
		}
		for (int i=0;i<v.size();i++)
		{
			int x=v[i].first;
			int y=v[i].second;
			p_mp[x][y] =min(p_mp[x][y],(double)cnt1[i]/get_Pro_legal);
		}
	}

	void sample()
	{
		int len = v_set.size();
		for (int i=0;i<len;i++)
			if (v_set[i].size()>1 && v_set[i].size()<60)
			{
				vector<pair<int,int> > sample_item;
				for (auto it = v_set[i].cbegin();it!=v_set[i].cend();it++)
					sample_item.push_back(*it);
			 	get_Pro(i,sample_item);
			}
	}

	double cal_Q_Funtion(int ix,int iy)
	{
		int cal_Q_tot=(1<<9);
		int cal_Q_m[9]={0};
		int fz[9] = {0};
		map<pair<int,int>, int > qp;
		for (int i=0;i<cal_Q_tot;i++)
		{
			int cal_x = i,cal_cnt=0;
			cal_Q_m[8]=0;
			for (int j=0;j<8;j++)
			{
				if (cal_x%2==1) cal_Q_m[j]=MINE,cal_cnt++;
					else cal_Q_m[j] = 0 ;
				cal_x/=2;
			}
			for (int j=0;j<8;j++)
			{
				if (ix+dx[j]>n || ix+dx[j]<1 || iy+dy[j]>m ||iy+dy[j]<1) continue;
				if (mp[ix+dx[j]][iy+dy[j]]==NOT_SCAN) 
				{
					qp[make_pair(ix+dx[j],iy+dy[j])] = cal_Q_m[j];
					if (cal_Q_m[j] == MINE) cal_Q_m[8]++;
				}
				else if (mp[ix+dx[j]][iy+dy[j]]==MINE) cal_Q_m[8]++;
			}

			bool cal_Q_b = 1;
			for (int j = ix-2;j<=ix+2;j++)
				for (int k = iy-2;k <= iy+2;k++)
				{
					if (mp[j][k] == MINE) continue;
					if (mp[j][k] != NOT_SCAN )
					{
						int op = 0;
						for (int o=0;o<8;o++)
						{
							auto search = qp.find(make_pair(i+dx[o],k+dy[o]));
							if (mp[i+dx[o]][k+dy[o]] == MINE) op++;
							else 
							{
								if(search != qp.end())
									if (qp[make_pair(i+dx[o],k+dy[o])]==MINE) op++;
							}
						}
						if (op>mp[j][k]) cal_Q_b = 0;
					}
				}
			if (cal_Q_b) fz[cal_cnt]++;
		}
		double ans = 0 ;
		for (int i=0;i<8;i++) 
		{
			double o = (double)fz[i]/(cal_Q_tot-1);
			ans += o*log(o)/log(2);
		}
		return -ans;
	}

	void q_Function() //信息熵函数
	{
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				qFunction[i][j]=0;
		for (int i=1;i<=n;i++)
			for (int j=1;j<=m;j++)
				if (mp[i][j]==NOT_SCAN)
					qFunction[i][j] = cal_Q_Funtion(i,j);
	}

	void debug_qFuntion()
	{
		for (int i=1;i<=n;i++)
		{
			for (int j=1;j<=m;j++)
				printf("%.4lf ",qFunction[i][j]);
			printf("\n");
		}
		printf("-----------------------\n");
	}

	int main1(int & _b)
	{
		init();
		if (ans[1][1]==MINE) return 1;
		mp[1][1]=ans[1][1];
		update();
		while (1)
		{
			//printf("---------------------\n");
			// print_Constraint();
			//debug_Print();
			_b++;
			if (check_Game_Is_End()==1) return 0;
			check_Constraint(); //逻辑推断
			if (check_Certain())
			{
				if (choose()==0) return 1;
				update();
				continue;
			}
			combine_Group();
			find_Mine_Group();
			// debug_type_Constraint();
			// debug_v_set();
			initial_P_MP();
			sample();
			q_Function();
			//debug_qFuntion();
			// debug_P_MP_Print();
			// 
			if (choose()==0) return 1; //选择新点
		}
		return 0;
	}
}


namespace RandomData{
	vector<int> q;
	int main1()
	{
		for (int i=0;i<=n+1;i++)
			for (int j=0;j<=m+1;j++) ans[i][j]=0;
		q.clear();
		int cnt=0;
		while (cnt<mine_Num)
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
		// printf("%d %d %d\n",n,m,mine_Num);
		// for (int i=1;i<=n;i++)
		// {
		// 	for (int j=1;j<=m;j++)
		// 		printf("%d ",ans[i][j]);
		// 	printf("\n");
		// }
		return 0;
	}
}


int main(int argc, char const *argv[])
{
	int cnt1=0,cnt2=0,t_o_t=1000;
	srand((unsigned int)time(NULL));
	for (int i=1;i<=t_o_t;i++)
	{
		// freopen("t_e_s_t.in","w",stdout);
		int _b=0;
		n=8;
		m=8;
		mine_Num=10;
		RandomData::main1();
		// freopen ("/dev/tty", "a", stdout);
		// printf("2333\n");
		// freopen("t_e_s_t.in","r",stdin);
		if (CSPRandomMinesweeper::main1(_b)==0) 
			{
				cnt1++;
			}
		if (_b>=2) 
			{
				cnt2++;
			}
			// else cnt1=0;
		// freopen ("/dev/tty", "a", stdin);
	}
	// #pragma omp parallel for reduction(+:cnt1,cnt2)
	if (cnt2==0) {printf("Try again\n");return 0;}
	printf("%d %d %lf\n",cnt1,cnt2,(double)cnt1/cnt2);
	return 0;
}

