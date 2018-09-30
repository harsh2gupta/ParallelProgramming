#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <cstdlib>
#include <ctime>

#define R 2048 // 2^11, therefore R = 11
using namespace std;
int X[R][R],Y[R][R],Z[R][R];

void iter_IJK(){
    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            for(int k=0;k<R;k++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ(){
    for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_JIK(){
    for(int j=0;j<R;j++){
        for(int i=0;i<R;i++){
            for(int k=0;k<R;k++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_JKI(){
    for(int j=0;j<R;j++){
        for(int k=0;k<R;k++){
            for(int i=0;i<R;i++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ(){
    for(int k=0;k<R;k++){
        for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KJI(){
    for(int k=0;k<R;k++)
    {
        for(int j=0;j<R;j++){
            for(int i=0;i<R;i++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void reset(){
    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            Z[i][j] = 0;
        }
    }
}

int main(){

    time_t start,end;
    srand(time(NULL));

    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            X[i][j] = Y[i][j] = rand()%5000 + 1;
            Z[i][j] = 0;
        }
    }

    start = time(0);
    iter_IJK();
    end = time(0);
    cout <<"IJK, "<<(end-start)<<endl;
    reset();

    start = time(0);
    iter_IKJ();
    end = time(0);
    cout <<"IKJ, "<<(end-start)<<endl;
    reset();

    start = time(0);
    iter_JIK();
    end = time(0);
    cout <<"JIK, "<<(end-start)<<endl;
    reset();

    start = time(0);
    iter_JKI();
    end = time(0);
    cout <<"JKI, "<<(end-start)<<endl;
    reset();

    start = time(0);
    iter_KIJ();
    end = time(0);
    cout <<"KIJ, "<<(end-start)<<endl;
    reset();

    start = time(0);
    iter_KJI();
    end = time(0);
    cout <<"KJI, "<<(end-start)<<endl;

    return 0;
}
