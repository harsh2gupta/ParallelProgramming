////
//// Created by Shweta Sahu on 3/13/18.
////
//
//#ifndef HW1_COMMON_H
//#define HW1_COMMON_H
//#include <mutex>
//#include <atomic>
//using namespace std;
//enum SchedulerType { DRSteal, DRShare, CShare, DRStealMod, DRShareMod};
//static string SchedulerName[] = {"DRSteal", "DRShare", "CShare", "DRStealMod", "DRShareMod"};
//
//
//int initScheduler(SchedulerType schedulerType, int p_maxCores, int (*function)(int **, int **, int **,int,int,int,int,int,int,int),
//                  int **Z, int **X, int **Y,
//                  int n,vector<int>* stack,mutex* stack_lock);
////supports only matrix multiplication
//int _sync(vector<int>* stack,mutex* stack_lock,int **Z, int **X, int **Y);
//int spawn(int (*function)(int **, int **, int **,int, int, int ,int,int, int,int),
//          int **Z, int **X, int **Y,
//          int Zr,int Zc,
//          int Xr,int Xc,
//          int Yr,int Yc,
//          int n,vector<int>* stack,mutex* stack_lock);
//static atomic<bool> init (true);
//static vector<int>* mainStack ;
//
//#endif //HW1_COMMON_H
