//
// Created by Harsh Gupta on 4/14/18.
//
#include "utils.h"

int M;
int N;
void par_randomized_quicksort(vector<int> &A,int q,int r){
    int n = r-q+1;
    if(n<=M){
        int i, key, j;
        for (i = q; i <= r; i++)
        {
            key = A[i];
            j = i-1;
            while (j >= 0 && A[j] > key)
            {
                A[j+1] = A[j];
                j = j-1;
            }
            A[j+1] = key;
        }
    }else{
        int temp = getRandom(q,r);
        int x = A[temp];
        int k = par_partition(A,q,r,x);
        cilk_spawn par_randomized_quicksort(A,q,k-1);
        //par_randomized_quicksort(A,q,k-1);
        par_randomized_quicksort(A,k+1,r);
        cilk_sync;
    }

}

int main(){

    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    M=32;
    float avg =0.0;
    for(int i=10;i<22;i++){
        N=pow(2,i);
        for(int k=0;k<3;k++){
            vector<int> A(N);
            for(int j=0;j<N;j++){
                A[j]=getRandom(1,N);
            }
            auto start = Time::now();
            par_randomized_quicksort(A,0,N-1);
            auto end = Time::now();
            auto d = end - start;
            auto diff = std::chrono::duration_cast<ms>(d);
            avg +=diff.count();

        }
        cout <<"N, "<<i<<" "<< avg/3 << " ms" <<endl;
        avg = 0.0;
    }

    return 0;
}
