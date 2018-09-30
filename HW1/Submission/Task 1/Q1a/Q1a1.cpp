#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <cstdlib>
#include <ctime>
#include <chrono>

#define R 2 // 2^1, therefore R = 1
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

    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    typedef std::chrono::duration<float> fsec;

    srand(time(NULL));

    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            X[i][j] = Y[i][j] = rand()%5000 + 1;
            Z[i][j] = 0;
        }
    }

    auto start = Time::now();
    iter_IJK();
    auto end = Time::now();
    auto d = end - start;
    auto diff = std::chrono::duration_cast<ms>(d);
    cout <<"IJK, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_IKJ();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"IKJ, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_JIK();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"JIK, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_JKI();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"JKI, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_KIJ();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"KIJ, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_KJI();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"KJI, "<< diff.count() << " ms" <<endl;

    return 0;
}
