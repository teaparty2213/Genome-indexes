//-------------------------------------------------------------
// Reference: N. Jesper Larsson and Kunihiko Sadakane. "Faster suffix sorting” Theoretical Computer Science, 387 (2007): 258-272.
// To compile, perform: g++ -std=c++20 -Wall --pedantic-errors -O3 -o SA-LS SA-LS.hpp
//-------------------------------------------------------------
#pragma once
#include <iostream>
#include <vector>
#include <random>

void update_VandI(const std::string & X, std::vector<int> & V, std::vector<int> & I, const int n, int & f, const int h, const int i, const int j) { // インデックスk=i~jの範囲でV[I[k]]とI[k]を更新する
    int key = j;
    for (int k = key; k >= i; k--) {
        if (h == 0 && X[abs(I[k]) + h] != X[abs(I[key]) + h]) key = k; // h=0のときは異なる文字が来たらkeyを更新
        if (h > 0 && V[abs(I[k]) + h] != V[abs(I[key]) + h]) key = k; // h>0のときはXでなくVを見る
        V[abs(I[k])] = key; // V[I[k]]をupdate
    }
    for (int k = j; k >= i; k--) { // I[k]がsorted groupなら-1倍してf(combined sorted groupの要素数)を1増やす．debugの際はI[i]をabs()して使うことに注意
        if (k == n - 1 && V[abs(I[k])] != V[abs(I[k - 1])]) { I[k] *= -1; f++; } // 配列Iの範囲外参照を避けるために追加
        else if (V[abs(I[k])] != V[abs(I[k + 1])] && V[abs(I[k])] != V[abs(I[k - 1])]) { I[k] *= -1; f++; }
    }
}

void init_VandI(const std::string & X, std::vector<int> & V, std::vector<int> & I, const int n, const std::vector<char> alphabet, int & f, const int h) {
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
            if (X[i] == alphabet[j]) { I[sum[j]] = i; sum[j]++; } // step(1) Iを初期化
        }
    }
    update_VandI(X, V, I, n, f, h, 0, n - 1); // step(2) Vを初期化．step(3) Lを初期化．ここでは論文のrefinementに合わせて，Lを使わずにIを変形することでgroupの長さを示す
}

void tsQsort(std::vector<int> & V, std::vector<int> & I, const int h, const int i, const int j, std::mt19937 & rng) {
    if (i == j) return;
    std::uniform_int_distribution<> dist(i, j); // i以上j以下の整数を一様に生成する一様分布
    int pivot = V[abs(I[dist(rng)]) + h];
    std::vector<int> S; // elements smaller than pivot
    std::vector<int> E; // elements equals to pivot
    std::vector<int> L; // elements larger than pivot
    for (int k = i; k <= j; k++) { // pivotとの大小関係でI[k]を3通りの分類
        if (V[abs(I[k]) + h] < pivot) S.push_back(I[k]);
        else if (V[abs(I[k]) + h] > pivot) L.push_back(I[k]);
        else E.push_back(I[k]);
    }
    int l = S.size(); int m = E.size(); int r = L.size();
    for (int k = i; k <= j; k++) { // I[i~j]を更新
        if (k < i + l) I[k] = S[k - i]; // Sの要素をIに入れる
        else if (i + l <= k && k < i + l + m) I[k] = E[k - i - l]; // Eの要素をIに入れる
        else I[k] = L[k - i - l - m]; // Lの要素をIに入れる
    }
    if (l > 0) tsQsort(V, I, h, i, i + l - 1, rng); // Sだった要素を再帰的にソート
    if (r > 0) tsQsort(V, I, h, j - r + 1, j, rng); // Lだった要素を再帰的にソート
}

void SALS(const std::string & X, std::vector<int> & V, std::vector<int> & I, const int n, const std::vector<char> alphabet, std::mt19937 & rng) {
    int f = 0; // combined sorted groupの要素数．論文ではI[0]にその情報を格納していたが，ここでは変数1つを特別に使用する
    int h = 0; // h-order
    init_VandI(X, V, I, n, alphabet, f, h); // 論文のFig.1におけるstep(1)~(3)
    h = 1; // 1-order
    while (f < n && h <= n) {
        int i = 0; // iはunsorted groupの左端を表す
        while (i < n) {
            if (I[i] < 0) {i++; continue;}
            int j = i; // jはunsorted groupの右端を表す
            while (j + 1 < n && V[abs(I[j + 1])] == V[abs(I[i])]) j++;
            tsQsort(V, I, h, i, j, rng); // step(4) インデックスk=i~jの範囲でV[|I[k]|+h]をキーとしたI[k]のternary-split quicksort
            update_VandI(X, V, I, n, f, h, i, j); //step(5,6) ソートされたI[k]を使ってV[I[k]]とI[k]を更新
            i = j + 1; // 次のgroupのためにiを更新
        }
        h *= 2;
    }
    for (int i = 0; i < n; i++) {
        I[i] *= -1; // Iの要素は全て負なので-1倍して元に戻す
    }
}