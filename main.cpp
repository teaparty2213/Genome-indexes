//-------------------------------------------------------------
// To compile, perform: 
// g++ -std=c++20 -Wall --pedantic-errors -o SA-LS SA-LS.hpp
// g++ -std=c++20 -Wall --pedantic-errors -o main main.cpp
//-------------------------------------------------------------
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include "SA-LS.hpp"

void strGen(std::string & X, const int n, const std::vector<char> & alphabet, std::mt19937 & rng) { // ランダムに長さnの文字列を生成
    std::uniform_int_distribution<int> d(1, alphabet.size() - 1);
    for (int i = 0; i < n; i++) {
        if (i < n - 1) X.push_back(alphabet[d(rng)]);
        else X.push_back(alphabet[0]); // 最後の1文字だけ'$'になる
    }
}

int main() {
    std::mt19937 rng; // 乱数生成器
    std::random_device seed_gen;
    auto seed = seed_gen();
    rng.seed(seed);

    std::vector<char> alphabet = {'$', 'A', 'C', 'G', 'T'}; // 用途としてゲノム配列解析を想定している
    std::vector<int> testLen = {100, 1000, 10000, 100000, 1000000, 10000000};
    for (auto L : testLen) {
        std::string X;
        strGen(X, L, alphabet, rng);
        int n = X.length();
        std::vector<int> V(n, 0); // inverse suffix array．groupを表す
        std::vector<int> I(n, 0); // 最終的なoutput．論文のrefinementに合わせてLの役割も兼ねている

        auto start = clock();
        SALS(X, V, I, n, alphabet, rng);
        auto end = clock();
        auto CPU_time = double(end - start) / CLOCKS_PER_SEC;
        std::cout << L << ": " << CPU_time << "\n";
    }
    return 0;
}