/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include "parser.h"
#include <string>
#include <map>
#include <vector>

using namespace std;

struct ID_struct{
    string ID;
    string type;
    int decl_num = 0;
    int refer_num = 0;
    bool init = false;
    bool LHS = false;
};

struct scope{
    std::map <string, ID_struct*> symbol_map;
    struct scope *parent =NULL;
    struct scope *child = NULL;
    struct scope *next = NULL;
};

vector <string> ID_list;
string type_arr[4] = {"INT", "REAL", "STRING", "BOOLEAN"};
map<string, int> find_type;
map<int,vector<string>> error_save;
vector <string> initial_v;
vector <string> correct;

int error_count = 1;



void Parser::syntax_error()
{
    cout << "Syntax Error\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

// this function simply checks the next token without
// consuming the input
// Written by Mohsen Zohrevandi
Token Parser::peek()
{
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}


// Parsing
struct scope* Parser::parse_program() {
    Token t = lexer.GetToken();
    find_type.insert(make_pair(type_arr[0], 1));
    find_type.insert(make_pair(type_arr[1], 2));
    find_type.insert(make_pair(type_arr[2], 3));
    find_type.insert(make_pair(type_arr[3], 4));
    if (t.token_type == LBRACE){
        struct scope * current_scope = new scope;
        struct scope* head = current_scope;
        lexer.UngetToken(t);
        parse_scope(current_scope);
        return current_scope;
    }
    else{
        syntax_error();
    }
}

void Parser::parse_scope(struct scope* current_scope){
    Token t = lexer.GetToken();
    if (t.token_type== LBRACE){
        parse_scope_list(current_scope);
        Token t1 = lexer.GetToken();
        if (t1.token_type == RBRACE){
            for (map<string,ID_struct*>::const_iterator it = current_scope->symbol_map.begin();
            it!=current_scope->symbol_map.end(); ++it){
                if(it->second->refer_num==0){
                    string temp;
                    temp = "ERROR CODE 1.3 "+it->first;
                    vector <string> v;
                    v.push_back(temp);
                    error_save.insert(make_pair(error_count, v));
                    error_count++;
                    return;
                }
            }
            return;
        }
        else{
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
}

void Parser::parse_scope_list(struct scope* current_scope){
    Token t = lexer.GetToken();
    if (t.token_type == LBRACE){
        lexer.UngetToken(t);
        current_scope->child= new scope;
        current_scope->child->parent = current_scope;
        current_scope = current_scope->child;
        parse_scope(current_scope);
        Token t1 = lexer.GetToken();
        if (t1.token_type == RBRACE){
            lexer.UngetToken(t1);
            return;
        }
        else if (t1.token_type == LBRACE){
            lexer.UngetToken(t1);
            if (current_scope->parent!=NULL){
                current_scope->next = new scope;
                current_scope->next->parent = current_scope->parent;
                current_scope = current_scope->next;
            }
            else{
                current_scope->next = new scope;
                current_scope = current_scope->next;
            }
            parse_scope_list(current_scope);
        }
        else if (t1.token_type == ID || t1.token_type == WHILE){
            current_scope = current_scope->parent;
            lexer.UngetToken(t1);
            parse_scope_list(current_scope);
        }
        else{
//            cout << "4" << endl;
            syntax_error();
        }
    }
    else if ( t.token_type== WHILE){
        lexer.UngetToken(t);
        parse_stmt(current_scope);
        Token t2 = lexer.GetToken();
        if (t2.token_type == RBRACE){
            lexer.UngetToken(t2);
            return;
        }
        else if (t2.token_type == LBRACE || t2.token_type == ID || t2.token_type == WHILE){
            lexer.UngetToken(t2);
            parse_scope_list(current_scope);
        }
        else{
//            cout << "5" << endl;
            syntax_error();
        }
    }
    else if (t.token_type== ID ){
        Token t3 = lexer.GetToken();
        if (t3.token_type == COLON || t3.token_type == COMMA){
            lexer.UngetToken(t3);
            lexer.UngetToken(t);
            parse_var_decl(current_scope);
            Token t4 = lexer.GetToken();
            if (t4.token_type == RBRACE){
                lexer.UngetToken(t4);
                return;
            }
            else if (t4.token_type == LBRACE || t4.token_type == ID || t4.token_type == WHILE){
                lexer.UngetToken(t4);
                parse_scope_list(current_scope);
            }
            else{
//                cout << "6" << endl;
                syntax_error();
            }
        }
        else if (t3.token_type == EQUAL){
            lexer.UngetToken(t3);
            lexer.UngetToken(t);
            parse_stmt(current_scope);
            Token t5 = lexer.GetToken();
            if (t5.token_type == RBRACE){
                lexer.UngetToken(t5);
                return;
            }
            else if (t5.token_type == LBRACE || t5.token_type == ID || t5.token_type == WHILE){
                lexer.UngetToken(t5);
                parse_scope_list(current_scope);
            }
            else{
//                cout << "7" << endl;
                syntax_error();
            }
        }
        else{
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
}
void Parser::parse_var_decl(struct scope* current_scope) {
    Token t = lexer.GetToken();
    if (t.token_type == ID){
        lexer.UngetToken(t);
        parse_id_list(current_scope);
        Token t1 = lexer.GetToken();
        if (t1.token_type == COLON){
            Token t2 = lexer.GetToken();
            if(t2.token_type == REAL || t2.token_type == INT || t2.token_type == BOOLEAN || t2.token_type == STRING){
                for (const auto & i : ID_list){
                    current_scope->symbol_map[i]->type = t2.lexeme;
//                    cout << current_scope->symbol_map[i]->decl_num << endl;
//                    cout << current_scope->symbol_map[i]->type << endl;
                }
                ID_list.clear();
                lexer.UngetToken(t2);
                parse_type_name();
                Token t3 = lexer.GetToken();
                if (t3.token_type == SEMICOLON){
                    return;
                }
                else{
//                    cout << "8" << endl;
                    syntax_error();
                }
            }
            else{
//                cout << "9" << endl;
                syntax_error();
            }
        }
        else{
//            cout << "10" << endl;
            syntax_error();
        }
    }
    else{
//        cout << "11" << endl;
        syntax_error();
    }
}

void Parser::parse_id_list(struct scope* current_scope) {
    Token t = lexer.GetToken();
    if (t.token_type == ID){
        Token t1 = lexer.GetToken();
        if (t1.token_type == COLON){
            ID_list.push_back(t.lexeme);
            if(current_scope->symbol_map.count(t.lexeme) == 0)
            {
                auto* temp = new ID_struct;
                temp->decl_num = t.line_no;
                current_scope->symbol_map.insert(make_pair(t.lexeme, temp));
            }
            else{
                string temp;
                temp = "ERROR CODE 1.1 "+ t.lexeme;
                vector <string> v;
                v.push_back(temp);
                error_save.insert(make_pair(error_count, v));
                error_count++;
            }
            lexer.UngetToken(t1);
            return;
        }
        else if(t1.token_type == COMMA){
            ID_list.push_back(t.lexeme);
            if(current_scope->symbol_map.count(t.lexeme) == 0)
            {
                auto* temp = new ID_struct;
                temp->decl_num = t.line_no;
                current_scope->symbol_map.insert(make_pair(t.lexeme, temp));
            }
            else{
                string temp;
                temp = "ERROR CODE 1.1 "+ t.lexeme;
                vector <string> v;
                v.push_back(temp);
                error_save.insert(make_pair(error_count, v));
                error_count++;
            }
            parse_id_list(current_scope);
        }
        else{
//            cout << "12" << endl;
            syntax_error();
        }
    }
    else{
//        cout << "13" << endl;
        syntax_error();
    }
}

void Parser::parse_type_name() {
    Token t = lexer.GetToken();
    if (t.token_type == REAL || t.token_type == INT || t.token_type == BOOLEAN || t.token_type == STRING){
        return;
    }
    else{
//        cout << "14" << endl;
        syntax_error();
    }
}

void Parser::parse_stmt_list(struct scope* current_scope) {
    Token t = lexer.GetToken();
    if (t.token_type == WHILE || t.token_type == ID){
        lexer.UngetToken(t);
        parse_stmt(current_scope);
        Token t1 = lexer.GetToken();
        if (t1.token_type == RBRACE){
            lexer.UngetToken(t1);
            return;
        }
        else if (t1.token_type == ID || t1.token_type == WHILE){
            lexer.UngetToken(t1);
            parse_stmt_list(current_scope);
        }
        else{
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
}

void Parser::parse_stmt(struct scope* current_scope) {
    Token t = lexer.GetToken();
    if (t.token_type == ID){
        lexer.UngetToken(t);
        parse_assign_stmt(current_scope);
    }
    else if (t.token_type==WHILE){
        lexer.UngetToken(t);
        parse_while_stmt(current_scope);
    }
    else{
//        cout << "17" << endl;
        syntax_error();
    }
}

void Parser::parse_assign_stmt(struct scope* current_scope) {
    Token t = lexer.GetToken();
    int type_LHS = 0;
    int type_RHS=0;
    struct scope* temp_scope = current_scope;
    if (t.token_type == ID){


        while (temp_scope->parent != NULL ){
            if (temp_scope->symbol_map.count(t.lexeme) != 0){
                break;
            }
            else{
                temp_scope = temp_scope->parent;
            }
        }
        if (temp_scope->symbol_map.count(t.lexeme) == 0){
            string temp;
            temp = "ERROR CODE 1.2 "+ t.lexeme;
            vector <string> v;
            v.push_back(temp);
            error_save.insert(make_pair(error_count, v));
            error_count++;
        }
        else{
            temp_scope->symbol_map[t.lexeme]->refer_num = t.line_no;
            type_LHS = find_type[temp_scope->symbol_map[t.lexeme]->type];
            temp_scope->symbol_map[t.lexeme]->LHS = true;
            string temp_correct;
            temp_correct = t.lexeme+ " "+to_string(temp_scope->symbol_map[t.lexeme]->refer_num) +" "+
                           to_string(temp_scope->symbol_map[t.lexeme]->decl_num);
            correct.push_back(temp_correct);
        }


        Token t1 = lexer.GetToken();
        if (t1.token_type == EQUAL){

            Token t2 = lexer.GetToken();
            if (t2.token_type == PLUS || t2.token_type == MINUS || t2.token_type == MULT
            || t2.token_type == DIV || t2.token_type == AND || t2.token_type == OR || t2.token_type == XOR
            || t2.token_type == GREATER || t2.token_type == GTEQ || t2.token_type == LESS
            || t2.token_type == NOTEQUAL || t2.token_type == LTEQ
            || t2.token_type == NOT|| t2.token_type == ID){
                lexer.UngetToken(t2);
                type_RHS = parse_expr(current_scope);
                Token t3 = lexer.GetToken();
                if (t3.token_type == SEMICOLON){
                    if ( (type_LHS == 1&& type_RHS==1)||
                            (type_LHS == 4&& type_RHS==4)||
                            (type_LHS == 3&& type_RHS==3)){
                        temp_scope->symbol_map[t.lexeme]->init = true;
                        return;
                    }
                    else if (type_LHS == 2){
                        if (type_RHS==1 || type_RHS==2){
                            temp_scope->symbol_map[t.lexeme]->init = true;
                            return;
                        }
                        else{
                            string temp;
                            temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C2";
                            vector <string> v;
                            v.push_back(temp);
                            error_save.insert(make_pair(error_count, v));
                            error_count++;
                        }
                    }
                    else{
                        string temp;
                        temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C1";
                        vector <string> v;
                        v.push_back(temp);
                        error_save.insert(make_pair(error_count, v));
                        error_count++;
                        return;
                    }
                }
                else{
                    syntax_error();
                }
            }
            else if (t2.token_type == NUM ||t2.token_type == REALNUM ||t2.token_type == STRING_CONSTANT
                     ||t2.token_type == TRUE ||t2.token_type == FALSE ){
                // do something
                lexer.UngetToken(t2);
                temp_scope->symbol_map[t.lexeme]->init = true;
                type_RHS = parse_expr(current_scope);
                Token t3 = lexer.GetToken();
                if (t3.token_type == SEMICOLON){
                    if ( (type_LHS == 1&& type_RHS==1)||
                         (type_LHS == 4&& type_RHS==4)||
                         (type_LHS == 3&& type_RHS==3)){
                        return;
                    }
                    else if (type_LHS == 2){
                        if (type_RHS==1 || type_RHS==2){
                            return;
                        }
                        else{
                            string temp;
                            temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C2";
                            vector <string> v;
                            v.push_back(temp);
                            error_save.insert(make_pair(error_count, v));
                            error_count++;
                        }
                    }
                    else{
                        string temp;
                        temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C1";
                        vector <string> v;
                        v.push_back(temp);
                        error_save.insert(make_pair(error_count, v));
                        error_count++;
                        return;
                    }
                }
                else{
                    syntax_error();
                }
            }
            else{
                syntax_error();
            }
        }
        else{
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
}

void Parser::parse_while_stmt(struct scope* current_scope) {
    Token t = lexer.GetToken();
    if (t.token_type == WHILE){
        Token t1 = lexer.GetToken();
        if (t1.token_type == LPAREN){
            lexer.UngetToken(t1);
            parse_condition(current_scope);
            Token t2 = lexer.GetToken();
            if (t2.token_type == LBRACE){
                parse_stmt_list(current_scope);
                Token t3 = lexer.GetToken();
                if (t3.token_type == RBRACE){
                    return;
                }
                else{
//                    cout << "22" << endl;
                    syntax_error();
                }
            }
            else if (t2.token_type == ID || t2.token_type == WHILE){
                lexer.UngetToken(t2);
                parse_stmt(current_scope);
            }
            else{
//                cout << "23" << endl;
                syntax_error();
            }
        }
        else{
//            cout << "24" << endl;
            syntax_error();
        }
    }
    else{
//        cout << "25" << endl;
        syntax_error();
    }
}

int Parser::parse_expr(struct scope* current_scope) {
    Token t = lexer.GetToken();
    int type_1;
    int type_2;
    int type;
    if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT ){
        lexer.UngetToken(t);
        parse_arithmetic_operator();
        type_1=parse_expr(current_scope);
        type_2=parse_expr(current_scope);
        if (type_1 == 1 && type_2 == 1){
            return type_1;
        }
        else if ((type_1==1 && type_2 == 2 )|| (type_1==2 && type_2 == 1)){
            type = 2;
            return type;
        }
        else{
            string temp;
            temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C3";
            vector <string> v;
            v.push_back(temp);
            error_save.insert(make_pair(error_count, v));
            error_count++;
            return 0;
        }
    }
    else if (t.token_type == DIV){
        lexer.UngetToken(t);
        parse_arithmetic_operator();
        type_1=parse_expr(current_scope);
        type_2=parse_expr(current_scope);
        if (type_1 == 1 && type_2 == 1){
            type = 2;
            return type;
        }
        else if ( (type_1==1 && type_2 == 2 )|| (type_1==2 && type_2 == 1)
        || (type_1==2 && type_2 == 2)){
            type = 2;
            return type;
        }
        else{
            string temp;
            temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C3";
            vector <string> v;
            v.push_back(temp);
            error_save.insert(make_pair(error_count, v));
            error_count++;
            return 0;
        }
    }
    else if (t.token_type == AND || t.token_type == OR || t.token_type == XOR){
        lexer.UngetToken(t);
        parse_binary_boolean_operator();
        type_1=parse_expr(current_scope);
        type_2=parse_expr(current_scope);
        if (type_1 == 4 && type_2 == 4){
            return type_1;
        }
        else{
            string temp;
            temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C4";
            vector <string> v;
            v.push_back(temp);
            error_save.insert(make_pair(error_count, v));
            error_count++;
            return 0;
        }
    }
    else if (t.token_type == GREATER || t.token_type == GTEQ || t.token_type == LESS
    || t.token_type == NOTEQUAL || t.token_type == LTEQ){
        lexer.UngetToken(t);
        parse_relational_operator();
        type_1=parse_expr(current_scope);
        type_2=parse_expr(current_scope);
        if ((type_1 == 1|| type_1 == 2) && (type_2 == 1|| type_2 == 2)){
            type = 4;
            return type;
        }
        else if ( (type_1 != 1&& type_1 != 2)&&(type_2!=1 && type_2!=2) ){
            if ( (type_1 == 3 && type_2 == 3)||(type_1 == 4 && type_2 == 4) ){
                type = 4;
                return type;
            }
            else{
                string temp;
                temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C5";
                vector <string> v;
                v.push_back(temp);
                error_save.insert(make_pair(error_count, v));
                error_count++;
                return 0;
            }
        }
        else{
            string temp;
            temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C6";
            vector <string> v;
            v.push_back(temp);
            error_save.insert(make_pair(error_count, v));
            error_count++;
            return 0;
        }
    }
    else if (t.token_type == NOT){
        type = parse_expr(current_scope);
        if (type == 4){
            return type;
        }
        else{
            string temp;
            temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C8";
            vector <string> v;
            v.push_back(temp);
            error_save.insert(make_pair(error_count, v));
            error_count++;
            return 0;
        }
    }
    else if (t.token_type == ID){
        lexer.UngetToken(t);
        return parse_primary(current_scope);
    }
    else if (t.token_type == NUM || t.token_type == REALNUM ||
             t.token_type == STRING_CONSTANT || t.token_type == TRUE || t.token_type == FALSE){
        lexer.UngetToken(t);
        return parse_primary(current_scope);
    }
    else{
//        cout << "33" << endl;
        syntax_error();
    }
}

void Parser::parse_arithmetic_operator() {
    Token t = lexer.GetToken();
    if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV){
        return;
    }
    else{
//        cout << "34" << endl;
        syntax_error();
    }
}

void Parser::parse_binary_boolean_operator() {
    Token t = lexer.GetToken();
    if (t.token_type == AND || t.token_type == OR || t.token_type == XOR){
        return;
    }
    else{
//        cout << "35" << endl;
        syntax_error();
    }
}

void Parser::parse_relational_operator() {
    Token t = lexer.GetToken();
    if (t.token_type == GREATER || t.token_type == GTEQ || t.token_type == LESS || t.token_type == NOTEQUAL
    || t.token_type == LTEQ){
        return;
    }
    else{
//        cout << "36" << endl;
        syntax_error();
    }
}

int Parser::parse_primary(struct scope* current_scope) {
    Token t = lexer.GetToken();
    string type;
    int type_no;
    struct scope* temp_scope = current_scope;
    if (t.token_type == ID ){
        while (temp_scope->parent != NULL ){
            if (temp_scope->symbol_map.count(t.lexeme) != 0){
                break;
            }
            else{
                temp_scope = temp_scope->parent;
            }
        }
        if (temp_scope->symbol_map.count(t.lexeme) == 0){
            string temp;
            temp = "ERROR CODE 1.2 "+ t.lexeme;
            vector <string> v;
            v.push_back(temp);
            error_save.insert(make_pair(error_count, v));
            error_count++;
        }
        else{
            if (temp_scope->symbol_map[t.lexeme]->init==false) {
                string temp;
                temp = "UNINITIALIZED "+t.lexeme+" "+to_string(t.line_no);
                initial_v.push_back(temp);
                temp_scope->symbol_map[t.lexeme]->refer_num = t.line_no;
                type = temp_scope->symbol_map[t.lexeme]->type;
                type_no = find_type[type];
                return type_no;
//                if (temp_scope->symbol_map[t.lexeme]->LHS == true){
//                    string temp;
//                    temp = "UNINITIALIZED "+t.lexeme+" "+to_string(t.line_no);
//                    initial_v.push_back(temp);
//                    temp_scope->symbol_map[t.lexeme]->refer_num = t.line_no;
//                    type = temp_scope->symbol_map[t.lexeme]->type;
//                    type_no = find_type[type];
//                    return type_no;
//                }
//                else{
//                    string temp;
//                    temp = "UNINITIALIZED "+t.lexeme+" "+to_string(t.line_no);
//                    initial_v.push_back(temp);
//                    temp_scope->symbol_map[t.lexeme]->refer_num = t.line_no;
//                    type = temp_scope->symbol_map[t.lexeme]->type;
//                    type_no = find_type[type];
//                    return type_no;
//                }
            }
            else {
                temp_scope->symbol_map[t.lexeme]->refer_num = t.line_no;
                type = temp_scope->symbol_map[t.lexeme]->type;
                type_no = find_type[type];
                string temp_correct;
                temp_correct = t.lexeme+ " "+to_string(temp_scope->symbol_map[t.lexeme]->refer_num) +" "+
                        to_string(temp_scope->symbol_map[t.lexeme]->decl_num);
                correct.push_back(temp_correct);
                return type_no;
            }


            temp_scope->symbol_map[t.lexeme]->refer_num = t.line_no;
            type = temp_scope->symbol_map[t.lexeme]->type;
            type_no = find_type[type];
            return type_no;
        }
    }
    else if (t.token_type == NUM){
        type = "INT";
        type_no = find_type[type];
        return type_no;
    }
    else if (t.token_type == REALNUM){
        type = "REAL";
        type_no = find_type[type];
        return type_no;
    }
    else if (t.token_type == STRING_CONSTANT){
        type = "STRING";
        type_no = find_type[type];
        return type_no;
    }
    else if (t.token_type == TRUE || t.token_type == FALSE){
        type = "BOOLEAN";
        type_no = find_type[type];
        return type_no;
    }
    else{
//        cout << "37" << endl;
        syntax_error();
    }
}

void Parser::parse_condition(struct scope* current_scope) {
    Token t = lexer.GetToken();
    int type;
    if (t.token_type == LPAREN){
        type = parse_expr(current_scope);
        Token t1 = lexer.GetToken();
        if (t1.token_type == RPAREN){
            if (type!=4){
                string temp;
                temp = "TYPE MISMATCH "+ to_string(t.line_no) +" C7";
                vector <string> v;
                v.push_back(temp);
                error_save.insert(make_pair(error_count, v));
                error_count++;
            }
            return;
        }
        else{
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    auto *p = new Parser;
    p->parse_program();
//    for (auto &s: error_save[1]){
//        cout<<s<<endl;
//    }
    if (error_save.count(1)==0){
        if (initial_v.empty()){
            // to do
            if (!correct.empty()){
                for(auto &s: correct){
                    cout<< s<<endl;
                }
            }
        }
        else{
            for(auto &s: initial_v){
                cout<< s<<endl;
            }
        }
    }
    else{
        cout<<error_save[1].front()<<endl;
    }

//    while (token.token_type != END_OF_FILE)
//    {
//        token = lexer.GetToken();
//        token.Print();
//    }
}

//int main()
//{
//    LexicalAnalyzer lexer;
//    Token token;
//
//
//    // token = lexer.GetToken();
//    // token.Print();
//}
