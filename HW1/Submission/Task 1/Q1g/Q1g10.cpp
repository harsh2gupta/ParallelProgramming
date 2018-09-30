#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <cilk/cilk.h>
#define R 1024 // 2^10, therefore R = 10

int M;
using namespace std;

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

void reset(recMatrix X, recMatrix Y, recMatrix Z){
    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            X[i][j] = Y[i][j] = rand()%100 + 1;
            Z[i][j] = 0;
        }
    }
}

int main(){
    recMatrix X, Y, Z;
    srand(time(0));

    for(int i=0;i<R;i++){
        for(int j=0;j<R;j++){
            X[i][j] = Y[i][j] = rand()%100 + 1;
            Z[i][j] = 0;
        }
    }

    border xb = {0,R,0,R};
    border yb = {0,R,0,R};
    border zb = {0,R,0,R};

    typedef std::chrono::high_resolution_clock Time;
    typedef std::chrono::milliseconds ms;
    typedef std::chrono::duration<float> fsec;

    auto start = Time::now();
    M = 1;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    auto end = Time::now();
    fsec d = end - start;
    ms diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 1, "<< diff.count() << " ms" <<endl;
    reset(X, Y, Z);
    xb = {0,R,0,R};
    yb = {0,R,0,R};
    zb = {0,R,0,R};

    start = Time::now();
    M = 2;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 2, "<< diff.count() << " ms" <<endl;
    reset(X, Y, Z);
    xb = {0,R,0,R};
    yb = {0,R,0,R};
    zb = {0,R,0,R};

    start = Time::now();
    M = 4;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 4, "<< diff.count() << " ms" <<endl;
    reset(X, Y, Z);
    xb = {0,R,0,R};
    yb = {0,R,0,R};
    zb = {0,R,0,R};

    start = Time::now();
    M = 8;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 8, "<< diff.count() << " ms" <<endl;
    reset(X, Y, Z);
    xb = {0,R,0,R};
    yb = {0,R,0,R};
    zb = {0,R,0,R};

    start = Time::now();
    M = 16;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 16, "<< diff.count() << " ms" <<endl;
    reset(X, Y, Z);
    xb = {0,R,0,R};
    yb = {0,R,0,R};
    zb = {0,R,0,R};

    start = Time::now();
    M = 32;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 32, "<< diff.count() << " ms" <<endl;
    reset(X, Y, Z);
    xb = {0,R,0,R};
    yb = {0,R,0,R};
    zb = {0,R,0,R};

    start = Time::now();
    M = 64;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 64, "<< diff.count() << " ms" <<endl;
    reset(X, Y, Z);
    xb = {0,R,0,R};
    yb = {0,R,0,R};
    zb = {0,R,0,R};

    start = Time::now();
    M = 128;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 128, "<< diff.count() << " ms" <<endl;
    reset(X, Y, Z);
    xb = {0,R,0,R};
    yb = {0,R,0,R};
    zb = {0,R,0,R};

    start = Time::now();
    M = 256;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 256, "<< diff.count() << " ms" <<endl;
    reset(X, Y, Z);
    xb = {0,R,0,R};
    yb = {0,R,0,R};
    zb = {0,R,0,R};

    start = Time::now();
    M = 512;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 512, "<< diff.count() << " ms" <<endl;
    reset(X, Y, Z);
    xb = {0,R,0,R};
    yb = {0,R,0,R};
    zb = {0,R,0,R};

    start = Time::now();
    M = 1024;
    par_rec_mm(X, Y, Z, xb, yb, zb);
    end = Time::now();
    d = end - start;
    diff = std::chrono::duration_cast<ms>(d);
    cout <<"par_rec_mm 1024, "<< diff.count() << " ms" <<endl;

    return 0;
}
