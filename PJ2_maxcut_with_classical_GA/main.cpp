#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <bitset>
#include <numeric>

using namespace std;

vector<bitset<500>> generate_population(int P, int n){
    vector<bitset<500>> x_s;
    bitset<500> x;
    for (int i=0; i<P; i++){
        for (int j=0; j<n; j++){
            x[j] = rand() % 2;
        }
        x_s.push_back(x);
    }
    return x_s;
}

int compute_score(bitset<500> x, int edges[5000][3], int n, int e){
    int i;
    int score = 0;
    for (i=0; i<e; i++){
        if(x[edges[i][0]-1] != x[edges[i][1]-1]){
            score += edges[i][2];
        }
    }
    return score;
}

bitset<500> cross_over(bitset<500> x1, bitset<500> x2, int n, int m){
    int i, j, idx;
    bitset<500> mask;
    bitset<500> offspring;

    for (j=0; j<m; j++){
        idx = rand() % n;
        for (i=0; i<idx; i++){
            mask[i].flip();
        }
        offspring = (mask & x1) | (mask.flip() & x2);
    }

    return offspring;
}

bitset<500> u_cross_over(bitset<500> x1, bitset<500> x2, int n){
    int i, sel;
    bitset<500> offspring, mask;
    for (i=0; i<n; i++){
        sel = rand() % 2;
        if (sel == 1){
            offspring[i] = x1[i];
        } else{
            offspring[i] = x2[i];
        }
    }
    return offspring;
}

bitset<500> mutation(bitset<500> x, int n, float prob){
    int i;
    for (i=0; i<n; i++){
        if (rand() % 100 < prob*100){
            x.flip(i);
        }
    }
    return x;
}

bitset<500> irr_mutation(bitset<500> x, int n, float prob, int it){
    int i;
    for (i=0; i<n; i++){
        if (rand() % 100 < prob*100*(0.98)^it){
            x.flip(i);
        }
    }
    return x;
}

int main() {
    srand((unsigned)time(NULL));

    time_t start, finish;
    double duration;
    start = time(NULL);

    string file_path = "./maxcut.in";
    ifstream fin, sin;

    fin.open(file_path);

    int n, e, P=6000, k, iter=1000000, i;
    k = (int)(P/5*3);
    char *ch;
    string st;

    fin >> n;
    fin >> e;

    int edges[5000][3];
    for (i=0; i<e; i++){
        fin >> edges[i][0];
        fin >> edges[i][1];
        fin >> edges[i][2];
    }

    fin.close();

    vector<bitset<500>> x_s = generate_population(P, n);
    int score;
    int max_score=0;
    vector<int> scores_iter;
    int it;

    for (it=0; it<iter; it++){
        vector<int> scores;
        for (i=0; i<P; i++){
            score = compute_score(x_s[i], edges, n, e);
            scores.push_back(score);
        }
        max_score = *max_element(scores.begin(), scores.end());
        scores_iter.push_back(max_score);
        finish  = time(NULL);
        duration = (double)(finish - start);

        if (duration > 178){
            ofstream fout, sout;
            fout.open("./maxcut.out");

            max_score = *max_element(scores.begin(), scores.end());

            int max_index = max_element(scores.begin(), scores.end()) - scores.begin();
            bool temp;
            for (i=0; i<n; i++){
                temp = x_s[max_index][i];
                if (temp == 1) {
                    fout << i+1 << " " ;
                }
            }
            fout.close();
            break;
        }

        vector<size_t> idx(scores.size());
        iota(idx.begin(), idx.end(), 0);
        stable_sort(idx.begin(), idx.end(),
                    [&scores](size_t i1, size_t i2) {return scores[i1] > scores[i2];});

        vector<int> sample_idx;
        for (int j=0; j<k; j++){
            if (rand() % (int)P/k == 1){
                sample_idx.push_back(idx[j]);
            }
        }

        int idx1, idx2;
        bitset<500> offspring;
        for (i=0; i<k; i++){
            idx1 = rand() % k;
            idx2 = rand() % k;
            offspring = u_cross_over(x_s[idx1], x_s[idx2], n);
            x_s[idx[P-1-i]] = offspring;
        }
    }
    return 0;
}