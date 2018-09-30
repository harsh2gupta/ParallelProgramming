//
// Created by Shweta Sahu on 3/24/18.
//

#ifndef HW1_CSHARE_H
#define HW1_CSHARE_H


#include <vector>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "tbb/concurrent_queue.h"
#include <mutex>
#include <atomic>
#include <iostream>


using namespace std;



//supports only matrix multiplication
int _sync(vector<int>* stack,mutex* stack_lock,int **Z, int **X, int **Y);

class Task {
public:
    Task( int (*function)(int **, int **, int **, int, int, int ,int,int, int,int),
          int **Z, int **X, int **Y,
          int Zr,int Zc,
          int Xr,int Xc,
          int Yr,int Yc,
          int n ,vector<int>* stack,mutex* stack_lock) : Z(Z), X(X), Y(Y),Zr(Zr),Zc(Zc),Xr(Xr),Xc(Xc),Yr(Yr),Yc(Yc),n(n),function(function),stack(stack),stack_lock(stack_lock) {}

    int n;
    int execute() {
        //printf("Inside Task execute n=%d thread=%x ------------------------------------\n",n,this_thread::get_id());
        function(Z,X,Y,Zr,Zc,Xr,Xc,Yr,Yc,n);
        //printf("Task executed sucessfully n=%d thread=%x --------------------------------\n",n,this_thread::get_id());
        _sync(stack,stack_lock,Z,X,Y);
        return 1;

    }
    vector<int>* stack;
    mutex* stack_lock;

    int** Z;
    int** X;
    int** Y;
    int Zr,Zc,Xr,Xc,Yr,Yc;
    int (*function)(int **, int **, int **, int, int, int ,int,int, int,int) = 0;


};

int maxCores;

bool ready; //job in deque
atomic<bool> terminated;

tbb::concurrent_queue<Task*> *centralizedTaskQueue;

int max_attempts;

void start(){
    while(!terminated){
        Task * task = NULL;
        int count =0;
        while(ready && task == NULL && (count < max_attempts) && !terminated){

            centralizedTaskQueue->try_pop(task);
            count++;
        }
        if(task != NULL){
            task->execute();
        }

    }

}


int initScheduler( int p_maxCores, int (*function)(int **, int **, int **,int,int,int,int,int,int,int),
                   int **Z, int **X, int **Y,
                   int n,vector<int>* stack,mutex* stack_lock){

    printf("inside init");
    ready = false;
    maxCores =p_maxCores;
    terminated = false;

    max_attempts = 2*maxCores*log(maxCores);

    Task* initialTask = new Task(function,Z,X,Y,0,0,0,0,0,0,n,stack,stack_lock);
    centralizedTaskQueue = new tbb::concurrent_queue<Task*>();

    centralizedTaskQueue->push(initialTask);

    int id =0;
    while(id < maxCores-1){
        // threads.push_back(thread(start));
        cilk_spawn start();
        id++;
    }

    ready = true;
    cilk_sync;
    return 1;
}


int spawn(int (*function)(int **, int **, int **,int,int,int,int,int,int,int),int **Z, int **X, int **Y,
          int Zr,int Zc,
          int Xr,int Xc,
          int Yr,int Yc,int n,vector<int>* stack,mutex* stack_lock){
    Task* task = new Task(function,Z,X,Y,Zr,Zc,Xr,Xc,Yr,Yc,n,stack,stack_lock);
    centralizedTaskQueue->push(task);
    return 1;
}


#endif //HW1_CSHARE_H