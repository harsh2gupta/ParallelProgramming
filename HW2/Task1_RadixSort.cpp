//
// Created by Shweta Sahu on 4/15/18.
//

#include "utils.h"
using namespace std;


void par_counting_rank(vector<int> &S, int n,int d,vector<int> &r,int p){
    int max = pow(2,d)-1;
    vector<vector<int>> f(max+1,vector<int>(p+1));
    vector<vector<int>> r1(max+1,vector<int>(p+1));
    vector<int> j_start(p+1);
    vector<int> j_end(p+1);
    vector<int> ofs(p+1);

    cilk_for(int i =1; i <=p; i++){
        for(int j=0; j <= max; j++){
            f[j][i] =0;
        }

        j_start[i]=(i-1)*floor(n/p)+1;
        j_end[i] = (i<p)?i*floor(n/p):n;

        for(int j = j_start[i]; j<=j_end[i];j++){
            f[S[j]][i]++;
        }
    }

    for(int j =0 ; j<=max; j++){
        f[j] = par_prefix_sum(f[j]);
    }

    cilk_for(int i =1; i <=p; i++){
        ofs[i] =1;
        for(int j=0; j <= max; j++){
            r1[j][i] = (i==1)?ofs[i]:(ofs[i]+f[j][i-1]);
            ofs[i] = ofs[i] +f[j][p];
        }
        for(int j = j_start[i]; j<=j_end[i];j++){
            r[j]= r1[S[j]][i];
            r1[S[j]][i]++;
        }
    }

}
int extract_bit_segment(int x, int a , int b){
    return (x>>a)&((1<<(b-a+1))-1);
}
void par_radix_sort_with_counting_rank(vector<int> &A, int n, int b,int p) {
    vector<int> r(n+1);
    vector<int> B(n+1);
    vector<int> S(n+1);

    int d = ceil(log(n/(p*log(n))));
    d = d<1?1:d;
    for(int k =0; k<=(b-1); k=k+d){
        int q = (k+d)<=b ? d:(b-k);
        cilk_for(int i =1; i <=n; i++){
            S[i] = extract_bit_segment(A[i],k,k+q-1);
        }
        par_counting_rank(S,n,q,r,p);
        cilk_for(int i=1;i<=n;i++){
            B[r[i]]= A[i];
        }
        cilk_for(int i=1;i<=n;i++){
            A[i]=B[i];
        }
    }
}

int main() {
    int b = 32;
    int max = 2147483647;
    int p =64;
    //int r = 8;
    for(int r =8; r <=22; r++) {
        int N = pow(2, r);
        vector<int> A(N);
        cilk_for (int i = 1; i <= N; i++) {
            A[i] = getRandom(0, max);
        }

        auto start = Time::now();
        par_radix_sort_with_counting_rank(A, N, b, p);
        auto end = Time::now();
        auto diff = std::chrono::duration_cast<ms>(end - start);
        //      cout << "p =" << p << "\ttime:" << diff.count() << "\n";
        cout << "r =" << r << "\ttime:" << diff.count() << "\n";
//        if(diff.count()>2*60*1000)
//            break;
//        }

//    for(int i=1;i<=N;i++){
//        cout<<A[i]<<"\t";
//    }
    }
    return 0;
}
