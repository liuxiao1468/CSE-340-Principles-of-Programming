/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <bits/stdc++.h>

using namespace std;
LexicalAnalyzer lexer;

std::map<string, int> symbol_map;


vector <string> symbols;

struct rule{
    int LHS{};
    vector<int> RHS;
    int RHS_size{};
    struct rule * next{};
};
struct rule* st = new rule;
struct rule* start = st;

struct first{
    int symbol;
    vector<int> first_set;
    struct first * next;
};
struct first* find_first = new first;
struct first* first_start = find_first;

struct follow{
    int symbol;
    vector<int> follow_set;
    struct follow * next;
};
struct follow* find_follow = new follow;
struct follow* follow_start = find_follow;

std::map<int, first*> first_table;
std::map<int, follow*> follow_table;
vector <int> terminal;
vector <int> non_terminal;
vector <int> v_LHS;
vector <int> v_RHS;
vector <int> v_all;
vector <int> gen_set;
vector <int> reach_set;
vector <rule> p_rule;

void syntax_error()
{
    cout << "SYNTAX ERROR\n";
    exit(1);
}

// read grammar
void ReadGrammar()
{
    symbol_map.insert(make_pair("#", 0));
    symbol_map.insert(make_pair("$", 1));
    symbols.emplace_back("#");
    symbols.emplace_back("$");
    int i=2;
    Token t = lexer.GetToken();
    while(t.token_type != DOUBLEHASH)
    {
        Token t1 = lexer.GetToken();
        if (t1.token_type == ARROW)
        {
            if(symbol_map.count(t.lexeme) == 0)
            {
                symbol_map.insert(make_pair(t.lexeme, i));
                i++;
                st->LHS =symbol_map[t.lexeme];
                symbols.push_back(t.lexeme);
//                cout << "LHS" << t.lexeme << endl;
            }
            else{
                st->LHS = symbol_map[t.lexeme];
//                cout << "LHS" << t.lexeme << endl;
            }
            Token t2 = lexer.GetToken();
            if (t2.token_type == HASH)
            {
                st->RHS.push_back(symbol_map["#"]);
//                cout << "RHS" << "#" << endl;

                st->RHS_size = st->RHS.size();
                st->next = new rule;
                st = st-> next;
            }
            else
            {
                while(t2.token_type!=HASH)
                {
                    if (symbol_map.count(t2.lexeme) == 0) {
                        symbol_map.insert(make_pair(t2.lexeme, i));
                        i++;
                        st->RHS.push_back(symbol_map[t2.lexeme]);
                        symbols.push_back(t2.lexeme);
//                        cout << "RHS" << t2.lexeme << endl;
                    }
                    else {
                        st->RHS.push_back(symbol_map[t2.lexeme]);
//                        cout << "RHS" << t2.lexeme << endl;
                    }
                    t2 = lexer.GetToken();
                }
                st->RHS_size = st->RHS.size();
                st->next = new rule;
                st = st-> next;
            }
        }
        t = lexer.GetToken();
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct rule* start_2 = start;
    while (start_2!=nullptr)
    {
        if (start_2->RHS_size!=0)
        {
            if (count(v_LHS.begin(), v_LHS.end(), start_2->LHS)==0)
            {
                v_LHS.push_back(start_2->LHS);
            }
            if (count(v_all.begin(), v_all.end(), start_2->LHS)==0)
            {
                v_all.push_back(start_2->LHS);
            }
            for (int i = 0; i<start_2->RHS_size; i++)
            {
                if (count(v_RHS.begin(), v_RHS.end(), start_2->RHS.at(i))==0)
                {
                    v_RHS.push_back(start_2->RHS.at(i));
                }
                if (count(v_all.begin(), v_all.end(), start_2->RHS.at(i))==0)
                {
                    v_all.push_back(start_2->RHS.at(i));
                }
            }
        }
        start_2 = start_2->next;
    }

    for (int i : v_LHS) //remove LHS from RHS to get all terminals
    {
        v_RHS.erase(remove(v_RHS.begin(), v_RHS.end(), i), v_RHS.end());
    }

    for (int i : v_RHS) //remove all terminals (right now in RHS) from all, get all none terminals
    {
        v_all.erase(remove(v_all.begin(), v_all.end(), i), v_all.end());
    }

    terminal.insert(terminal.end(), v_RHS.begin(), v_RHS.end());
    non_terminal.insert(non_terminal.end(), v_all.begin(), v_all.end());

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    for (int i=0; i<symbols.size();i++)
    {
        gen_set.push_back(0);
    }
    gen_set[0] = 1; // "#" is always true
    for (int i : terminal)
    {
        gen_set[i] = 1;
    }

    bool change=true;
    while(change) // find the gernerating set
    {
        vector <int> temp_set;
        temp_set.insert(temp_set.end(), gen_set.begin(), gen_set.end());
        struct rule* start_3 = start;
        while (start_3!=nullptr)
        {
            if (start_3->RHS_size!=0) {
                if (gen_set[start_3->LHS] != 1) {
                    //remove useless symbol
                    int temp = 0;
                    for (int i = 0; i < start_3->RHS_size; i++) {
                        if (gen_set.at(start_3->RHS.at(i)) == 1) {
                            temp++;
                        }
                    }
                    if (temp == start_3->RHS_size) {
                        gen_set[start_3->LHS] = 1;
                        change = true;
                    }
                }
            }
            start_3 = start_3->next;
        }
        if(gen_set==temp_set)
        {
            change = false;
        }
    }




    struct rule* start_3 = start;
    while (start_3!=nullptr) // save the rules based on generating set
    {
        if (start_3->RHS_size!=0) {
            if (gen_set[start_3->LHS]==1)
            {
                int temp = 0;
                for (int i = 0; i < start_3->RHS_size; i++) {
                    if (gen_set.at(start_3->RHS.at(i)) == 1) {
                        temp++;
                    }
                }
                if (temp == start_3->RHS_size){
                    p_rule.push_back(*start_3);
                }
            }
        }
        start_3 = start_3->next;
    }



    reach_set.reserve(symbols.size());
    for (int i=0; i<symbols.size();i++)
    {
        reach_set.push_back(0);
    }

    if (!p_rule.empty())
    {
        if (gen_set.at(non_terminal[0])==1)
        {
            reach_set.at(non_terminal[0])=1;
        }
//        reach_set.at(p_rule.front().LHS) = 1; // find the none reacheable set
        change = true;
        while (change)
        {
            vector <int> temp_reach_set;
            temp_reach_set.insert(temp_reach_set.end(), reach_set.begin(), reach_set.end());
            change = false;
            for(auto &r:p_rule)
            {
                if (reach_set[r.LHS]==1)
                {
                    for (int i=0;i<r.RHS_size;i++)
                    {
                        if (reach_set[r.RHS.at(i)]!=1)
                        {
                            reach_set[r.RHS.at(i)]=1;
                            change = true;
                        }
                    }
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

//    cout << "Hi here" << endl;
    for (int & i : terminal)
    {
        if (first_table.count(i) == 0) {
            first_table.insert(make_pair(i, find_first));
            find_first->symbol = i;
            find_first->first_set.push_back(i);
            find_first-> next = new first;
            find_first = find_first->next;
        }
    }


    for (int & i : non_terminal)
    {
        if (first_table.count(i)==0){
            first_table.insert(make_pair(i, find_first));
            find_first->symbol = i;
            find_first-> next = new first;
            find_first = find_first-> next;
        }
    }

//    cout<<"my Grammer"<< endl;
//    struct rule* start_4 = start;
//    while (start_4!=nullptr) {
//        if (start_4->RHS_size != 0) {
//            cout << symbols[start_4->LHS] << " ";
//            for (int i = 0; i < start_4->RHS_size; i++)
//                cout << symbols[start_4->RHS.at(i)];
//        }
//        cout << endl;
//        start_4=start_4->next;
//    }




    change = true;
    while (change){
        change = false;
        struct rule* start_4 = start;
        while (start_4!=nullptr)
        {
            if (start_4->RHS_size!=0) {
                // Apply Rule III
                // first_table[start_4->LHS]->first_set;
                // first_table[start_4->RHS[0]]->first_set.at(0);
                if (!first_table[start_4->RHS[0]]->first_set.empty())
                {
                    for (int i=0; i<first_table[start_4->RHS[0]]->first_set.size(); i++)
                    {
                        if (first_table[start_4->RHS[0]]->first_set.at(i)!=0 &&
                            count(first_table[start_4->LHS]->first_set.begin(), first_table[start_4->LHS]->first_set.end(),
                                  first_table[start_4->RHS[0]]->first_set.at(i))==0){
                            first_table[start_4->LHS]->first_set.push_back(first_table[start_4->RHS[0]]->first_set.at(i));
                            change = true;
                        }
                    }
                }
                // Apply Rule IV
                if (start_4->RHS_size >1)
                {
                    int empty=0;
                    for (int i=0; i<start_4->RHS_size;i++){
                        if(first_table[start_4->RHS[i]]->first_set.empty()){
                            empty=1;
                        }
                    }
                    if(empty==0){
                        int m=0;
                        for (int i=0; i<start_4->RHS_size; i++) {
                            if (count(first_table[start_4->RHS.at(i)]->first_set.begin(),
                                      first_table[start_4->RHS.at(i)]->first_set.end(),
                                      0) != 0) {
                                m++;
                            }
                            else{
                                break;
                            }
                        }
                        int n = m;
                        if (m!=start_4->RHS_size && m!=0){ // Rule IV
                            while (m>0){
                                for(int j=0; j<first_table[start_4->RHS.at(m)]->first_set.size();j++){
                                    if (first_table[start_4->RHS[m]]->first_set.at(j)!=0 &&
                                        count(first_table[start_4->LHS]->first_set.begin(), first_table[start_4->LHS]->first_set.end(),
                                              first_table[start_4->RHS[m]]->first_set.at(j))==0){
                                        first_table[start_4->LHS]->first_set.push_back(first_table[start_4->RHS[m]]->first_set.at(j));
                                        change = true;
                                    }
                                }
                                m--;
                            }
                        }
                        else if (n==start_4->RHS_size){
                            // Rule IV
                            n=n-1;
                            while (n>0){
                                for(int j=0; j<first_table[start_4->RHS.at(n)]->first_set.size();j++){
                                    if (first_table[start_4->RHS[n]]->first_set.at(j)!=0 &&
                                        count(first_table[start_4->LHS]->first_set.begin(), first_table[start_4->LHS]->first_set.end(),
                                              first_table[start_4->RHS[n]]->first_set.at(j))==0){
                                        first_table[start_4->LHS]->first_set.push_back(first_table[start_4->RHS[n]]->first_set.at(j));
                                        change = true;
                                    }
                                }
                                n--;
                            }
                            // Rule V
                            if (count(first_table[start_4->LHS]->first_set.begin(),
                                      first_table[start_4->LHS]->first_set.end(),
                                      0) == 0) {
                                first_table[start_4->LHS]->first_set.insert(
                                        first_table[start_4->LHS]->first_set.begin(), 0);
                                change = true;
                                // insert epsilon to the first of a vector
                            }
                        }
                    }
                }
                else{
                    // Rule V
                    if(!first_table[start_4->RHS[0]]->first_set.empty()){
                        if (count(first_table[start_4->RHS.at(0)]->first_set.begin(),
                                  first_table[start_4->RHS.at(0)]->first_set.end(),
                                  0) != 0 &&
                            count(first_table[start_4->LHS]->first_set.begin(),
                                  first_table[start_4->LHS]->first_set.end(),
                                  0) == 0) {
                            first_table[start_4->LHS]->first_set.insert(first_table[start_4->LHS]->first_set.begin(),0);
                            change = true;
                            // insert epsilon to the first of a vector
                        }
                    }
                }
            }
            start_4=start_4->next;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
//    for (int & i : non_terminal)
//    {
//        if (follow_table.count(i)==0){
//            follow_table.insert(make_pair(i, find_follow));
//            find_follow->symbol = i;
//            find_follow-> next = new follow;
//            find_follow = find_follow-> next;
//        }
//    }
//
//    struct rule* start_5 = start;
//    while (start_5!=nullptr) {
//        if (start_5->RHS_size != 0) {
//            // Apply Rule I
//            if(start_5->LHS==non_terminal[0]){
//                if (count(follow_table[start_5->LHS]->follow_set.begin(),
//                          follow_table[start_5->LHS]->follow_set.end(),1)==0){
//                    follow_table[start_5->LHS]->follow_set.insert(follow_table[start_5->LHS]->follow_set.begin(),1);
//                    // insert "$" to S
//                }
//            }
//            // Apply Rule IV
//            if (start_5->RHS_size>1){
//                int m=0;
//                for (int i=0; i<start_5->RHS_size;i++) {
//                    if (count(terminal.begin(),terminal.end(),start_5->RHS.at(i))!=0){
//                        m++;
//                    }
//                }
//                if (m<start_5->RHS_size){
//                    int cc=0;
//                    for (int i=0;i<start_5->RHS_size;i++){
//                        if(count(non_terminal.begin(),non_terminal.end(),start_5->RHS.at(i))!=0){
//                            break;
//                        }
//                        else{
//                            cc++;
//                        }
//                    }
//                    int p1=cc;
//                    int p2=cc;
////                    if (start_5->LHS==2){
////                        cout<< symbols[2]<<" "<<"P1"<<p1 <<"P2" <<" "<< p2<< endl;
////                    }
//                    while(count(terminal.begin(),terminal.end(),start_5->RHS.at(p1))==0&&
//                            p2<start_5->RHS_size-1&& p1<start_5->RHS_size-1){
//                        while(count(terminal.begin(),terminal.end(),start_5->RHS.at(p2))==0&&
//                                p2<start_5->RHS_size-1){
//                            p2++;
//                        }
//
//                        // do rule IV, add to follow set
//                        for (int i=p2; i>p1; i--){
//                            for (int j=0; j<first_table[start_5->RHS.at(i)]->first_set.size();j++){
//                                if(count(follow_table[start_5->RHS.at(i-1)]->follow_set.begin(),
//                                         follow_table[start_5->RHS.at(i-1)]->follow_set.end(),
//                                                 first_table[start_5->RHS.at(i)]->first_set[j])==0){
//                                    if(first_table[start_5->RHS.at(i)]->first_set[j]!=0){
//                                        follow_table[start_5->RHS.at(i-1)]->follow_set
//                                        .push_back(first_table[start_5->RHS.at(i)]->first_set[j]);
//                                    }
//                                }
//                            }
//                        }
//                        if(p2 == start_5->RHS_size-1)
//                            break;
//                        else{
//                            p1 = p2+1;
//                            p2 = p1;
//                        }
//                    }
//                }
//            }
//            // Apply rule V
//            if (start_5->RHS_size>1) {
//                int m = 0;
//                for (int i = 0; i < start_5->RHS_size; i++) {
//                    if (count(terminal.begin(), terminal.end(), start_5->RHS.at(i)) != 0) {
//                        m++;
//                    }
//                }
//                if (m < start_5->RHS_size) {
////                    int cc=0;
////                    for (int i=0;i<start_5->RHS_size;i++){
////                        if(count(non_terminal.begin(),non_terminal.end(),start_5->RHS.at(i))!=0){
////                            break;
////                        }
////                        else{
////                            cc++;
////                        }
////                    }
//                    int p1=0;
//                    int p2=0;
//                    while (count(terminal.begin(), terminal.end(), start_5->RHS.at(p1)) == 0 &&
//                           p2 < start_5->RHS_size - 1 && p1 < start_5->RHS_size - 1) {
//                        while (count(terminal.begin(), terminal.end(), start_5->RHS.at(p2)) == 0 &&
//                               p2 < start_5->RHS_size - 1) {
//                            p2++;
//                        }
//                        int count_z=0;
//                        for(int i=0;i<p2-p1-1;i++){
//                            if(count(first_table[start_5->RHS.at(p1+i+1)]->first_set.begin(),
//                                     first_table[start_5->RHS.at(p1+i+1)]->first_set.end(),0) !=0){
//                                count_z++;
//                            }
//                        }
//                        if (count_z==p2-p1-1){
//                            // do rule V, add to follow set
//                            for (int i = p2; i > p1; i--) {
//                                for (int k=0; k<p2-k-1;k++){
//                                    for (int j = 0; j < first_table[start_5->RHS.at(i)]->first_set.size(); j++) {
//                                        if (count(follow_table[start_5->RHS.at(k)]->follow_set.begin(),
//                                                  follow_table[start_5->RHS.at(k)]->follow_set.end(),
//                                                  first_table[start_5->RHS.at(i)]->first_set[j]) == 0) {
//                                            if (first_table[start_5->RHS.at(i)]->first_set[j] != 0) {
//                                                follow_table[start_5->RHS.at(k)]->follow_set
//                                                        .push_back(first_table[start_5->RHS.at(i)]->first_set[j]);
//                                            }
//                                        }
//                                    }
//                                }
//                            }
//                        }
//                        if (p2 == start_5->RHS_size - 1)
//                            break;
//                        else {
//                            p1 = p2 + 1;
//                            p2 = p1;
//                        }
//                    }
//                }
//            }
//        }
//        start_5=start_5->next;
//    }
//
//    change = true;
//    while (change) {
//        change = false;
//        start_5 = start;
//        while (start_5 != nullptr) {
//            if (start_5->RHS_size != 0) {
//                // Apply Rule II
////            start_5->RHS.at(start_5->RHS_size-1); // last symbol of a rule
//                if (count(non_terminal.begin(), non_terminal.end(),
//                          start_5->RHS.at(start_5->RHS_size - 1)) != 0) { // A-> a B put follow(A) to follow(B)
//                    for (int j = 0; j < follow_table[start_5->LHS]->follow_set.size(); j++) {
//                        if (count(follow_table[start_5->RHS.at(start_5->RHS_size - 1)]->follow_set.begin(),
//                                  follow_table[start_5->RHS.at(start_5->RHS_size - 1)]->follow_set.end(),
//                                  follow_table[start_5->LHS]->follow_set[j]) == 0) {
//                            if (follow_table[start_5->LHS]->follow_set[j] != 0) {
//                                follow_table[start_5->RHS.at(start_5->RHS_size - 1)]->follow_set
//                                        .push_back(follow_table[start_5->LHS]->follow_set[j]);
//                                change = true;
//                            }
//                        }
//                    }
//                }
//
//                // Apply Rule III
//                if (start_5->RHS_size > 1) {
//                    if (count(non_terminal.begin(), non_terminal.end(),
//                              start_5->RHS.at(start_5->RHS_size - 1)) != 0) {
//                        int m = start_5->RHS_size;
////                    cout << "hi" << " "<<m <<" "<< symbols[start_5->LHS]<<endl;
//                        while (count(non_terminal.begin(), non_terminal.end(),
//                                     start_5->RHS.at(m - 1)) != 0) {
//                            m--;
//                            if (m == 0) {
//                                break;
//                            }
//                        }
////                    cout<< symbols[start_5->LHS]<<" "<<"m" << " "<<m<<endl;
//                        int count_z = 0;
//                        for (int i = 0; i < start_5->RHS_size - m - 1; i++) {
//                            if (count(first_table[start_5->RHS.at(m + i + 1)]->first_set.begin(),
//                                      first_table[start_5->RHS.at(m + i + 1)]->first_set.end(), 0) != 0) {
//                                count_z++;
//                            }
//                        }
//                        if (count_z == start_5->RHS_size - m - 1) {
//                            // put value in
//                            for (int i = 0; i < start_5->RHS_size - m; i++) {
//                                for (int j = 0; j < follow_table[start_5->LHS]->follow_set.size(); j++) {
//                                    if (count(follow_table[start_5->RHS.at(m + i)]->follow_set.begin(),
//                                              follow_table[start_5->RHS.at(m + i)]->follow_set.end(),
//                                              follow_table[start_5->LHS]->follow_set[j]) == 0) {
//                                        if (follow_table[start_5->LHS]->follow_set[j] != 0) {
//                                            follow_table[start_5->RHS.at(m + i)]->follow_set
//                                                    .push_back(follow_table[start_5->LHS]->follow_set[j]);
//                                            change = true;
//                                        }
//                                    }
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//            start_5 = start_5->next;
//        }
//    }



    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

//    cout << "0\n";
}


// Task 1
void printTerminalsAndNoneTerminals()
{
    for(int j : v_all)
    {
        cout<< symbols.at(j)<< ' ';
    }

    for(int j : v_RHS)
    {
        if (symbols.at(j)!="#")
            cout<< symbols.at(j)<< ' ';
    }
    cout << endl;
//    cout << "1\n";
}



// Task 2
void RemoveUselessSymbols()
{

//    cout << "Generating Symbol"<< endl;
//    for (int i=0; i<symbols.size();i++)
//    {
//        cout<< i << " "<< symbols.at(i)<< " "<< gen_set.at(i)<< " "<< reach_set.at(i)<<endl;
//    }

    struct rule* start_3 = start;
    while (start_3!=nullptr){
        if (start_3->RHS_size!=0) {
            if (gen_set[start_3->LHS]==1 && reach_set[start_3->LHS]==1) {
                int temp = 0;
                for (int i = 0; i < start_3->RHS_size; i++) {
                    if (gen_set.at(start_3->RHS.at(i)) == 1 && reach_set.at(start_3->RHS.at(i)) == 1) {
                        temp++;
                    }
                }
                if (temp == start_3->RHS_size) {
                    //print the rule
                    cout << symbols.at(start_3->LHS) << " " << "->";
                    for (int i = 0; i < start_3->RHS_size; i++) {
                        cout << " " << symbols[start_3->RHS.at(i)];
                    }
                    cout << "\n";
                }
            }
        }
        start_3 = start_3->next;
    }


//    cout << "2\n";
}

// Task 3
void CalculateFirstSets()
{
    for (int i=0; i<non_terminal.size();i++){
        cout<< "FIRST(" << symbols[non_terminal.at(i)]<< ") = {";
        sort(first_table[non_terminal.at(i)]->first_set.begin(),first_table[non_terminal.at(i)]->first_set.end());
        int k=0;
        for (int j=0; j<first_table[non_terminal.at(i)]->first_set.size();j++){
            cout << " "<<symbols[first_table[non_terminal.at(i)]->first_set.at(j)];
            k++;
            if (k!=first_table[non_terminal.at(i)]->first_set.size()){
                cout<<",";
            } else{
                cout<< " ";
            }
        }
        cout<<"}";
        cout<<endl;
    }
//    cout << "3\n";
}

// Task 4
void CalculateFollowSets()
{

    for (int & i : non_terminal)
    {
        if (follow_table.count(i)==0){
            follow_table.insert(make_pair(i, find_follow));
            find_follow->symbol = i;
            find_follow-> next = new follow;
            find_follow = find_follow-> next;
        }
    }

    struct rule* start_5 = start;
    while (start_5!=nullptr) {
        if (start_5->RHS_size != 0) {
            // Apply Rule I
            if(start_5->LHS==non_terminal[0]){
                if (count(follow_table[start_5->LHS]->follow_set.begin(),
                          follow_table[start_5->LHS]->follow_set.end(),1)==0){
                    follow_table[start_5->LHS]->follow_set.insert(follow_table[start_5->LHS]->follow_set.begin(),1);
                    // insert "$" to S
                }
            }
            // Apply Rule IV
            if (start_5->RHS_size>1){
                int m=0;
                for (int i=0; i<start_5->RHS_size;i++) {
                    if (count(terminal.begin(),terminal.end(),start_5->RHS.at(i))!=0){
                        m++;
                    }
                }
                if (m<start_5->RHS_size){
                    int cc=0;
                    for (int i=0;i<start_5->RHS_size;i++){
                        if(count(non_terminal.begin(),non_terminal.end(),start_5->RHS.at(i))!=0){
                            break;
                        }
                        else{
                            cc++;
                        }
                    }
                    int p1=cc;
                    int p2=cc;
//                    if (start_5->LHS==2){
//                        cout<< symbols[2]<<" "<<"P1"<<p1 <<"P2" <<" "<< p2<< endl;
//                    }
                    while(count(terminal.begin(),terminal.end(),start_5->RHS.at(p1))==0&&
                          p2<start_5->RHS_size-1&& p1<start_5->RHS_size-1){
                        while(count(terminal.begin(),terminal.end(),start_5->RHS.at(p2))==0&&
                              p2<start_5->RHS_size-1){
                            p2++;
                        }

                        // do rule IV, add to follow set
                        for (int i=p2; i>p1; i--){
                            for (int j=0; j<first_table[start_5->RHS.at(i)]->first_set.size();j++){
                                if(count(follow_table[start_5->RHS.at(i-1)]->follow_set.begin(),
                                         follow_table[start_5->RHS.at(i-1)]->follow_set.end(),
                                         first_table[start_5->RHS.at(i)]->first_set[j])==0){
                                    if(first_table[start_5->RHS.at(i)]->first_set[j]!=0){
                                        follow_table[start_5->RHS.at(i-1)]->follow_set
                                                .push_back(first_table[start_5->RHS.at(i)]->first_set[j]);
                                    }
                                }
                            }
                        }
                        if(p2 == start_5->RHS_size-1)
                            break;
                        else{
                            p1 = p2+1;
                            p2 = p1;
                        }
                    }
                }
            }
            // Apply rule V
            if (start_5->RHS_size>1) {
                int m = 0;
                for (int i = 0; i < start_5->RHS_size; i++) {
                    if (count(terminal.begin(), terminal.end(), start_5->RHS.at(i)) != 0) {
                        m++;
                    }
                }
                if (m < start_5->RHS_size) {
//                    int cc=0;
//                    for (int i=0;i<start_5->RHS_size;i++){
//                        if(count(non_terminal.begin(),non_terminal.end(),start_5->RHS.at(i))!=0){
//                            break;
//                        }
//                        else{
//                            cc++;
//                        }
//                    }
                    int p1=0;
                    int p2=0;
                    while (count(terminal.begin(), terminal.end(), start_5->RHS.at(p1)) == 0 &&
                           p2 < start_5->RHS_size - 1 && p1 < start_5->RHS_size - 1) {
                        while (count(terminal.begin(), terminal.end(), start_5->RHS.at(p2)) == 0 &&
                               p2 < start_5->RHS_size - 1) {
                            p2++;
                        }
                        int count_z=0;
                        for(int i=0;i<p2-p1-1;i++){
                            if(count(first_table[start_5->RHS.at(p1+i+1)]->first_set.begin(),
                                     first_table[start_5->RHS.at(p1+i+1)]->first_set.end(),0) !=0){
                                count_z++;
                            }
                        }
                        if (count_z==p2-p1-1){
                            // do rule V, add to follow set
                            for (int i = p2; i > p1; i--) {
                                for (int k=0; k<p2-k-1;k++){
                                    for (int j = 0; j < first_table[start_5->RHS.at(i)]->first_set.size(); j++) {
                                        if (count(follow_table[start_5->RHS.at(k)]->follow_set.begin(),
                                                  follow_table[start_5->RHS.at(k)]->follow_set.end(),
                                                  first_table[start_5->RHS.at(i)]->first_set[j]) == 0) {
                                            if (first_table[start_5->RHS.at(i)]->first_set[j] != 0) {
                                                follow_table[start_5->RHS.at(k)]->follow_set
                                                        .push_back(first_table[start_5->RHS.at(i)]->first_set[j]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (p2 == start_5->RHS_size - 1)
                            break;
                        else {
                            p1 = p2 + 1;
                            p2 = p1;
                        }
                    }
                }
            }
        }
        start_5=start_5->next;
    }

    bool change = true;
    while (change) {
        change = false;
        start_5 = start;
        while (start_5 != nullptr) {
            if (start_5->RHS_size != 0) {
                // Apply Rule II
//            start_5->RHS.at(start_5->RHS_size-1); // last symbol of a rule
                if (count(non_terminal.begin(), non_terminal.end(),
                          start_5->RHS.at(start_5->RHS_size - 1)) != 0) { // A-> a B put follow(A) to follow(B)
                    for (int j = 0; j < follow_table[start_5->LHS]->follow_set.size(); j++) {
                        if (count(follow_table[start_5->RHS.at(start_5->RHS_size - 1)]->follow_set.begin(),
                                  follow_table[start_5->RHS.at(start_5->RHS_size - 1)]->follow_set.end(),
                                  follow_table[start_5->LHS]->follow_set[j]) == 0) {
                            if (follow_table[start_5->LHS]->follow_set[j] != 0) {
                                follow_table[start_5->RHS.at(start_5->RHS_size - 1)]->follow_set
                                        .push_back(follow_table[start_5->LHS]->follow_set[j]);
                                change = true;
                            }
                        }
                    }
                }

                // Apply Rule III
                if (start_5->RHS_size > 1) {
                    if (count(non_terminal.begin(), non_terminal.end(),
                              start_5->RHS.at(start_5->RHS_size - 1)) != 0) {
                        int m = start_5->RHS_size;
//                    cout << "hi" << " "<<m <<" "<< symbols[start_5->LHS]<<endl;
                        while (count(non_terminal.begin(), non_terminal.end(),
                                     start_5->RHS.at(m - 1)) != 0) {
                            m--;
                            if (m == 0) {
                                break;
                            }
                        }
//                    cout<< symbols[start_5->LHS]<<" "<<"m" << " "<<m<<endl;
                        int count_z = 0;
                        for (int i = 0; i < start_5->RHS_size - m - 1; i++) {
                            if (count(first_table[start_5->RHS.at(m + i + 1)]->first_set.begin(),
                                      first_table[start_5->RHS.at(m + i + 1)]->first_set.end(), 0) != 0) {
                                count_z++;
                            }
                        }
                        if (count_z == start_5->RHS_size - m - 1) {
                            // put value in
                            for (int i = 0; i < start_5->RHS_size - m; i++) {
                                for (int j = 0; j < follow_table[start_5->LHS]->follow_set.size(); j++) {
                                    if (count(follow_table[start_5->RHS.at(m + i)]->follow_set.begin(),
                                              follow_table[start_5->RHS.at(m + i)]->follow_set.end(),
                                              follow_table[start_5->LHS]->follow_set[j]) == 0) {
                                        if (follow_table[start_5->LHS]->follow_set[j] != 0) {
                                            follow_table[start_5->RHS.at(m + i)]->follow_set
                                                    .push_back(follow_table[start_5->LHS]->follow_set[j]);
                                            change = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            start_5 = start_5->next;
        }
    }
    for (int i=0; i<non_terminal.size();i++){
        cout<< "FOLLOW(" << symbols[non_terminal.at(i)]<< ") = {";
        sort(follow_table[non_terminal.at(i)]->follow_set.begin(),follow_table[non_terminal.at(i)]->follow_set.end());
        int k=0;
        for (int j=0; j<follow_table[non_terminal.at(i)]->follow_set.size();j++){
            cout << " "<<symbols[follow_table[non_terminal.at(i)]->follow_set.at(j)];
            k++;
            if (k!=follow_table[non_terminal.at(i)]->follow_set.size()){
                cout<<",";
            } else{
                cout<< " ";
            }
        }
        cout<<"}";
        cout<<endl;
    }
//    cout << "4\n";
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{

    cout<<"NO\n";
//    cout << "5\n";
}
    
int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);

    
    ReadGrammar();  // Reads the input grammar from standard input
                    // and represent it internally in data structures
                    // ad described in project 2 presentation file

    switch (task) {
        case 1: printTerminalsAndNoneTerminals();
            break;

        case 2: RemoveUselessSymbols();
            break;

        case 3: CalculateFirstSets();
            break;

        case 4: CalculateFollowSets();
            break;

        case 5: CheckIfGrammarHasPredictiveParser();
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}

