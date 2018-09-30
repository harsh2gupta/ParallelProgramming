#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include "cilk/cilk.h"
#include "papi.h"
#define R 1024 // 2^11, therefore R = 11
int M;
using namespace std;
int X[R][R],Y[R][R],Z[R][R];

typedef int recMatrix[R][R];
typedef struct{
    int row_s, row_e, col_s, col_e;
}border;

void iter_IKJ(recMatrix X, recMatrix Y, recMatrix Z, border x, border y, border z){
    for(int i=0;i<M;i++){
        for(int k=0;k<M;k++){
            for(int j=0;j<M;j++){
                Z[i+z.row_s][j+z.col_s] += X[i+x.row_s][k+x.col_s]*Y[k+y.row_s][j+y.col_s];
            }
        }
    }
}

void par_rec_mm(recMatrix X, recMatrix Y, recMatrix Z, border x, border y, border z) {
    // instead of individually storing each submatrix borders..
    border xii[2][2], yii[2][2], zii[2][2];

    int interval = x.row_e - x.row_s;

    if (interval==M) {
        iter_IKJ(X,Y,Z,x,y,z);
    }
    else{
        // Create the smaller matrices:
        for(int i=0;i<2;i++) {
            for(int j=0;j<2;j++) {
                xii[i][j] = x;
                yii[i][j] = y;
                zii[i][j] = z;

                // only update necessary border
                if(i == 0){
                    xii[i][j].row_e = x.row_s+(x.row_e - x.row_s)/2;
                    yii[i][j].row_e = y.row_s+(y.row_e - y.row_s)/2;
                    zii[i][j].row_e = z.row_s+(z.row_e - z.row_s)/2;
                }
                else if(i == 1){
                    xii[i][j].row_s = x.row_s+(x.row_e - x.row_s)/2;
                    yii[i][j].row_s = y.row_s+(y.row_e - y.row_s)/2;
                    zii[i][j].row_s = z.row_s+(z.row_e - z.row_s)/2;
                }
                if(j == 0){
                    xii[i][j].col_e = x.col_s+(x.col_e - x.col_s)/2;
                    yii[i][j].col_e = y.col_s+(y.col_e - y.col_s)/2;
                    zii[i][j].col_e = z.col_s+(z.col_e - z.col_s)/2;
                }
                else if(j == 1){
                    xii[i][j].col_s = x.col_s+(x.col_e - x.col_s)/2;
                    yii[i][j].col_s = y.col_s+(y.col_e - y.col_s)/2;
                    zii[i][j].col_s = z.col_s+(z.col_e - z.col_s)/2;
                }

            }
        }

        // Now do the 8 sub matrix multiplications in the algo mentioned in question.
        cilk_spawn par_rec_mm(X,Y,Z,xii[0][0],yii[0][0],zii[0][0]); // Z11, X11, Y11
        cilk_spawn par_rec_mm(X,Y,Z,xii[0][0],yii[0][1],zii[0][1]); // Z12, X11, Y12
        cilk_spawn par_rec_mm(X,Y,Z,xii[1][0],yii[0][0],zii[1][0]); // Z21, X21, Y11
        par_rec_mm(X,Y,Z,xii[1][0],yii[0][1],zii[1][1]); // Z22, X21, Y12
        cilk_sync;

        cilk_spawn par_rec_mm(X,Y,Z,xii[0][1],yii[1][0],zii[0][0]); // Z11, X12, Y21
        cilk_spawn par_rec_mm(X,Y,Z,xii[0][1],yii[1][1],zii[0][1]); // Z12, X12, Y22
        cilk_spawn par_rec_mm(X,Y,Z,xii[1][1],yii[1][0],zii[1][0]); // Z21, X22, Y21
        par_rec_mm(X,Y,Z,xii[1][1],yii[1][1],zii[1][1]); // Z22, X22, Y22
        cilk_sync;
    }

}


void iter_IKJ_I(){
    cilk_for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_IJ(){
    cilk_for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}


void iter_KIJ_I(){
    for(int k=0;k<R;k++){
        cilk_for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_IJ(){
    for(int k=0;k<R;k++){
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


int main(){
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

    add_event(PAPI_L1_TCM,  "PAPI_L1_TCM",  "L1 total cache misses");
    //add_event(PAPI_L2_TCM,  "PAPI_L2_TCM",  "L2 total cache misses");
    //add_event(PAPI_L3_TCM,  "PAPI_L3_TCM",  "L3 total cache misses");
    //add_event(PAPI_L1_TCM,  "PAPI_L1_TCH",  "L1 total cache hits");
    //add_event(PAPI_L2_TCM,  "PAPI_L2_TCH",  "L2 total cache hits");

    /* Translation Lookaside Buffer */
    //add_event(PAPI_TLB_TL,  "PAPI_TLB_TL",  "TLB buffer misses");

    n = PAPI_num_events(events);
    fprintf(stderr, "%d counters initialized.\n", n);
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
    PAPI_start(events);
    PAPI_reset(events);
    iter_IKJ_I();
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "iter_IKJ_I %-40s %lld\n", events_desc[i], counters[i]);
    }
    auto end = Time::now();
    auto d = end - start;
    auto diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_IKJ_I, "<< diff.count() << " ms" <<endl;
    reset();
   
    start = Time::now();
    PAPI_start(events);
    PAPI_reset(events);
    iter_IKJ_IJ();
    end = Time::now();
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "iter_IKJ_IJ %-40s %lld\n", events_desc[i], counters[i]);
    }
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_IKJ_IJ, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    PAPI_start(events);
    PAPI_reset(events);
    iter_KIJ_I();
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "iter_KIJ_I %-40s %lld\n", events_desc[i], counters[i]);
    }
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_KIJ_I, "<< diff.count() << " ms" <<endl;
    reset();

    start = Time::now();
    PAPI_start(events);
    PAPI_reset(events);
    iter_KIJ_IJ();
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "iter_KIJ_IJ %-40s %lld\n", events_desc[i], counters[i]);
    }
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"iter_KIJ_IJ, "<< diff.count() << " ms" <<endl;
    reset();

    border xb = {0,R,0,R};
    border yb = {0,R,0,R};
    border zb = {0,R,0,R};
    start = Time::now();
    M = 32;
    PAPI_start(events);
    PAPI_reset(events);
    par_rec_mm(X, Y, Z, xb, yb, zb);
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "par_rec_mm 32 %-40s %lld\n", events_desc[i], counters[i]);
    }
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 32, "<< diff.count() << " ms" <<endl;
    reset();

    return 0;
}

