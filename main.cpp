//-------------------------------------------------------------
// To compile, perform: 
// g++ -std=c++20 -Wall --pedantic-errors -O3 -o SA-LS SA-LS.hpp
// g++ -std=c++20 -Wall --pedantic-errors -O3 -o SA-IS SA-IS.hpp
// g++ -std=c++20 -Wall --pedantic-errors -O3 -o main main.cpp
//-------------------------------------------------------------
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include "SA-LS.hpp"
#include "SA-IS.hpp"

void strGen(std::string & X, const int n, const std::vector<char> & alphabet, std::mt19937 & rng) { // ランダムに長さnの文字列を生成
    std::uniform_int_distribution<int> d(1, alphabet.size() - 1);
    for (int i = 0; i < n; i++) {
        if (i < n - 1) X.push_back(alphabet[d(rng)]);
        else X.push_back(alphabet[0]); // 最後の1文字だけ'$'になる
    }
}

void StoI(const std::string & X, std::vector<int> & Y, const int n, const std::vector<char> & alphabet) { // 解析する文字列を整数の列に変換
    int m = alphabet.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (X[i] == alphabet[j]) Y[i] = j;
        }
    }
}

int main() {
    std::vector<char> alphabet = {'$', 'A', 'C', 'G', 'T'}; // 用途としてゲノム配列解析を想定している
    std::string X; // Suffix Arrayを作る文字列
    int n = 0; // 文字列の長さ
    std::mt19937 rng; // 乱数生成器
    std::random_device seed_gen;
    auto seed = seed_gen();
    rng.seed(seed);

    const int mode = 1; // 0,1,2で指定(0: debug, 1: 時間計測, 2: ファイルを読んでSuffix Array構築)
    const int algo = 1; // 0,1で指定(0: Larsson-Sadakane, 1: Induced Sorting)

    if (mode == 0) { // 自分で決めた文字列に対してsuffix arrayを作る
        X = "ATAATACGATAATAA$";
        n = X.length();
        if (algo == 0) {
            std::vector<int> V(n, 0); // inverse suffix array．groupを表す
            std::vector<int> I(n, 0); // 最終的なoutput．論文のrefinementに合わせてLの役割も兼ねている
            SALS(X, V, I, n, alphabet, rng);
        }
        else if (algo == 1) {
            int max = alphabet.size() - 1;
            std::vector<int> Y(n, 0); // Xを0~maxの整数列に変換したもの
            StoI(X, Y, n, alphabet);
            std::vector<int> SA = SAIS(Y, n, max);
        }
    }

    else if (mode == 1) {
        std::vector<int> testLen = {100, 1000, 10000, 100000, 1000000, 10000000};
        for (auto L : testLen) {
            strGen(X, L, alphabet, rng);
            n = X.length();
            if (algo == 0) {
                std::vector<int> V(n, 0); // inverse suffix array．groupを表す
                std::vector<int> I(n, 0); // 最終的なoutput．論文のrefinementに合わせてLの役割も兼ねている

                auto start = clock();
                SALS(X, V, I, n, alphabet, rng);
                auto end = clock();
                auto CPU_time = double(end - start) / CLOCKS_PER_SEC;
                std::cout << L << ": " << CPU_time << "\n";
            }
            else if (algo == 1) {
                int max = alphabet.size() - 1;
                std::vector<int> Y(n, 0); // Xを0~maxの整数列に変換したもの
                StoI(X, Y, n, alphabet);

                auto start = clock();
                std::vector<int> SA = SAIS(Y, n, max);
                auto end = clock();
                auto CPU_time = double(end - start) / CLOCKS_PER_SEC;
                std::cout << L << ": " << CPU_time << "\n";
            }
        }
    }
    return 0;
}