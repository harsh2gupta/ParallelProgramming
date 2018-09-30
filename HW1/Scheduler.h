//
// Created by Shweta Sahu on 3/13/18.
//

#ifndef HW1_SCHEDULER_H
#define HW1_SCHEDULER_H

#include <deque>
#include <list>
#include <vector>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <unordered_map>
#include "ConcurrentDeque.h"
#include "tbb/concurrent_queue.h"
#include <mutex>
#include <atomic>
#include <iostream>
//#include <thread>


using namespace std;
enum SchedulerType { DRSteal, DRShare, CShare, DRStealMod, DRShareMod};

static string SchedulerName[] = {"DRSteal", "DRShare", "CShare", "DRStealMod", "DRShareMod"};
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


SchedulerType schedulerType;
int maxCores;

bool ready; //job in deque
atomic<bool> terminated;

tbb::concurrent_queue<Task*> *centralizedTaskQueue;

int max_attempts;
Task* lookForTask(ConcurrentDeque<Task*>& taskQueue, bool isSteal);
Task* getNextTask();

static std::random_device rd; // random device engine
// initialize Mersennes' twister using rd to generate the seed
static std::mt19937 rng(rd());

static unordered_map<int,ConcurrentDeque<Task*>*> processQueue;
vector<int> threadIds;
//static unordered_map<std::thread::id,ConcurrentDeque<Task*>*> processQueue;
//vector<std::thread::id> threadIds;


int  getRandom(){
    static std::uniform_int_distribution<int> uid(1,maxCores-1); // random dice
    return threadIds[uid(rng)-1]; // use rng as a generator
}

//std::thread::id  getRandom(){
//    static std::uniform_int_distribution<int> uid(1,maxCores-1); // random dice
//    return threadIds[uid(rng)-1]; // use rng as a generator
//}
void start(){
    int thread_id = __cilkrts_get_worker_number();
    //std::thread::id thread_id = std::this_thread::get_id();

    //printf("thread id %d\n", thread_id);
    threadIds.push_back(thread_id);
    processQueue[thread_id] = new ConcurrentDeque<Task*>();

    //printf("Thread created %x\n",thread_id);
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

//void do_join(std::thread& t)
//{
//    t.join();
//}

int initScheduler(SchedulerType pschedulerType, int p_maxCores, int (*function)(int **, int **, int **,int,int,int,int,int,int,int),
                  int **Z, int **X, int **Y,
                  int n,vector<int>* stack,mutex* stack_lock){

    printf("inside init");
    ready = false;
	maxCores =p_maxCores;
    terminated = false;

    max_attempts = 2*maxCores*log(maxCores);
    schedulerType = pschedulerType;
    int id =0;
    //vector<thread> threads ;
    Task* initialTask = new Task(function,Z,X,Y,0,0,0,0,0,0,n,stack,stack_lock);
    //threads.push_back(thread(start));
    centralizedTaskQueue = new tbb::concurrent_queue<Task*>();

    //threads.push_back(thread(start));
    //std::this_thread::sleep_for (std::chrono::seconds(1));
    //printf("thread created");
    switch (schedulerType){
        case CShare : centralizedTaskQueue->push(initialTask);
            break;
        default: //processQueue[1]->push_back(initialTask);
            //processQueue[threadIds[0]]->push_back(initialTask);
            processQueue[1]->push_back(initialTask);
    }
    while(id < maxCores-1){
       // threads.push_back(thread(start));
        cilk_spawn start();
        id++;
    }



    //std::this_thread::sleep_for (std::chrono::seconds(1));
    ready = true;
    //for_each(threads.begin(),threads.end(),do_join);
    cilk_sync;
    return 1;
}


int spawn(int (*function)(int **, int **, int **,int,int,int,int,int,int,int),int **Z, int **X, int **Y,
          int Zr,int Zc,
          int Xr,int Xc,
          int Yr,int Yc,int n,vector<int>* stack,mutex* stack_lock){
    //printf("inside scheduler spawn thread id %x\n",std::this_thread::get_id());
    Task* task = new Task(function,Z,X,Y,Zr,Zc,Xr,Xc,Yr,Yc,n,stack,stack_lock);
    //printf("********Next task into the queue is for n %d  %d %d\n", task->n, task->Zr, task->Zc);
    int processId = __cilkrts_get_worker_number();
    //std::thread::id processId = std::this_thread::get_id();

    switch(schedulerType){
        case CShare: centralizedTaskQueue->push_back(task);
            break;
        case DRStealMod:
            processQueue[processId]->work += n*n*n;
        case DRSteal:
            processQueue[processId]->push_back(task);
            break;
        case DRShare:
            processQueue[getRandom()]->push_back(task);
            break;
        case DRShareMod:
            ConcurrentDeque<Task*> *q1 = processQueue[getRandom()];
            ConcurrentDeque<Task*> *q2 = processQueue[getRandom()];
            ConcurrentDeque<Task*> *q = (q1->work < q2->work)?q1:q2;
            q1->work += n*n*n;
            q1->push_back(task);
            break;
    }
    return 1;
}

Task* lookForTask(ConcurrentDeque<Task*>* taskQueue, bool isSteal){
    int count =0;
    Task* task = NULL;
    //if(centralizedTaskQueue)
    while(ready && task == NULL && (count < max_attempts) && !terminated){
        task = (isSteal)?processQueue[getRandom()]->pop_back():taskQueue->pop_front();
        count++;
    }
    return task;
}

Task* lookForModTask(){
    int count =0;
    Task* task = NULL;
    ConcurrentDeque<Task*> *q;
    while(ready && task == NULL && (count < max_attempts) && !terminated){
        ConcurrentDeque<Task*> *q1 = processQueue[getRandom()];
        ConcurrentDeque<Task*> *q2 = processQueue[getRandom()];
        q = (q1->work > q2->work)?q1:q2;
        task = q->pop_back();
        count++;
    }
    if(task != NULL){
        int n = task->n;
        q->work -= n*n*n;
    }

    return task;
}



Task* getNextTask(){
    int processId = __cilkrts_get_worker_number();

    //std::thread::id processId = std::this_thread::get_id();

    Task* task = NULL;
    int n;
    switch(schedulerType){
        case CShare:  task = lookForTask(centralizedTaskQueue,false);
            break;
        case DRStealMod:
            task = processQueue[processId]->pop_front();
            if(task == NULL){
                task = lookForModTask();
            }
            break;
        case DRSteal:
            task = processQueue[processId]->pop_front();
            if(task == NULL){
                task = lookForTask(processQueue[processId],true);
            }
            break;
        case DRShareMod:
            task = lookForTask(processQueue[processId],false);
            n = task->n;
            processQueue[processId]->work -= n*n*n;
            break;
        case DRShare:
            task = lookForTask(processQueue[processId],false);
            break;
    }
    //if (task != NULL) printf("********Next task out of queue is for n %d  %d\n", task->n, task->Zr, task->Zc);
    return task;

}












#endif //HW1_SCHEDULER_H
