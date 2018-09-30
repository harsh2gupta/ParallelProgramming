//
// Created by Shweta Sahu on 3/24/18.
//

#ifndef HW1_STEALMOD_H
#define HW1_STEALMOD_H


#include <vector>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include "tbb/concurrent_queue.h"
#include <atomic>
#include <iostream>
#include <random>
#include "tbb/atomic.h"
#include <unordered_map>
#include <mutex>


using namespace std;
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

static std::unordered_map<int,tbb::concurrent_queue<Task*>*> processQueue;
static std::unordered_map<int,long> work;
int max_attempts;

static std::random_device rd; // random device engine
// initialize Mersennes' twister using rd to generate the seed
static std::mt19937 rng(rd());
int  getRandom(){
    static std::uniform_int_distribution<int> uid(1,maxCores); // random dice
    return uid(rng)-1; // use rng as a generator
}



void start(){
    while(!terminated){
        int processId = __cilkrts_get_worker_number();
        Task * task = NULL;
        if(ready){
            processQueue[processId]->try_pop(task);
            int count =0;
            while(task == NULL && (count < max_attempts) && !terminated){
                int p1Id = getRandom();
                int p2Id = getRandom();
                int pId = (work[p1Id] > work[p2Id])?p1Id:p2Id;
                processQueue[pId]->try_pop(task);
                if(task != NULL){
                    int n = task->n;
                    work[pId] -= n*n*n;
                }
                count++;
            }
        }
        if(task != NULL){
            task->execute();
        }

    }

}

int spawn(int (*function)(int **, int **, int **,int,int,int,int,int,int,int),int **Z, int **X, int **Y,
          int Zr,int Zc,
          int Xr,int Xc,
          int Yr,int Yc,int n,vector<int>* stack,mutex* stack_lock){
    Task* task = new Task(function,Z,X,Y,Zr,Zc,Xr,Xc,Yr,Yc,n,stack,stack_lock);
    int processId = __cilkrts_get_worker_number();
    processQueue[processId]->push(task);
    work[processId] += n*n*n;
    return 1;
}

int initScheduler( int p_maxCores, int (*function)(int **, int **, int **,int,int,int,int,int,int,int),
                   int **Z, int **X, int **Y,
                   int n,vector<int>* stack,mutex* stack_lock){

    printf("\nRunning Steal Mod Scheduler\n");
    ready = false;
    maxCores =p_maxCores;
    terminated = false;

    max_attempts = 2*maxCores*log(maxCores);

    Task* initialTask = new Task(function,Z,X,Y,0,0,0,0,0,0,n,stack,stack_lock);
    processQueue[0] = new tbb::concurrent_queue<Task*>();

    processQueue[0]->push(initialTask);

    int id =1;
    while(id < maxCores){
        processQueue[id] = new tbb::concurrent_queue<Task*>();
        work[id] =0;
        cilk_spawn start();
        id++;
    }

    ready = true;
    cilk_sync;
    return 1;
}




#endif //HW1_STEALMOD_H
