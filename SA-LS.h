//-------------------------------------------------------------
// Reference: N. Jesper Larsson and Kunihiko Sadakane. "Faster suffix sorting” Theoretical Computer Science, 387 (2007): 258-272.
// To compile, perform: g++ -std=c++20 -Wall --pedantic-errors -o SA-LS SA-LS.cpp
//-------------------------------------------------------------
#pragma once
#include <iostream>
#include <vector>

void init_I(std::string X, std::vector<int> I, int n, std::vector<char> alphabet) {
    int m = alphabet.size();
    std::vector<int> sum(m, 0); // alphabetの各文字の個数をカウント
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (X[i] == alphabet[j]) sum[j]++;
        }
    }
    for (int i = m - 1; i >= 0; i--) { // sumが各文字のIでの開始位置を表すように変換
        if (i == m - 1) sum[i] = n - sum[i];
        else sum[i] = sum[i + 1] - sum[i];
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (X[i] == alphabet[j]) { I[sum[j]] = i; sum[j]++; }
        }
    }
}

void SALS(std::string X, std::vector<int> V, std::vector<int> I, int n, std::vector<char> alphabet) {
    init_I(X, I, n, alphabet);
}
