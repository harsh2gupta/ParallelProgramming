//
// Created by Shweta Sahu on 3/14/18.
//

#include <random>
#include <vector>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "ConcurrentDeque.h"
#include <mutex>
#include <atomic>
#include <iostream>

#include <unordered_map>
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
    vector<int>* child_stack;
    mutex* child_stack_lock;
    int Zr,Zc,Xr,Xc,Yr,Yc;

};

int** Z;
int** X;
int** Y;
int N;
int maxCores;
bool ready;
atomic<bool> terminated;
int max_attempts;
atomic<long> _count;
bool init;
mutex * init_lock = new mutex();
vector<int>* initial_stack = new vector<int>();
static Task* initialTask;
std::unordered_map<int,ConcurrentDeque<Task*>*> processQueue;
vector<int> processIds;
int mm(int** Z, int** X, int** Y, int Zr,int Zc, int Xr,int Xc, int Yr,int Yc, int n,Task* t);
int _sync(vector<int>* stack,mutex* stack_lock,int **Z, int **X, int **Y,Task* t);
void second_half(int **Z, int **X, int **Y, int Zr, int Zc, int Xr, int Xc, int Yr, int Yc, int n,vector<int>* stack,mutex* stack_lock2,Task* t) ;

std::random_device rd; // random device engine
// initialize Mersennes' twister using rd to generate the seed
std::mt19937 rng(rd());
int  getRandom(){
    std::uniform_int_distribution<int> uid(0,maxCores); // random dice
    return uid(rng)-1; // use rng as a generator
}

bool checkAllEmpty(){
    std::unordered_map<int,ConcurrentDeque<Task*>*>::iterator it = processQueue.begin();
// Iterate over the map using iterator
    while(it != processQueue.end())
    {
        ConcurrentDeque<Task*>* t = (ConcurrentDeque<Task*>*)it->second;
        if(t !=NULL && t->size()>0){
            return false;
        }
        it++;
    }
    return true;
}

void execute(Task* t){
    mm(Z,X,Y,t->Zr,t->Zc,t->Xr,t->Xc,t->Yr,t->Yc,t->n,t);
    _sync(t->stack,t->stack_lock,Z,X,Y,t);
}

void start(){
    int processId = __cilkrts_get_worker_number();
    processIds.push_back(processId);
    //printf("start: %d\n",processId);
    processQueue[processId] = new ConcurrentDeque<Task*>();
    Task * task = NULL;

    while(!terminated || !checkAllEmpty()){
        task = NULL;
        if(ready){
            task = processQueue[processId]->pop_front();
            int count =0;
            while(task == NULL && (count < max_attempts)){
                int pId = processIds[getRandom()];
                task = processQueue[pId]->pop_back();
                count++;
            }
        }
        if(task != NULL){
            execute(task);
        }

    }
}

int initScheduler(){
    ready = false;
    terminated = false;
    init = true;
    max_attempts = 2*maxCores*log(maxCores);
    vector<int>* stack = new vector<int>();
    stack->push_back(0);
    stack->push_back(0);
    stack->push_back(0);
    stack->push_back(0);
    stack->push_back(0);
    stack->push_back(0);
    stack->push_back(N);
    stack->push_back(4);
    stack->push_back(-1);
    mutex* stack_lock = new mutex();

    initialTask = new Task(0,0,0,0,0,0,N,stack,stack_lock);
    int id =0;
    while(id < maxCores-1){
        cilk_spawn start();
        id++;
    }
    processQueue[0]->push_back(initialTask);
    maxCores = processIds.size();

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
    int processId = __cilkrts_get_worker_number();
    processQueue[processId]->push_back(task);
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

int _sync(vector<int>* stack,mutex* stack_lock,int **Z, int **X, int **Y,Task* t){
    if(stack->size() ==9) {
        stack_lock->lock();
        //stack[len-2] = stack[len-2]-1;
        stack->at(7)--;
        //check sync value and -1
        //if 0 call second_half method with stack values
        if (stack->at(7) == 0) {
            if (stack->at(8) == -1) {
                stack_lock->unlock();
                second_half(Z, X, Y, stack->at(0), stack->at(1), stack->at(2), stack->at(3), stack->at(4), stack->at(5),
                            stack->at(6), stack, stack_lock, t);
            }
            else if (stack->at(8) == -2 &&  t->n == N/2) {
                //printf("terminated by n =%d size of queue %d\n", stack->at(6),centralizedTaskQueue->size());
                terminated = true;
            }
        }
        stack_lock->unlock();
    }

    return 1;
}

void first_half(int **Z, int **X, int **Y, int Zr, int Zc, int Xr, int Xc, int Yr, int Yc, int n,vector<int>* stack,mutex* stack_lock, Task* t) {
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
       Yr,Yc+n,n,t);
    _sync(stack,stack_lock,Z,X,Y,t);
}

void second_half(int **Z, int **X, int **Y, int Zr, int Zc, int Xr, int Xc, int Yr, int Yc, int n, vector<int>* stack,mutex* stack_lock2,Task* t ) {
//Z11, X12, Y21


    stack->clear();
    stack->push_back(Zr);
    stack->push_back(Zc);
    stack->push_back(Xr);
    stack->push_back(Xc);
    stack->push_back(Yr);
    stack->push_back(Yc);
    stack->push_back(n);
    stack->push_back(4);
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
       Yr+n,Yc+n,n,t);

    _sync(stack,stack_lock2,Z,X,Y,t);
}

int mm(int** Z, int** X, int** Y,
       int Zr,int Zc,
       int Xr,int Xc,
       int Yr,int Yc,
       int n,Task* t){
    if( n == 32) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    Z[Zr + i][Zc + j] +=  X[Xr + i][Xc + k] * Y[Yr + k][Yc + j];

                }
            }
        }
        //Z[Zr][Zc ] +=  X[Xr][Xc] * Y[Yr][Yc];
        //_count-=32768;
//        if(_count==0){
//            terminated = true;
//        }
    }
    else{
        n =n/2;
        int sync1=4;
        t->child_stack_lock = new mutex();
        t->child_stack = new vector<int>();
        if(init){
            init_lock->lock();
            if(init){
                init = false;
                initial_stack = t->child_stack;
            }
            init_lock->unlock();

        }
        t->child_stack->push_back(Zr);
        t->child_stack->push_back(Zc);
        t->child_stack->push_back(Xr);
        t->child_stack->push_back(Xc);
        t->child_stack->push_back(Yr);
        t->child_stack->push_back(Yc);
        t->child_stack->push_back(n);
        t->child_stack->push_back(sync1);
        t->child_stack->push_back(-1);
        first_half(Z, X, Y, Zr, Zc, Xr, Xc, Yr, Yc, n,t->child_stack,t->child_stack_lock,t);
    }
    return 1;

}
//size schedulerNumber corescount
int multiply(int s,int cores){
    printf("Using %d number of Processors \n",cores);
    printf("For n = 2^%d\n",s);

    N = pow(2,s);
    maxCores = cores;
    Z = new int*[N];
    X = new int*[N];
    Y = new int*[N];
    for(int i = 0; i <N; i++){
        X[i] = new int[N];
        Y[i] = new int[N];
        Z[i] = new int[N];

    }
    //_count = n*n*n;
    fillArray(X,N);
    fillArray(Y,N);
    fillZero(Z,N);
    int duration = initScheduler();
    printf("duration %d secs\n",duration);
    printf("value %d\n",Z[N-2][N-4]);
    return duration;

}
void main(){
    int s = 10;
    int cores = 64;
    multiply(s,cores);

    //c part
    /*   s = 12;
    int T1 = multiply(s,1);
    float efficieny[cores];
    for(int p =2 ; p <64; p++ ){
    int Tp = multiply(s,p);
    efficieny[p-2]=(T1/(p*Tp));
    }

    printf("T1 = %f\n",T1);
    int p =2;
    for(float e: efficieny){
    printf("Efficiency of program on %d cores is %f",p++,e);
    }*/
}





