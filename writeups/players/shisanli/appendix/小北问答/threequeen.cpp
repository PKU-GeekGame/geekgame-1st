#include<fstream>
#include<iostream>
#include<ctime>

std::ofstream fout("queenoo.txt");

/**//* 记录当前的放置方案 */
int *x; 
/**//* 皇后的个数N 和 方案数目 */
int n,m1,m2,sum=0;
/**//* 检查参数所指示的这一行皇后放置方案是否满足要求 */
int  Place(int);
/**//* 递归方法求取皇后放置方案*/
void Queen1(void);
/**//* 用户递归求取皇后放置方案的递归方法 */
void TraceBack(int);
/**//* 打印当前成功的放置方案 */
//void PrintMethod(void);

int queennum=0;

int main(int argc, char** argv)
{
    using namespace std;
    long start,stop;
    
    n = atoi(argv[1]);
    m1 = atoi(argv[2]);
    m2 = atoi(argv[3]);
    
    //cout<<"input n  Number of Queen: ";
    //cin>>n;
    
    //cout<<"input m1: ";
    //cin>>m1;
    
    //cout<<"input m2: ";
    //cin>>m2;

    x=(int *)malloc(sizeof(int)*m1);
    x[0] = -2;
    x[1] = -2;
    x[2] = -2;
    time(&start);/**//*记录开始时间*/
    Queen1();
    time(&stop);/**//*记录结束时间*/
    cout<<"Number of methods: "<<sum<<"\n";
    cout<<"Total time: "<<(int(stop-start))<<"\n";
    fout<<"Number of methods: "<<sum<<"\n";
    fout<<"Total time: "<<((stop-start))<<"\n";

}

int Place(int r)
{
    int i;
    for(i=0;i<r;i++){
        if(x[i]==-1) continue;
        if(x[r]==x[i] || abs(r-i)==abs(x[r]-x[i]))
            return 0;
    }
    return 1;
}

void TraceBack(int r)
{
    int i;
    if(r>=m1){
        //PrintMethod();
        //if(queennum==n) sum++;
        /**//* PrintMethod(); */
    }else{
        queennum++;
        for(i=0;i<m2;i++){
            x[r]=i;
            if(Place(r)) {
              if(queennum<n)TraceBack(r+1);
              else sum++; //std::cout<<x[0]<<'\t'<<x[1]<<'\t'<<x[2]<<std::endl;}
            }
        }
        queennum--;
        x[r]=-1;
        TraceBack(r+1);
    }
}

/*
void PrintMethod(void)
{
    int i,j;
    std::cout<<"The "<<sum<<" th method\n";
    fout<<"The "<<sum<<" th method\n";
    for(i=0;i<m1;i++){
        for(j=0;j<m2;j++){
            if(j==x[i]) std::cout<<"1",fout<<"1";
            else std::cout<<"0",fout<<"0";
        }
        std::cout<<"\n";
        fout<<"\n";
    }
}
*/

void Queen1(void)
{
    TraceBack(0);
}