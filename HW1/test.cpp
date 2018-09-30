//
// Created by Shweta Sahu on 3/14/18.
//

#include <random>
#include <vector>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "tbb/concurrent_queue.h"
#include <mutex>
#include <atomic>
#include <iostream>
using namespace std;
class Task {
public:
    Task( int Zr,int Zc,
          int Xr,int Xc,
          int Yr,int Yc,
          int n ,vector<int>* stack,mutex* stack_lock) : Zr(Zr),Zc(Zc),Xr(Xr),Xc(Xc),Yr(Yr),Yc(Yc),n(n),stack(stack),stack_lock(stack_lock) {}

    int n;
    vector<int>* stack;
    mutex* stack_lock;
    int Zr,Zc,Xr,Xc,Yr,Yc;

};

int** Z;
int** X;
int** Y;
int n;
int maxCores;
bool ready;
atomic<bool> terminated;
int max_attempts;
atomic<long> _count;
tbb::concurrent_queue<Task*> *centralizedTaskQueue;
int mm(int** Z, int** X, int** Y, int Zr,int Zc, int Xr,int Xc, int Yr,int Yc, int n);
int _sync(vector<int>* stack,mutex* stack_lock,int **Z, int **X, int **Y);
void second_half(int **Z, int **X, int **Y, int Zr, int Zc, int Xr, int Xc, int Yr, int Yc, int n,vector<int>* stack) ;




void execute(Task* t){
    mm(Z,X,Y,t->Zr,t->Zc,t->Xr,t->Xc,t->Yr,t->Yc,t->n);
    _sync(t->stack,t->stack_lock,Z,X,Y);
}

void start(){
    Task * task = NULL;
    while(!terminated){
         task = NULL;
        int count =0;
        while(ready && task == NULL && (count < max_attempts) && !terminated){
            centralizedTaskQueue->try_pop(task);
            count++;
        }
        if(task != NULL){
            execute(task);
        }
    }
}

int initScheduler(){
    ready = false;
    terminated = false;
    max_attempts = 2*maxCores*log(maxCores);
    vector<int>* stack = new vector<int>();
    stack->push_back(0);
    stack->push_back(0);
    stack->push_back(0);
    stack->push_back(0);
    stack->push_back(0);
    stack->push_back(0);
    stack->push_back(n);
    stack->push_back(4);
    stack->push_back(-1);
    mutex* stack_lock = new mutex();

    Task* initialTask = new Task(0,0,0,0,0,0,n,stack,stack_lock);
    centralizedTaskQueue = new tbb::concurrent_queue<Task*>();
    centralizedTaskQueue->push(initialTask);

    int id =0;
    while(id < maxCores-1){
        // threads.push_back(thread(start));
        cilk_spawn start();
        id++;
    }
    time_t start = time(0);
    ready = true;
    cilk_sync;
    time_t end = time(0);

    return end-start;
}
int spawn(int Zr,int Zc,
          int Xr,int Xc,
          int Yr,int Yc,int n,vector<int>* stack,mutex* stack_lock){
    Task* task = new Task(Zr,Zc,Xr,Xc,Yr,Yc,n,stack,stack_lock);
    centralizedTaskQueue->push(task);
    return 1;
}

float calculateGFLOPS(int n , int secs){
    float FLOPS = 2*n*n*n/secs;
    return FLOPS/pow(10,9);
}

void fillArray(int** A, int n){
    for(int i =0; i<n ; i++){
        for(int j =0 ; j<n ; j++){
            A[i][j]= 1;
        }
    }
}
void fillZero(int** A, int n){
    for(int i =0; i<n ; i++){
        for(int j =0 ; j<n ; j++){
            A[i][j]= 0;
        }
    }
}
void printArray(int**A, int n){
    for(int i =0; i<n ; i++){
        for(int j =0 ; j<n ; j++){
            cout<<A[i][j]<<"\t";

        }
        cout<<"\n";
    }
}

int _sync(vector<int>* stack,mutex* stack_lock,int **Z, int **X, int **Y){
    int len = stack->size();
    if(len!=0){
        stack_lock->lock();
        //stack[len-2] = stack[len-2]-1;
        stack->at(len-2)--;
        //check sync value and -1
        //if 0 call second_half method with stack values
        if(stack->at(len-2)==0){
            if(stack->at(len-1)==-1) {
                stack_lock->unlock();
                second_half(Z, X, Y, stack->at(0), stack->at(1), stack->at(2), stack->at(3), stack->at(4), stack->at(5),
                            stack->at(6),stack);
            }
        }
        stack_lock->unlock();
    }

    return 1;
}

void first_half(int **Z, int **X, int **Y, int Zr, int Zc, int Xr, int Xc, int Yr, int Yc, int n) {
    int sync1=4;
    mutex* stack_lock = new mutex();
    vector<int>* stack = new vector<int>();
    stack->push_back(Zr);
    stack->push_back(Zc);
    stack->push_back(Xr);
    stack->push_back(Xc);
    stack->push_back(Yr);
    stack->push_back(Yc);
    stack->push_back(n);
    stack->push_back(sync1);
    stack->push_back(-1);
    //Z11,X11,Y11
    spawn(Zr, Zc,
          Xr, Xc,
          Yr, Yc, n, stack, stack_lock);
    //Z12, X11, Y12
    spawn(Zr, Zc+n,
          Xr, Xc,
          Yr, Yc + n , n, stack,stack_lock);
    //Z21, X21, Y11
    spawn(Zr + n , Zc,
          Xr + n , Xc,
          Yr, Yc, n, stack,stack_lock);
    //Z22,X21,Y12
    mm(Z,X,Y,
       Zr+n,Zc+n,
       Xr+n,Xc,
       Yr,Yc+n,n);
    _sync(stack,stack_lock,Z,X,Y);
}

void second_half(int **Z, int **X, int **Y, int Zr, int Zc, int Xr, int Xc, int Yr, int Yc, int n, vector<int>* stack) {
//Z11, X12, Y21

    int sync2=4;
    mutex* stack_lock2 = new mutex();
    //vector<int>* stack = new vector<int>();
    stack->clear();
    stack->push_back(sync2);
    stack->push_back(-2);
    spawn(Zr, Zc,
          Xr, Xc + n,
          Yr + n, Yc, n, stack,stack_lock2);
    //Z12, X12, Y22
    spawn(Zr, Zc + n ,
          Xr, Xc + n ,
          Yr + n , Yc + n , n, stack,stack_lock2);
    //Z21 , X22 , Y21
    spawn(Zr + n , Zc,
          Xr + n , Xc + n ,
          Yr + n , Yc, n, stack,stack_lock2);
    //Z22 , X22 , Y22
    mm(Z,X,Y,
       Zr+n,Zc+n,
       Xr+n,Xc+n,
       Yr+n,Yc+n,n);

    _sync(stack,stack_lock2,Z,X,Y);
}

int mm(int** Z, int** X, int** Y,
       int Zr,int Zc,
       int Xr,int Xc,
       int Yr,int Yc,
       int n){
    if(n==0)
        return 1;
    if( n <= 32) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    Z[Zr + i][Zc + j] = Z[Zr + i][Zc + j] + (X[Xr + i][Xc + k] * Y[Yr + k][Yc + j]);
                    _count--;
                }
            }
        }
//        Z[Zr ][Zc ] = Z[Zr ][Zc ] + (X[Xr ][Xc ] * Y[Yr ][Yc ]);
//        _count--;
        if(_count==0){
            printf("\n terminating*********\n");
            terminated = true;
        }
    }
    else if (n>32){
        n =n/2;
        first_half(Z, X, Y, Zr, Zc, Xr, Xc, Yr, Yc, n);
    }
    return 1;

}
//size schedulerNumber corescount
int multiply(int s,int cores){
    printf("Using %d number of Processors \n",cores);
    printf("For n = 2^%d\n",s);

    n = pow(2,s);
    maxCores = cores;
    Z = new int*[n];
    X = new int*[n];
    Y = new int*[n];
    for(int i = 0; i <n; i++){
        X[i] = new int[n];
        Y[i] = new int[n];
        Z[i] = new int[n];

    }
    _count = n*n*n;
    fillArray(X,n);
    fillArray(Y,n);
    fillZero(Z,n);
    int duration = initScheduler();


    printf("duration %d secs\n",duration);

//    printf("\nOutput-----------\n");
//    printArray(Z,n);
//
//    multiply(W,X,Y,n);
//    printf("\nExpected Output-----------\n");
//    printArray(W,n);
//    bool result = checkResult(Z,W,n);
//    printf("\nResult----------\t");
//    if(result)
//        printf("Pass");
//    else
    //       printf("Fail");
//    //printArray(Z,n);

    return duration;

}
//void main(){
//    int s = 10;
//    int cores = 64;
//    multiply(s,cores);
//
//    //c part
//    /*s = 12;
//     int T1 = multiply(s,1);
//     float efficieny[cores];
//     for(int p =2 ; p <64; p++ ){
//     int Tp = multiply(s,p);
//     efficieny[p-2]=(T1/(p*Tp));
//     }
//
//     printf("T1 = %f\n",T1);
//     int p =2;
//     for(float e: efficieny){
//     printf("Efficiency of program on %d cores is %f",p++,e);
//     }*/
//}





