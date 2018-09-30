////
//// Created by Shweta Sahu on 3/13/18.
////
//
//#ifndef HW1_TASK_H
//#define HW1_TASK_H
//
//#include <iostream>
//#include <thread>
//#include "Scheduler.h"
//using namespace std;
//class Task {
//public:
//    Task( int (*function)(int **, int **, int **, int, int, int ,int,int, int,int),
//          int **Z, int **X, int **Y,
//          int Zr,int Zc,
//          int Xr,int Xc,
//          int Yr,int Yc,
//          int n ,vector<int>* stack,mutex* stack_lock) : Z(Z), X(X), Y(Y),Zr(Zr),Zc(Zc),Xr(Xr),Xc(Xc),Yr(Yr),Yc(Yc),n(n),function(function),stack(stack),stack_lock(stack_lock) {}
//
//    int n;
//    int execute() {
//        //printf("Inside Task execute n=%d thread=%x ------------------------------------\n",n,this_thread::get_id());
//        function(Z,X,Y,Zr,Zc,Xr,Xc,Yr,Yc,n);
//        //printf("Task executed sucessfully n=%d thread=%x --------------------------------\n",n,this_thread::get_id());
//        _sync(stack,stack_lock,Z,X,Y);
//        return 1;
//
//    }
//    vector<int>* stack;
//    mutex* stack_lock;
//
//    int** Z;
//    int** X;
//    int** Y;
//    int Zr,Zc,Xr,Xc,Yr,Yc;
//    int (*function)(int **, int **, int **, int, int, int ,int,int, int,int) = 0;
//
//
//};
//
//
//
//#endif //HW1_TASK_H
