#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <bitset>
#include <numeric>
#include <limits>

using namespace std;

vector<bitset<5000>> generate_population(int P, int n){
    vector<bitset<5000>> x_s;
    bitset<5000> x;
    for (int i=0; i<P; i++){
        for (int j=0; j<n; j++){
            x[j] = rand() % 2;
        }
        x_s.push_back(x);
    }
    return x_s;
}

int compute_score(bitset<5000> x, int edges[40000][3], int n, int e){
    int i;
    int score = 0;
    for (i=0; i<e; i++){
        if(x[edges[i][0]-1] != x[edges[i][1]-1]){
            score += edges[i][2];
        }
    }
    return score;
}
bitset<5000> cross_over(bitset<5000> x1, bitset<5000> x2, int n, int m){
    int i, j, idx;
    bitset<5000> mask;
    bitset<5000> offspring;
    for (j=0; j<m; j++){
        idx = rand() % n;
        for (i=0; i<idx; i++){
            mask[i].flip();
        }
        offspring = (mask & x1) | (mask.flip() & x2);
    }
    return offspring;
}

bitset<5000> u_cross_over(bitset<5000> x1, bitset<5000> x2, int n){
    int i, sel;
    bitset<5000> offspring, mask;
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

bitset<5000> mutation(bitset<5000> x, int n, float prob){
    int i;
    for (i=0; i<n; i++){
        if (rand() % 100 < prob*100){
            x.flip(i);
        }
    }
    return x;
}

bitset<5000> irr_mutation(bitset<5000> x, int n, float prob, int it){
    int i;
    for (i=0; i<n; i++){
        if (rand() % 100 < prob*100*(0.98)^it){
            x.flip(i);
        }
    }
    return x;
}

bitset<5000> max_LG(bitset<5000> x, int edges[40000][3], int n, int e){
    int i;
    int max_idx, early_stop=1000, cnt=0;
    int prev_score=compute_score(x, edges, n, e);
    int new_score=0;

    vector<int> scores;
    for (i=0; i<n; i++){
        x.flip(i);
        scores.push_back(compute_score(x, edges, n, e));
        x.flip(i);
    }

    while(true){
        for (i=0; i<n; i++){
            if(scores[i] != 0){
                x.flip(i);
                scores[i] = compute_score(x, edges, n, e);
                x.flip(i);
            }
        }
        max_idx = max_element(scores.begin(), scores.end()) - scores.begin();
        new_score = scores[max_idx];
        if (prev_score > new_score || cnt > early_stop){
            break;
        } else {
            x.flip(max_idx);
            scores[max_idx] = 0;
            prev_score = new_score;
        }
        cnt++;
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
    int n, e, P_old=2000, k, iter=1000000, i, conv=50;
    int Ps[3];

    Ps[0] = 1000;
    Ps[1] = 1000;
    Ps[2] = 1000;

    string st;
    fin >> n;
    fin >> e;

    int edges[40000][3];
    for (i=0; i<e; i++){
        fin >> edges[i][0];
        fin >> edges[i][1];
        fin >> edges[i][2];
    }

    fin.close();

    vector<vector<bitset<5000>>> pops;

    int score;
    vector<int> max_scores, max_indexes;
    int flag[3];
    int track[3];

    for (i=0; i<3; i++){
        flag[i] = 0;
        track[i] = i;
        max_scores.push_back(-numeric_limits<int>::max());
        max_indexes.push_back(-numeric_limits<int>::max());
        pops.push_back(generate_population(Ps[i], n));
    }

    vector<vector<int>> scores_iter;
    vector<int> max_scores_iter;

    int it;
    for (it=0; it<iter; it++){
        if (duration > n/6 - n/500){
            ofstream fout, sout;

            fout.open("./maxcut.out");
            bool temp;
            int idx = max_element(max_scores.begin(), max_scores.end()) - max_scores.begin();
            int max_index =  max_indexes[idx];
            vector<bitset<5000>> x_s = pops[idx];

            x_s[max_index] = max_LG(x_s[max_index], edges, n, e);
//            cout << "Final score: " << compute_score(x_s[max_index], edges, n, e) << endl;

            for (i=0; i<n; i++){
                temp = x_s[max_index][i];
                if (temp == 1) {
                    fout << i+1 << " " ;
                }
            }
            fout.close();

            vector<int> scores_iter_integ;
            for(i=0; i<it; i++){
                scores_iter_integ.push_back(*max_element(scores_iter[i].begin(), scores_iter[i].end()));
            }

//            for(i=0; i<it; i++){
//                cout << scores_iter_integ[i] << ", ";
//            }

            break;
        }
        for (int l=0; l<3; l++){
            if(track[l] == -1){
                continue;
            }

            int P = Ps[l];
            int max_score;
            vector<bitset<5000>> x_s = pops[l];
            vector<int> scores;

            for (i=0; i<P; i++){
                score = compute_score(x_s[i], edges, n, e);
                scores.push_back(score);
            }

            max_score = *max_element(scores.begin(), scores.end());
            max_scores_iter.push_back(max_score);

            finish  = time(NULL);
            duration = (double)(finish - start);
            vector<size_t> idx(scores.size());
            iota(idx.begin(), idx.end(), 0);
            stable_sort(idx.begin(), idx.end(),
                        [&scores](size_t i1, size_t i2) {return scores[i1] > scores[i2];});

            int idx1, idx2;
            bitset<5000> offspring = x_s[idx[0]];
            k = (int)(Ps[l]/5*3);

            for (i=0; i<k; i++){
                idx1 = rand() % k;
                idx2 = rand() % k;
                int idx3 = rand() % (n-k);

                if (l == 0){
                    offspring = u_cross_over(x_s[idx1], x_s[idx2], n);
                }
                if (l == 1){
                    offspring = cross_over(x_s[idx1], x_s[idx2], n, 10);
                }
                if (l == 2){
                    offspring = cross_over(x_s[idx1], x_s[idx2], n, 20);
                }

                // random mutation and irregular mutation for population number 2 and 1, resp.
                // if (l == 1){
                //     offspring = irr_mutation(offspring, n, 0.01, it);
                // }
                // if (l == 2){
                //     offspring = mutation(offspring, n, 0.01);
                // }

//                offspring = max_LG(offspring, edges, n, e);

                x_s[idx[P-1-i]] = offspring;
//                x_s[idx[k + idx3 -1]] = offspring;
            }

            if (max_score <= max_scores[l]){
                flag[l]++;
            } else{
                max_scores[l] = max_score;
                max_indexes[l] = max_element(scores.begin(), scores.end()) - scores.begin();
                flag[l] = 0;
            }
            pops[l] = x_s;
        }

        scores_iter.push_back(max_scores_iter);

        for (i=0; i<3; i++){
            int cnt = 0;
            for (int j=0; j<3; j++){
                if (track[j] == -1){
                    cnt += 1;
                }
            }
            if (cnt < 2) {
                if (flag[i] > conv) {
                    int target = 0;

                    if(track[(i + 1) % 3] != -1){
                        target = (i + 1) % 3;
                    } else target = ((i+1) % 3 + 1) % 3;

                    pops[target].insert(pops[target].end(), pops[i].begin(), pops[i].end());
                    track[i] = -1;
                    flag[i] = -1;
//                    cout << "Population " << i << " to population " << target << endl;
                    Ps[target] = Ps[i] + Ps[target];
                }
            }
        }

//        if(it%10 == 0){
//            for (i=0; i<3; i++){
//                cout << max_scores[i] << " ";
//            }
//            cout << endl;
//        }
    }
    return 0;
}
