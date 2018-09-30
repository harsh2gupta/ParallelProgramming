//
// Created by Shweta Sahu on 3/13/18.
//

#include <iostream>
#include <thread>
#include <random>
#include "Scheduler.h"

using namespace std;
//atomic<bool> terminated;
//static std::random_device rd; // random device engine
//// initialize Mersennes' twister using rd to generate the seed
//static std::mt19937 rng(rd());
//static unordered_map<thread::id,Process*> processes;
//static vector<Process*> thread_ids;

//int initScheduler(SchedulerType pschedulerType, int p_maxCores, int (*function)(int **, int **, int **,int,int,int,int,int,int,int),
//                  int **Z, int **X, int **Y,
//                   int n,vector<int>* stack,mutex* stack_lock){
//    maxCores =p_maxCores;
//    terminated = false;
//    ready = false;
//    max_attempts = 2*maxCores*log(maxCores);
//    schedulerType = pschedulerType;
//    int id =0;
//    while(id < maxCores){
//        Process* p = new Process();
//        thread_ids.push_back(p);
//        id++;
//    }
//    std::this_thread::sleep_for (std::chrono::seconds(2));
//    for (auto p :thread_ids){
//        processes[p->getId()]=p;
//    }
//
//    initialTask = new Task(function,Z,X,Y,0,0,0,0,0,0,n,stack,stack_lock);
//    switch (schedulerType){
//        case CShare : taskQueue->push_back(initialTask);
//            break;
//        default: thread_ids[0]->taskQueue->push_back(initialTask);
//    }
//
//    ready = true;
//    for(auto p:thread_ids){
//        p->join();
//    }
//    return 1;
//}
//
//
//int spawn(int (*function)(int **, int **, int **,int,int,int,int,int,int,int),int **Z, int **X, int **Y,
//          int Zr,int Zc,
//          int Xr,int Xc,
//          int Yr,int Yc,int n,vector<int>* stack,mutex* stack_lock){
//    //printf("inside scheduler spawn thread id %x\n",std::this_thread::get_id());
//    Task* task = new Task(function,Z,X,Y,Zr,Zc,Xr,Xc,Yr,Yc,n,stack,stack_lock);
//    //printf("********Next task into the queue is for n %d  %d %d\n", task->n, task->Zr, task->Zc);
//    std::thread::id processId = std::this_thread::get_id();
//
//    switch(schedulerType){
//         case CShare: taskQueue->push_back(task);
//             break;
//         case DRStealMod:
//             processes[processId]->taskQueue->work += n*n*n;
//         case DRSteal:
//             processes[processId]->taskQueue->push_back(task);
//             break;
//         case DRShare:
//             thread_ids[getRandom()]->taskQueue->push_back(task);
//             break;
//         case DRShareMod:
//             ConcurrentDeque<Task*> *q1 = thread_ids[getRandom()]->taskQueue;
//             ConcurrentDeque<Task*> *q2 = thread_ids[getRandom()]->taskQueue;
//             ConcurrentDeque<Task*> *q = (q1->work < q2->work)?q1:q2;
//             q1->work += n*n*n;
//             q1->push_back(task);
//             break;
//     }
//    return 1;
//}
//
// Task* lookForTask(ConcurrentDeque<Task*>* taskQueue, bool isSteal){
//    int count =0;
//    Task* task = NULL;
//    //if(taskQueue)
//    while(ready && task == NULL && (count < max_attempts) && !terminated){
//        task = (isSteal)?thread_ids[getRandom()]->taskQueue->pop_back():taskQueue->pop_front();
//        count++;
//    }
//    return task;
//}
//
//Task* lookForModTask(){
//    int count =0;
//    Task* task = NULL;
//    ConcurrentDeque<Task*> *q;
//    while(ready && task == NULL && (count < max_attempts) && !terminated){
//        ConcurrentDeque<Task*> *q1 = thread_ids[getRandom()]->taskQueue;
//        ConcurrentDeque<Task*> *q2 = thread_ids[getRandom()]->taskQueue;
//        q = (q1->work > q2->work)?q1:q2;
//        task = q->pop_back();
//        count++;
//    }
//    int n = task->n;
//    q->work -= n*n*n;
//    return task;
//}
//
//
//
// Task* getNextTask(Process* p){
//    Task* task = NULL;
//     int n;
//     switch(schedulerType){
//         case CShare:  task = lookForTask(taskQueue,false);
//             break;
//         case DRStealMod:
//             task = p->taskQueue->pop_front();
//             if(task == NULL){
//                 task = lookForModTask();
//             }
//             break;
//         case DRSteal:
//             task = p->taskQueue->pop_front();
//             if(task == NULL){
//                 task = lookForTask(p->taskQueue,true);
//             }
//             break;
//         case DRShareMod:
//             task = lookForTask(p->taskQueue,false);
//             n = task->n;
//             p->taskQueue->work -= n*n*n;
//             break;
//         case DRShare:
//             task = lookForTask(p->taskQueue,false);
//             break;
//     }
//     //if (task != NULL) printf("********Next task out of queue is for n %d  %d\n", task->n, task->Zr, task->Zc);
//    return task;
//
//}
//
//
//
//
//int  getRandom(){
//    static std::uniform_int_distribution<int> uid(1,maxCores); // random dice
//    return uid(rng)-1; // use rng as a generator
//}
//Process::Process(){
//    //id = p_id;
//    taskQueue = new ConcurrentDeque<Task*>();
//    run();
//}
//void Process::start(){
//    thread_id = std::this_thread::get_id();
//    //printf("Thread created %x\n",thread_id);
//    while(!terminated){
//        if(ready){
//            Task * task = getNextTask(this);
//            if(task != NULL){
//                task->execute();
//            }
//        }
//    }
//
//}
//
//void Process::run() {
//    //cout<<"Creating new thread for Process \n";
//    t= thread(std::bind(&Process::start,this));
//    //cout<<"Thread created successfully \n";
//}
//
//void Process::join(){
//    t.join();
//}
//thread::id Process::getId(){
//    return thread_id;
//}
//
//
//
