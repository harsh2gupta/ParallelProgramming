//
// Created by Shweta Sahu on 3/14/18.
//
#include <iostream>
#include <random>
//#include <cilk/cilk.h>
//#include "Scheduler.h"
#include "Share.h"
#include <mutex>
#include "papi.h"
using namespace std;


static random_device r; // random device engine
//// initialize Mersennes' twister using rd to generate the seed
static mt19937 rngen(r());
int mm(int** Z, int** X, int** Y,
       int Zr,int Zc,
       int Xr,int Xc,
       int Yr,int Yc,
       int n);

float calculateGFLOPS(int n , int secs){
    float FLOPS = 2*n*n*n/secs;
    return FLOPS/pow(10,9);
}

void second_half(int **Z, int **X, int **Y, int Zr, int Zc, int Xr, int Xc, int Yr, int Yc, int n,vector<int>* stack) ;
bool checkResult(int** A, int ** B, int n);
void multiply(int **Z, int **X, int **Y,int n){
    for(int i=0;i<n;i++){
        for(int k=0;k<n;k++){
            for(int j=0;j<n;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }

}
int  randomNumber(){
    static uniform_int_distribution<int> uid(1,10); // random dice
    return uid(rngen); // use rng as a generator
}

void fillArray(int** A, int n){
    for(int i =0; i<n ; i++){
        for(int j =0 ; j<n ; j++){
            A[i][j]= randomNumber();
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
extern atomic<bool> terminated;
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
    spawn(mm, Z, X, Y,
          Zr, Zc,
          Xr, Xc,
          Yr, Yc, n, stack, stack_lock);
    //Z12, X11, Y12
    spawn(mm, Z, X, Y,
          Zr, Zc+n,
          Xr, Xc,
          Yr, Yc + n , n, stack,stack_lock);
    //Z21, X21, Y11
    spawn(mm, Z, X, Y,
          Zr + n , Zc,
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
    spawn(mm, Z, X, Y,
          Zr, Zc,
          Xr, Xc + n,
          Yr + n, Yc, n, stack,stack_lock2);
    //Z12, X12, Y22
    spawn(mm, Z, X, Y,
          Zr, Zc + n ,
          Xr, Xc + n ,
          Yr + n , Yc + n , n, stack,stack_lock2);
    //Z21 , X22 , Y21
    spawn(mm, Z, X, Y,
          Zr + n , Zc,
          Xr + n , Xc + n ,
          Yr + n , Yc, n, stack,stack_lock2);
    //Z22 , X22 , Y22
    mm(Z,X,Y,
       Zr+n,Zc+n,
       Xr+n,Xc+n,
       Yr+n,Yc+n,n);

    _sync(stack,stack_lock2,Z,X,Y);
}
atomic<long> _count;
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

    int n = pow(2,s);
    int **Z = new int*[n];
    int **X = new int*[n];
    int **Y = new int*[n];
    int **W = new int*[n];
    for(int i = 0; i <n; i++){
        X[i] = new int[n];
        Y[i] = new int[n];
        Z[i] = new int[n];
        W[i] = new int[n];
    }
    _count = n*n*n;
    fillArray(X,n);
    fillArray(Y,n);
    fillZero(Z,n);
    fillZero(W,n);


//    printf("Matrix X values \n");
//    printArray(X,n);
//    printf("\nMatrix Y values \n");
//    printArray(Y,n);

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
    time_t start = time(0);
    cout<<"start\n";
    initScheduler(cores,mm,Z,X,Y,n,stack,stack_lock);
    time_t end = time(0);
    int duration = end-start;
    printf("duration %d secs",duration);


//    //printArray(Z,n);

    return duration;

}

#define error(func, ret) \
    { fprintf(stderr, "%s failed.\n", func); if (ret) exit(ret);}

// static PAPI_hw_info_t *info;
static int events = PAPI_NULL;
static long long counters[128];
static char *events_desc[128];

static void cleanup(void)
{
    if (PAPI_is_initialized()) PAPI_shutdown();
}

static void
add_event(int code, const char *name, const char *msg)
{
    static int nevents = 0;

    if (PAPI_query_event(code) == PAPI_OK) {
        if (PAPI_add_event(events, code) != PAPI_OK) {
            fprintf(stderr, "PAPI_add_event failed for %s (%s).\n", name, msg);
        } else {
            fprintf(stderr, "Successfully added PAPI_FP_OPS.\n");
            events_desc[nevents] = (char*)malloc(strlen(msg) + strlen(name) + 5);
            sprintf(events_desc[nevents], "%s (%s):", msg, name);
            nevents++;
        }
    } else {
        fprintf(stderr, "No event counter for %s.\n", msg);
    }
}


void main(){
    int s = 5;
    int cores = 64;
    int n;
    PAPI_option_t options;

    if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT)
    error("PAPI_library_init", 1);

    atexit(cleanup);
    if (PAPI_create_eventset(&events) != PAPI_OK)
    error("PAPI_create_eventset", 1);
#if 0   /* seems multiplexing does not work on PIII */
    if (PAPI_multiplex_init() != PAPI_OK)
    error("PAPI_multiplex_init", 1);
    if (PAPI_set_multiplex(events) != PAPI_OK)
    error("PAPI_set_multiplex", 1);
#endif
    options.domain.eventset = events;
    options.domain.domain = PAPI_DOM_ALL;
    PAPI_set_opt(PAPI_DOMAIN, &options);

    //add_event(PAPI_FP_OPS,  "PAPI_FP_OPS",  "floating point operations");
    //add_event(PAPI_FP_INS,  "PAPI_FP_INS",  "floating point instructions");

    //add_event(PAPI_L1_TCM,  "PAPI_L1_TCM",  "L1 total cache misses");
    add_event(PAPI_L2_TCM,  "PAPI_L2_TCM",  "L2 total cache misses");
    add_event(PAPI_L3_TCM,  "PAPI_L3_TCM",  "L3 total cache misses");
    //add_event(PAPI_L1_TCM,  "PAPI_L1_TCH",  "L1 total cache hits");
    //add_event(PAPI_L2_TCM,  "PAPI_L2_TCH",  "L2 total cache hits");

    /* Translation Lookaside Buffer */
    //add_event(PAPI_TLB_TL,  "PAPI_TLB_TL",  "TLB buffer misses");

    n = PAPI_num_events(events);
    fprintf(stderr, "%d counters initialized.\n", n);

    PAPI_start(events);
    PAPI_reset(events);

    multiply(s,cores);

    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "%-40s %lld\n", events_desc[i], counters[i]);
    }

}



