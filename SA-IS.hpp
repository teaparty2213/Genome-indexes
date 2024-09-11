//-------------------------------------------------------------
// Reference: G. Nong, S. Zhang and W. H. Chan, "Linear Suffix Array Construction by Almost Pure Induced-Sorting," 2009 Data Compression Conference, Snowbird, UT, USA, 2009, pp. 193-202,
// makeNewS()の作り方は https://gasin.hatenadiary.jp/entry/2017/06/08/101525 を参照
// To compile, perform: g++ -std=c++20 -Wall --pedantic-errors -O3 -o SA-IS SA-IS.hpp
//-------------------------------------------------------------
#pragma once
#include <iostream>
#include <vector>
#include <algorithm>

void inducedSort(const std::vector<int> & S, const std::vector<int> & t, std::vector<int> & SA, const std::vector<int> & LMS, std::vector<int> & L, std::vector<int> & R, const int n, const int m) {
    std::vector<int> R_copy = R; // Rは後で使うのでdeep copyしてから使う
    for (int i = LMS.size() - 1; i >= 0; i--) { // LMSを各bucketの後ろから大きい順に入れる
        int lms = LMS[i]; // SAに追加するLMS
        int p = R_copy[S[lms]]; // LMSを追加する位置
        SA[p] = lms; R_copy[S[lms]]--;
    }
    for (int i = 0; i < n; i++) { // L-typeをbucketの左端から入れる
        if (SA[i] == -1) continue;
        int idx = SA[i] - 1; // SAに追加するL-typeのindex
        if (t[idx] == 1 && SA[i] > 0) {
            int p = L[S[idx]]; // L-typeを追加する位置
            SA[p] = idx; L[S[idx]]++;
        }
    }
    for (int i = n - 1; i >= 0; i--) { // S-typeをbucketの右端から入れる
        if (SA[i] == -1) continue;
        int idx = SA[i] - 1; // SAに追加するS-typeのindex
        if (t[idx] == 0 && SA[i] > 0) {
            int p = R[S[idx]]; // S-typeを追加する位置
            SA[p] = idx; R[S[idx]]--;
        }
    }
}

std::vector<int> makeNewS(const std::vector<int> & S, const std::vector<int> & SA, std::vector<int> & P1, bool & unique, const int n) {
    int counter = -1; // LMS部分列の順位
    std::vector<int> pre, now; // 1つ前のLMS部分列と今見ているLMS部分列
    for (int i = 0; i < n; i++) {
        if (P1[SA[i]] != -1) { // SA[i]がLMSのとき
            P1[SA[i]] = ++counter;
            now.clear();
            int start = SA[i]; int end = SA[i];// nowの開始・終了位置
            for (int j = start; j < n; j++) {
                if (j != start && P1[j] != -1) { end = j; break; }
            }
            now.resize(end - start + 1);
            std::copy(S.begin() + start, S.begin() + end + 1, now.begin());
            if (pre == now) { P1[SA[i]] = --counter; unique = false;} // 同じLMS部分列が存在
            pre = now;
        }
    }
    std::vector<int> S1;
    for (int i = 0; i < n; i++) {
        if (P1[i] != -1) S1.push_back(P1[i]);
    }
    return S1;
}

std::vector<int> SAIS(const std::vector<int> & S, const int n, const int max) {
    int m = max + 1; // bucketの個数
    std::vector<int> t(n, 0); // 0はS-type, 1はL-type
    std::vector<int> sum(m, 0); sum[0] = 1; // 各アルファベットの個数
    std::vector<int> P1(n, -1); // S[i]がLMSならばP1[i]!=-1．LMSのチェックを定数時間で行うために使用
    std::vector<int> LMS; // LMSの順序を保存．inducedSortで使う
    for (int i = n - 2; i >= 0; i--) { // 論文のFigure1における(1) SをスキャンしてL-typeとS-typeをtに入れる
        if (S[i] == S[i + 1]) t[i] = t[i + 1];
        else if (S[i] > S[i + 1]) t[i] = 1;
        sum[S[i]]++; // 各アルファベットの個数を数える
        if (t[i] == 1 && t[i + 1] == 0) { LMS.push_back(i + 1); P1[i + 1] = 0; }// (2) tをスキャンしてLMSを見つける
    }
    std::vector<int> L(m, 0); std::vector<int> R(m, 0); // 各bucketの左端と右端のindex
    for (int i = 0; i < m; i++) {
        if (i < m - 1) { L[i + 1] = L[i] + sum[i]; }
        if (i > 0) { R[i] = R[i - 1] + sum[i]; }
    }
    std::vector<int> SA(n, -1);
    std::vector<int> L_copy = L; std::vector<int> R_copy = R; // LとRは後で使うのでdeep copyしたものをinducedSortで使う
    inducedSort(S, t, SA, LMS, L_copy, R_copy, n, m); // (3) LMS部分列のinduced sort
    bool unique = true; // S1の各文字がuniqueか判定する用
    std::vector<int> S1 = makeNewS(S, SA, P1, unique, n); // (4) Sの各LMS部分列に名前(順番)をつけて新たな短い整数列を作る
    int new_n = S1.size();
    int new_max = *std::max_element(S1.begin(), S1.end());
    if (unique) { // (5)~(9) 正しい順序でLMSを作り直す，もしくは再帰でLMSの順序を決める
        LMS.clear(); LMS.resize(new_max + 1); 
        for (int i = 0; i < n; i++) {
            if (P1[i] != -1) { LMS[P1[i]] = i; } // (7) S1から直接正しいLMSを計算
        }
    }
    else {
        std::vector<int> rec = SAIS(S1, new_n, new_max); // (9) 再帰でLMSに再び順位をつける
        std::vector<int> asLMS; // LMSをインデックスの昇順に入れる
        for (int i = 1; i < n; i++) {
            if (t[i] == 0 && t[i - 1] == 1) asLMS.push_back(i);
        }
        LMS.clear();
        for (int i = 0; i < rec.size(); i++) { LMS.push_back(asLMS[rec[i]]); }
    }
    for (int i = 0; i < n; i++) { SA[i] = -1; } // SAを-1で再び初期化
    inducedSort(S, t, SA, LMS, L, R, n, m); // (10) 正しいLMSを使ってSAをinducedSortする
    return SA;
}