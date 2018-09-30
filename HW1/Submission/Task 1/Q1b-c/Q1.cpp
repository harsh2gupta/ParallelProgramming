#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <cstdlib>
#include <ctime>
#include <chrono>

#include "papi.h"
#define R 1024 // 2^10, therefore R = 10
using namespace std;
int X[R][R],Y[R][R],Z[R][R];

void iter_IJK(){
    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            for(int k=0;k<R;k++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ(){
    for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_JIK(){
    for(int j=0;j<R;j++){
        for(int i=0;i<R;i++){
            for(int k=0;k<R;k++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_JKI(){
    for(int j=0;j<R;j++){
        for(int k=0;k<R;k++){
            for(int i=0;i<R;i++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ(){
    for(int k=0;k<R;k++){
        for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KJI(){
    for(int k=0;k<R;k++)
    {
        for(int j=0;j<R;j++){
            for(int i=0;i<R;i++){
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
    add_event(PAPI_L2_TCM,  "PAPI_L2_TCM",  "L2 total cache misses");
    add_event(PAPI_L3_TCM,  "PAPI_L3_TCM",  "L3 total cache misses");
    //add_event(PAPI_L1_TCM,  "PAPI_L1_TCH",  "L1 total cache hits");
    //add_event(PAPI_L2_TCM,  "PAPI_L2_TCH",  "L2 total cache hits");

    /* Translation Lookaside Buffer */
    //add_event(PAPI_TLB_TL,  "PAPI_TLB_TL",  "TLB buffer misses");

    n = PAPI_num_events(events);
    fprintf(stderr, "%d counters initialized.\n", n);

    time_t start,end;
    srand(time(NULL));

    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            X[i][j] = Y[i][j] = rand()%5000 + 1;
        }
    }

    cout << "**********************************************************\n";
    PAPI_start(events);
    PAPI_reset(events);
    start = time(0);
    iter_IJK();
    end = time(0);
    cout <<"Program IJK end, Time elapsed: "<<(end-start)<<endl;
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "%-40s %lld\n", events_desc[i], counters[i]);
    }
    reset();
    cout << "**********************************************************\n";
    PAPI_start(events);
    PAPI_reset(events);
    start = time(0);
    iter_IKJ();
    end = time(0);
    cout <<"Program IKJ end, Time elapsed: "<<(end-start)<<endl;
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "%-40s %lld\n", events_desc[i], counters[i]);
    }
    reset();
    cout << "**********************************************************\n";
    PAPI_start(events);
    PAPI_reset(events);    
    start = time(0);
    iter_JIK();
    end = time(0);
    cout <<"Program JIK end, Time elapsed: "<<(end-start)<<endl;
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "%-40s %lld\n", events_desc[i], counters[i]);
    }
    reset();
    cout << "**********************************************************\n";
    PAPI_start(events);
    PAPI_reset(events);
    start = time(0);
    iter_JKI();
    end = time(0);
    cout <<"Program JKI end, Time elapsed: "<<(end-start)<<endl;
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "%-40s %lld\n", events_desc[i], counters[i]);
    }
    reset();
    cout << "**********************************************************\n";
    PAPI_start(events);
    PAPI_reset(events);
    start = time(0);
    iter_KIJ();
    end = time(0);
    cout <<"Program KIJ end, Time elapsed: "<<(end-start)<<endl;
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "%-40s %lld\n", events_desc[i], counters[i]);
    }
    reset();
    cout << "**********************************************************\n";
    PAPI_start(events);
    PAPI_reset(events);
    start = time(0);
    iter_KJI();
    end = time(0);
    cout <<"Program KJI end, Time elapsed: "<<(end-start)<<endl;
    PAPI_stop(events, counters);
    for (int i = 0; i < n; i++) {
        fprintf(stderr, "%-40s %lld\n", events_desc[i], counters[i]);
    }
    reset();
    return 0;
}
