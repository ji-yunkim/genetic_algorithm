#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <limits.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <set>
#include <vector>
#include <ctime>
#include <bitset>
#include <algorithm>
#include <random>
#include <numeric>
#include <bits/stdc++.h>
using namespace std;
using namespace chrono;

int INI_POP = 10000;
int TOP_K = 500;
int HOWMANY_CO = 500;
int GENERATION = 1;

int final_s = 0;

class timer: high_resolution_clock {
    const time_point start_time;

public:
    timer(): start_time(now()) {}
    rep elapsed_time() const { return duration_cast<milliseconds>(now()-start_time).count(); } };

timer t;
vector<bitset<640>> randGen(int INI_POP, int n){
    srand(time(NULL));
    vector<bitset<640>> pop;
    for (int i = 0; i < INI_POP; i++){
        bitset<640> oneLine;
        for (int j = 0; j < 8*n; j++){
            int num = ((int)rand() % 2);
            oneLine[j] = num;
        }
        pop.push_back(oneLine);
    }
    return pop;
}

int chromosomeEvaluate(bitset<640> chromosome, bitset<640> answer, int n){
    int i = 1;
    size_t range = n;
    bool match = true;
    int totalScore = 0;
    for (i; i<=range; i++){
        if (chromosome[(i*8-8)+8] == answer[(i*8-8)+8]){
            if (i % 2 == 0) {
                totalScore += 5;
            }
            else {
                totalScore += 8;
            }
        }
        //cout << totalScore << endl;
    }
    return totalScore;
}

bitset<640> uniform_CO(bitset<640> s1, bitset<640> s2, int n){
    //cout << s1 << " ";
    //cout << s2 << " ";
        bitset<640> newChromosome;
        for (int q = 0; q < 8*n; q++){
            if (rand() % 2 == 1) {
                newChromosome[q] = s1[q];
            }
            else {
                newChromosome[q] = s2[q];
            }
        }
        //cout << newChromosome << "\n";
    return newChromosome;
}

int main(){
    // file read
    fstream fin("rr.in");
    string buffer;
    getline(fin, buffer);
    fin.close();

    // splitting and converting
    istringstream iss(buffer);
    string buffer2;
    vector<string> result;
    while (getline(iss, buffer2, ' ')) {
        result.push_back(buffer2);
    }
    int n = stoi(result.at(0));
    bitset<640> y(result.at(1));

    // population initialization
    vector<bitset<640>> population = randGen(INI_POP, n);
    
    int epoch = 1;
   // every generation
      //for (int epoch = 1; epoch <= GENERATION; epoch ++){
      while (t.elapsed_time() < 28000){
       vector<int> score_array;
       for (int each = 1; each <= population.size(); each++){
            int score = chromosomeEvaluate(population[each], y, n);
            score_array.push_back(score);
        }

        vector<int> index_array(population.size());
        iota(index_array.begin(), index_array.end(), 0);
        stable_sort(index_array.begin(), index_array.end(), [&score_array](size_t a, size_t b){ return score_array[a] > score_array[b];}); 

        //int half = round(population.size()/2);
        for (int j = 0; j < HOWMANY_CO; j++){ // select parents
            int firstParent = rand()%TOP_K; // 몇 번째를 가져오느냐
            int secondParent = rand()%TOP_K;
            //cout << firstParent << " ";
            //cout << secondParent <<  " ";
            bitset<640> newChromosome= uniform_CO(population[index_array[firstParent]],population[index_array[secondParent]], n);
            int true_size = index_array[population.size()-j-1];
            population[true_size] = newChromosome; // crossover
        }

        /*
        if (epoch%10 == 0){
            int res = score_array[index_array[0]];
            printf(" %u", res);
        }*/

        //cout << epoch << " ";
        epoch += 1;

        if (final_s <= score_array[index_array[0]]){
            final_s = score_array[index_array[0]];
            ofstream fout;
            fout.open("rr.out");
            bitset<640> answer = population.at(index_array[0]); 
            string finalAnswer;
            for (int i = 0; i < 8*n; i++){
                finalAnswer += to_string(answer[i]);
            }
            string str=to_string(score_array[index_array[0]])+" "+finalAnswer;
            fout.write(str.c_str(),str.size());
            fout.close();
        }
   }
    return 0;
}