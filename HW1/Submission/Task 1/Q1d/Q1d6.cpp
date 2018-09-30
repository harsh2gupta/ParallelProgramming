#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <chrono>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <cstdlib>
#include <time.h>
#include "cilk/cilk.h"
#define R 64 // 2^6, therefore R = 6
using namespace std;
int X[R][R],Y[R][R],Z[R][R];

void iter_IKJ_0(){
    for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_1(){
    cilk_for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_2(){
    for(int i=0;i<R;i++){
        cilk_for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_3(){
    for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_4(){
    cilk_for(int i=0;i<R;i++){
        cilk_for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_5(){
    for(int i=0;i<R;i++){
        cilk_for(int k=0;k<R;k++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_6(){
    cilk_for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_7(){
    cilk_for(int i=0;i<R;i++){
        cilk_for(int k=0;k<R;k++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_0(){
    for(int k=0;k<R;k++){
        for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_1(){
    cilk_for(int k=0;k<R;k++){
        for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_2(){
    for(int k=0;k<R;k++){
        cilk_for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_3(){
    for(int k=0;k<R;k++){
        for(int i=0;i<R;i++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_4(){
    cilk_for(int k=0;k<R;k++){
        cilk_for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_5(){
    for(int k=0;k<R;k++){
        cilk_for(int i=0;i<R;i++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_6(){
    cilk_for(int k=0;k<R;k++){
        for(int i=0;i<R;i++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_7(){
    cilk_for(int k=0;k<R;k++){
        cilk_for(int i=0;i<R;i++){
            cilk_for(int j=0;j<R;j++){
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

    srand(time(NULL));

    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            X[i][j] = Y[i][j] = rand()%5000 + 1;
            Z[i][j] = 0;
        }
    }

    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    typedef std::chrono::duration<float> fsec;

    auto start = Time::now();
    iter_IKJ_1();
    auto end = Time::now();
    auto d = end - start;
    auto diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_IKJ_I, "<< diff.count() << " ms" <<endl;
    reset();
   
    start = Time::now();
    iter_IKJ_2();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_IKJ_K, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_IKJ_3();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_IKJ_J, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_IKJ_4();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_IKJ_IK, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_IKJ_5();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_IKJ_KJ, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_IKJ_6();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_IKJ_IJ, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_IKJ_7();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_IKJ_IKJ, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_KIJ_1();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_KIJ_K, "<< diff.count() << " ms" <<endl;
    reset();
   
    start = Time::now();
    iter_KIJ_2();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_KIJ_I, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_KIJ_3();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_KIJ_J, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_KIJ_4();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_KIJ_KI, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_KIJ_5();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_KIJ_IJ, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_KIJ_6();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_KIJ_KJ, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    iter_KIJ_7();
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_KIJ_KIJ, "<< diff.count() << " ms" <<endl;
    reset();


    return 0;
}
