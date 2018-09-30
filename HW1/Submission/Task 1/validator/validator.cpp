#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <cstdlib>
#include <time.h>
#include "cilk/cilk.h"

#define R 8
using namespace std;
int X[R][R],Y[R][R],Z[R][R],ZOUT[R][R];

void iter_IJK_0(){
    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            for(int k=0;k<R;k++){
                ZOUT[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_0(){
    for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_1(){
    cilk_for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_2(){
    for(int i=0;i<R;i++){
        cilk_for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_3(){
    for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_4(){
    cilk_for(int i=0;i<R;i++){
        cilk_for(int k=0;k<R;k++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_5(){
    for(int i=0;i<R;i++){
        cilk_for(int k=0;k<R;k++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_6(){
    cilk_for(int i=0;i<R;i++){
        for(int k=0;k<R;k++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_IKJ_7(){
    cilk_for(int i=0;i<R;i++){
        cilk_for(int k=0;k<R;k++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_0(){
    for(int k=0;k<R;k++){
        for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_1(){
    cilk_for(int k=0;k<R;k++){
        for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_2(){
    for(int k=0;k<R;k++){
        cilk_for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_3(){
    for(int k=0;k<R;k++){
        for(int i=0;i<R;i++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_4(){
    cilk_for(int k=0;k<R;k++){
        cilk_for(int i=0;i<R;i++){
            for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_5(){
    for(int k=0;k<R;k++){
        cilk_for(int i=0;i<R;i++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_6(){
    cilk_for(int k=0;k<R;k++){
        for(int i=0;i<R;i++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void iter_KIJ_7(){
    cilk_for(int k=0;k<R;k++){
        cilk_for(int i=0;i<R;i++){
            cilk_for(int j=0;j<R;j++){
                Z[i][j] += X[i][k]*Y[k][j];
            }
        }
    }
}

void printArray(){
    for(int i =0; i<R ; i++){
        for(int j =0 ; j<R ; j++){
            cout<<Z[i][j]<<" ";
        }
        cout<<"\n";
    }
}

void reset(){
    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            Z[i][j] = 0;
        }
    }
}

int compare(){
    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            if(Z[i][j] != ZOUT[i][j])
            	return 1;
        }
    }

    return 0;
}

int main(){

    time_t start,end;
    srand(time(NULL));

    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            X[i][j] = Y[i][j] = rand()%5000 + 1;
            Z[i][j] = 0;
        }
    }

    time(&start);
    iter_IJK_0();
    time(&end);
    cout <<"iter_IJK_0, "<<(end-start) <<endl;
    printArray();
    reset();

    time(&start);
    iter_IKJ_1();
    time(&end);
    cout <<"iter_IKJ_i cilk i, "<<(end-start) <<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();

    time(&start);
    iter_IKJ_2();
    time(&end);
    cout <<"iter_IKJ_k, "<<(end-start) <<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_IKJ_3();
    time(&end);
    cout <<"iter_IKJ_j, "<<(end-start) <<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_IKJ_4();
    time(&end);
    cout <<"iter_IKJ_ik, "<<(end-start) <<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_IKJ_5();
    time(&end);
    cout <<"iter_IKJ_kj, "<<(end-start) <<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_IKJ_6();
    time(&end);
    cout <<"iter_IKJ_ij, "<<(end-start) <<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_IKJ_7();
    time(&end);
    cout <<"iter_IKJ_ikj, "<<(end-start)<<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_KIJ_1();
    time(&end);
    cout <<"iter_KIJ_k, "<<(end-start) <<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_KIJ_2();
    time(&end);
    cout <<"iter_KIJ_i, "<<(end-start) <<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_KIJ_3();
    time(&end);
    cout <<"iter_KIJ_j, "<<(end-start) <<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_KIJ_4();
    time(&end);
    cout <<"iter_KIJ_ki, "<<(end-start)<<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_KIJ_5();
    time(&end);
    cout <<"iter_KIJ_ij, "<<(end-start)<<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_KIJ_6();
    time(&end);
    cout <<"iter_KIJ_kj, "<<(end-start)<<endl;
    printArray();
    if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    time(&start);
    iter_KIJ_7();
    time(&end);
    cout <<"iter_KIJ_kij, "<<(end-start)<<endl;
    printArray();
	if(compare() == 0){
    	cout << "value is same"<< endl;
    }
    else{
    	cout << "value is different"<<endl;
    }
    reset();
    
    return 0;
}


