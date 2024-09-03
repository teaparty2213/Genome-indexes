#include <iostream>
#include <vector>
#include <random>
#include "SA-LS.h"

int main() {
    std::vector<char> alphabet = {'$', 'A', 'C', 'G', 'T'}; // 用途としてゲノム配列解析を想定している
    std::string X = "ATAATACGATAATAA$"; // 最後の一文字だけ'$'にする
    int n = X.length();
    std::vector<int> V(n, 0); // inverse suffix array
    std::vector<int> I(n, 0); // 最終的なoutputでもある

    std::mt19937 rng;
    std::random_device seed_gen;
    auto seed = seed_gen();
    rng.seed(seed);

    SALS(X, V, I, n, alphabet, rng);

    return 0;
}