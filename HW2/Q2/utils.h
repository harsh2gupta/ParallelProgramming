//
// Created by Harsh Gupta on 4/14/18.
//

#ifndef PARALLELPROGRAMMING_UTILS_H
#define PARALLELPROGRAMMING_UTILS_H

#include <vector>
#include<iostream>
#include <random>
#include <ctime>
#include <chrono>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
using namespace std;

// RM added for Task 2
enum COIN_FACE{
    FACE_HEAD = 0,
    FACE_TAIL = 1
};

class Edge{
    public:
    int u;
    int v;
    float weight;
    Edge(){
        u = 0;
        v = 0;
        weight = 0;
    }
    Edge(int a, int b, float c)
    {
        u = a;
        v = b;
        weight = c;
    }
    Edge(const Edge &t)
    {
       u = t.u;
       v = t.v;
       weight = t.weight;
    }
    Edge& operator = (const Edge &t)
    {
       u = t.u;
       v = t.v;
       weight = t.weight;
    }
};

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;

static std::random_device rd; // random device engine
// initialize Mersennes' twister using rd to generate the seed
static std::mt19937 rng(rd());
int  getRandom(int q,int r){
    std::uniform_int_distribution<int> uid(q,r); // random dice
    return uid(rng); // use rng as a generator
}


vector<int> par_prefix_sum(vector<int> &x){
    int n = x.size();
    vector<int> s(n);
    if(n==1){
        s[0] = x[0];
    }else{
        vector<int> y(n/2);
        for(int i=0;i<n/2;i++){
            y[i]=x[2*i+1]+x[2*i];
        }
        vector<int> z = par_prefix_sum(y);
        cilk_for(int i=0;i<n;i++){
            if(i==0){
                s[i]=x[i];
            }else if(i%2!=0){
                s[i]=z[(i-1)/2];
            }else{
                s[i]=z[i/2-1]+x[i];
            }
        }
    }
    return s;
}

int par_partition(vector<Edge> &A,int q,int r,float x){
    int n = r-q+1;
    if(n==1){
        return q;
    }
    vector<float> B(n);
    vector<int> lt(n);
    vector<int> gt(n);
    int j=-1;
    cilk_for(int i=0;i<n;i++){
        B[i]=A[q+i].weight;
        if(B[i]<x){
            lt[i]=1;
            gt[i]=0;
        }else{
            lt[i]=0;
            gt[i]=1;
        }
        if(B[i]==x){
            gt[i]=0;
            j++;
        }
    }
    lt = par_prefix_sum(lt);
    gt = par_prefix_sum(gt);
    int k = q+lt[n-1]+j;
    //A[k] = x;

    cilk_for(int i=0; i<n;i++){
        if(B[i]<x){
            A[q+lt[i]-1].weight=B[i];
        }else if(B[i]>x){
            A[k+gt[i]].weight=B[i];
        }
        else{
            A[k-j].weight=x;
            j--;
        }
    }
    return k;
}

#endif //PARALLELPROGRAMMING_UTILS_H
