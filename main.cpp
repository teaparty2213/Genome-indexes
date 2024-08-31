#include <iostream>
#include <vector>
#include "SA-LS.h"

int main() {
    std::vector<char> alphabet = {'$', 'A', 'C', 'G', 'T'}; // 用途としてゲノム配列解析を想定している
    std::string X = "ATAATACGATAATAA$"; // 最後の一文字だけ'$'にする
    int n = X.length();
    std::vector<int> V(n, 0); // inverse suffix array
    std::vector<int> I(n, 0); // 最終的なoutputでもある

    SALS(X, V, I, n, alphabet);

    return 0;
}