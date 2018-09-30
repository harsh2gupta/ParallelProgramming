//
// Created by Robin Manhas on 04/17/18.
//

#include "utils.h"
#include <fstream>

void par_randomized_quicksort(vector<Edge> &A,int q,int r,int M){
    int n = r-q+1;
    if(n<=M){
        int i, j;
        Edge key;
        for (i = q; i <= r; i++)
        {
            key = A[i];
            j = i-1;
            while (j >= 0 && A[j].weight > key.weight)
            {
                A[j+1] = A[j];
                j = j-1;
            }
            A[j+1] = key;
        }
    }else{
        int temp = getRandom(q,r);
        float x = A[temp].weight;
        int k = par_partition(A,q,r,x);
        cilk_spawn par_randomized_quicksort(A,q,k-1,M);
        //par_randomized_quicksort(A,q,k-1);
        par_randomized_quicksort(A,k+1,r,M);
        cilk_sync;
    }

}

void par_counting_rank(vector<int> &S, int n,int d,vector<int> &r,int p){
    int max = pow(2,d)-1;
    vector<vector<int>> f(max+1,vector<int>(p+1));
    vector<vector<int>> r1(max+1,vector<int>(p+1));
    vector<int> j_start(p+1);
    vector<int> j_end(p+1);
    vector<int> ofs(p+1);

    cilk_for(int i =1; i <=p; i++){
        for(int j=0; j <= max; j++){
            f[j][i] =0;
        }

        j_start[i]=(i-1)*floor(n/p)+1;
        j_end[i] = (i<p)?i*floor(n/p):n;

        for(int j = j_start[i]; j<=j_end[i];j++){
            f[S[j]][i]++;
        }
    }

    for(int j =0 ; j<=max; j++){
        f[j] = par_prefix_sum(f[j]);
    }

    cilk_for(int i =1; i <=p; i++){
        ofs[i] =1;
        for(int j=0; j <= max; j++){
            r1[j][i] = (i==1)?ofs[i]:(ofs[i]+f[j][i-1]);
            ofs[i] = ofs[i] +f[j][p];
        }
        for(int j = j_start[i]; j<=j_end[i];j++){
            r[j]= r1[S[j]][i];
            r1[S[j]][i]++;
        }
    }

}
int extract_bit_segment(int x, int a , int b){
    return (x>>a)&((1<<(b-a+1))-1);
}
void par_radix_sort_with_counting_rank(vector<int> &A, int n, int b,int p) {
    vector<int> r(n+1);
    vector<int> B(n+1);
    vector<int> S(n+1);

    int d = ceil(log(n/(p*log(n))));
    d = d<1?1:d;
    for(int k =0; k<=(b-1); k=k+d){
        int q = (k+d)<=b ? d:(b-k);
        cilk_for(int i =1; i <=n; i++){
            S[i] = extract_bit_segment(A[i],k,k+q-1);
        }
        par_counting_rank(S,n,q,r,p);
        cilk_for(int i=1;i<=n;i++){
            B[r[i]]= A[i];
        }
        cilk_for(int i=1;i<=n;i++){
            A[i]=B[i];
        }
    }
}

void Par_Simulate_Priority_CW_using_Radix_Sort_2(int n, vector<Edge> &E, vector<int> &R, int p){
    int edgeListSize = E.size();
    vector<int> A(edgeListSize);
    int k = ceil(log(edgeListSize))+1;

    cilk_for(int i = 1; i < edgeListSize; i++){
        A[i] = (E[i].u << k) + i;
    }

    par_radix_sort_with_counting_rank(A, edgeListSize, (k+ ceil(log(n))), p);

    cilk_for(int i = 1; i < edgeListSize; i++){
        int u = A[i] >> k;
        int j = A[i] - (u << k);
        if(i == 1 || u != (A[i-1] >> k)){
            R[u] = j;
        }
    }

}

void Par_Simulate_Priority_CW_using_Binary_Search(int n, vector<Edge> &E, vector<int> &R){
    vector<int> B(n+1);
    vector<int> l(n+1);
    vector<int> h(n+1);
    vector<int> lo(n+1);
    vector<int> hi(n+1);
    vector<int> md(n+1);

    int edgeListSize = E.size();
    cilk_for(int u = 1; u <= n; u++){
        l[u] = 1;
        h[u] = edgeListSize;
    }

    for(int k = 1; k < (1+log(edgeListSize)); k++){
        cilk_for(int u = 1; u <= n; u++){
            B[u] = 0;
            lo[u] = l[u];
            hi[u] = h[u];
        }

        cilk_for(int i = 1; i < edgeListSize; i++){
            int u = E[i].u;
            md[u] = floor( (lo[u]+hi[u])/2 );
            if(i >= lo[u] && i <= md[u]){
                B[u] = 1;
            }
        }

        cilk_for(int i = 1; i < edgeListSize; i++){
            int u = E[i].u;
            md[u] = floor( (lo[u]+hi[u])/2 );
            if(B[u] == 1 && i >= lo[u] && i<=md[i]){
                h[u] = md[u];
            }
            else if(B[u] == 0 && i > md[i] && i<=hi[u]){
                l[u] = md[u] + 1;
            }
        }
    }

    cilk_for(int i = 1; i < edgeListSize; i++){
        int u = E[i].u;
        if(i == l[u])
            R[u] = i;
    }
}

void Par_Randomized_MST_Priority_CW (int n, vector<Edge> &E, vector<bool> &MST){
	int M = 32;
	int b = 32; // 32 bit integer
	int p = 64; // number of processors
	bool F = false;
	vector<int> L(n+1);
	vector<int> C(n+1);
	vector<int> R(n+1);

	par_randomized_quicksort(E,1,E.size()-1,M);

//    for(int i=1;i<E.size();i++){
//        cout <<"Sorted u: "<<E[i].u<<" ,v: "<<E[i].v<<" ,weight: "<<E[i].weight<<endl;
//    }

	cilk_for(int v = 1; v <= n; v++){
		L[v] = v;
	}

	F = (E.size() > 0);
    while(F){
		cilk_for(int v = 1; v <= n; v++){
			C[v] = getRandom(0,1); // head : 0, tail : 1
            //cout <<"got random: "<<C[v]<<endl;
		}

        Par_Simulate_Priority_CW_using_Binary_Search(n,E,R);

		cilk_for(int i = 1; i<E.size();i++){
			int u = E[i].u;
			int v = E[i].v;
			if(C[u] == FACE_TAIL && C[v] == FACE_HEAD && R[u] == i){
				L[u] = v;
				MST[i] = 1;
                //cout<<"Setting MST "<<i<<" to "<<MST[i]<<endl;
			}
		}

		cilk_for(int i = 1; i<E.size();i++){
			E[i].u = L[E[i].u];
			E[i].v = L[E[i].v];
		}

		F = false;

		cilk_for(int i = 1; i<E.size();i++){
			if(E[i].u != E[i].v) {
                F = true;
            }
		}
	}
}

int main(){
    std::ifstream infile("com-youtube-in.txt");
    int n,m;
    infile >> n >> m;
    vector<Edge> E;
    cout <<"n: "<<n<<" ,m: "<<m<<endl;
    E.push_back(Edge(0,0,0));
    for(int i=0;i<m;i++){
        int a,b; float c;
        infile >> a >> b >> c;
        E.push_back(Edge(a,b,c));
    }

    vector<bool> MST(E.size(),false);
    Par_Randomized_MST_Priority_CW (n, E, MST);

    float sum= 0.0f;
    for(int i=1;i<MST.size();i++)
    {
        if(MST[i] == 1) {
            sum += E[i].weight;
        }
    }

    cout << "Weight of MST "<< sum<< endl;
    return 0;
}



