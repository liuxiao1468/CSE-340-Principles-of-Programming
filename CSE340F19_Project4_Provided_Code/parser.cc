#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#include "parser.h"
#include <map>
#include <vector>

using namespace std;
map <string, ValueNode*> symbol_table;

void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
    exit(1);
}

struct StatementNode * Parser::parse_program() {
    struct StatementNode* s;
    parse_var_section();
    s = parse_body();
    return s;
}

void Parser::parse_var_section() {
    parse_id_list();
    Token t = lexer.GetToken();
    if (t.token_type == SEMICOLON){
        return;
    }
    else{
        syntax_error();
    }
}

void Parser::parse_id_list() {
    Token t = lexer.GetToken();
    if (t.token_type == ID){
        struct ValueNode * temp = new ValueNode;
        temp->name = t.lexeme;
        temp->value = 0;
        symbol_table.insert(make_pair(t.lexeme, temp));
        Token t1 = lexer.GetToken();
        if (t1.token_type == COMMA){
            parse_id_list();
        }
        else if (t1.token_type == SEMICOLON){
            lexer.UngetToken(t1);
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

struct StatementNode * Parser::parse_body() {
    struct StatementNode* s;
    Token t = lexer.GetToken();
    if (t.token_type == LBRACE){
        s = parse_stmt_list();
        Token t1 = lexer.GetToken();
        if (t1.token_type == RBRACE){
            return s;
        }
        else{
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
}

struct StatementNode * Parser::parse_stmt_list() {
    struct StatementNode* s;
    struct StatementNode* temp;
    struct StatementNode* stl;
    s = parse_stmt();
    temp = s;
    while (temp->next!=NULL){
        temp = temp->next;
    }
//    cout<<temp->type<<endl;
    Token t = lexer.GetToken();
    if (t.token_type == ID || t.token_type == PRINT || t.token_type == WHILE || t.token_type == IF ||
            t.token_type == SWITCH || t.token_type == FOR){
        lexer.UngetToken(t);

        stl = parse_stmt_list();
        temp->next = stl;
//        if (s->type == IF_STMT){
//            s->next->next = stl;
//        }
//        else{
//            s->next = stl;
//        }
        return s;
    }
    else if (t.token_type == RBRACE){
        lexer.UngetToken(t);
        return s;
    }
    else{
        syntax_error();
    }
}

struct StatementNode * Parser::parse_stmt() {
    Token t = lexer.GetToken();
    struct StatementNode* st;
    if (t.token_type == ID){
        lexer.UngetToken(t);
        st = parse_assign_stmt();
        return st;
    }
    else if (t.token_type == PRINT){
        lexer.UngetToken(t);
        st = parse_print_stmt();
        return st;
    }
    else if (t.token_type == WHILE){
        lexer.UngetToken(t);
        st = parse_while_stmt();
        return st;
    }
    else if (t.token_type == IF){
        lexer.UngetToken(t);
        st = parse_if_stmt();
        return st;
    }
    else if (t.token_type == SWITCH){
        lexer.UngetToken(t);
        st = parse_switch_stmt();
        return st;
    }
    else if (t.token_type == FOR){
        lexer.UngetToken(t);
        st = parse_for_stmt();
        return st;
    }
    else{
        syntax_error();
    }
}

struct StatementNode * Parser::parse_assign_stmt() {
    Token t = lexer.GetToken();
    struct StatementNode * s = new StatementNode;
    if (t.token_type == ID){
        s->type = ASSIGN_STMT;
        s->assign_stmt = new AssignmentStatement;
        s->assign_stmt->left_hand_side = symbol_table[t.lexeme];
        Token t1 = lexer.GetToken();
        if (t1.token_type == EQUAL){
            Token t2 = lexer.GetToken();
            if (t2.token_type == ID || t2.token_type == NUM){
                Token t3 = lexer.GetToken();
                if (t3.token_type == PLUS || t3.token_type == MINUS ||
                t3.token_type == MULT || t3.token_type == DIV){

                    if (t2.token_type == ID){
                        s->assign_stmt->operand1= symbol_table[t2.lexeme];
                    }
                    else if (t2.token_type == NUM){
                        struct ValueNode * temp1 = new ValueNode;
                        temp1->name = t2.lexeme;
                        temp1->value = stoi(t2.lexeme);
                        s->assign_stmt->operand1= temp1;
                    }
                    if (t3.token_type == PLUS){
                        s->assign_stmt->op = OPERATOR_PLUS;
                    }
                    else if (t3.token_type == MINUS){
                        s->assign_stmt->op = OPERATOR_MINUS;
                    }
                    else if (t3.token_type == MULT){
                        s->assign_stmt->op = OPERATOR_MULT;
                    }
                    else if (t3.token_type == DIV){
                        s->assign_stmt->op = OPERATOR_DIV;
                    }

                    Token t5 = lexer.GetToken();

                    if (t5.token_type == ID){
                        s->assign_stmt->operand2 = symbol_table[t5.lexeme];
                    }
                    else if(t5.token_type == NUM){
                        struct ValueNode * temp = new ValueNode;
                        temp->name = t5.lexeme;
                        temp->value = stoi(t5.lexeme);
                        s->assign_stmt->operand2 = temp;
                    }
                    lexer.UngetToken(t5);
                    lexer.UngetToken(t3);
                    lexer.UngetToken(t2);
                    parse_expr();
                    Token t4 = lexer.GetToken();
                    if (t4.token_type == SEMICOLON){
                        return s;
                    }
                    else{
                        syntax_error();
                    }
                }
                else if (t3.token_type == SEMICOLON){

                    if (t2.token_type == ID){
                        s->assign_stmt->operand1 = symbol_table[t2.lexeme];
                    }
                    else if (t2.token_type == NUM){
                        struct ValueNode * temp1 = new ValueNode;
                        temp1->name = t2.lexeme;
                        temp1->value = stoi(t2.lexeme);
                        s->assign_stmt->operand1= temp1;
                    }
                    s->assign_stmt->op = OPERATOR_NONE;
                    s->assign_stmt->operand2 = NULL;
                    lexer.UngetToken(t3);
                    lexer.UngetToken(t2);
                    parse_primary();
                    Token t4 = lexer.GetToken();
                    if (t4.token_type == SEMICOLON){
                        return s;
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
    else{
        syntax_error();
    }
}

void Parser::parse_expr() {
    parse_primary();
    parse_op();
    parse_primary();
}

struct ValueNode*  Parser::parse_primary() {
    Token t = lexer.GetToken();
    if (t.token_type == ID){
        return symbol_table[t.lexeme];
    }
    else if (t.token_type == NUM){
        struct ValueNode * temp = new ValueNode;
        temp->name = t.lexeme;
        temp->value = stoi(t.lexeme);
        return temp;
    }
    else{
        syntax_error();
    }
}

void Parser::parse_op() {
    Token t = lexer.GetToken();
    if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV){
        return;
    }
    else{
        syntax_error();
    }
}

struct StatementNode * Parser::parse_print_stmt() {
    Token t = lexer.GetToken();
    struct StatementNode * s = new StatementNode;
    s->type = PRINT_STMT;
    s->print_stmt = new PrintStatement;
    if (t.token_type == PRINT){
        Token t1 = lexer.GetToken();
        if (t1.token_type == ID){
            s->print_stmt->id = symbol_table[t1.lexeme];
            Token t2 = lexer.GetToken();
            if (t2.token_type == SEMICOLON){
                return s;
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

struct StatementNode * Parser::parse_while_stmt() {
    Token t = lexer.GetToken();
    struct StatementNode * s = new StatementNode;
    s->type = IF_STMT;
    if (t.token_type == WHILE){
        s->if_stmt = parse_condition();
        s->if_stmt->true_branch = parse_body();
        struct StatementNode * gt = new StatementNode;
        gt->type = GOTO_STMT;
        struct GotoStatement * gt_node = new GotoStatement;
        gt->goto_stmt = gt_node;
        gt_node->target=s;
        struct StatementNode * temp;
        temp = s->if_stmt->true_branch;
        while (temp->next != NULL){
            temp = temp->next;
        }
        temp->next = gt;

        struct StatementNode * noop = new StatementNode;
        noop->type = NOOP_STMT;
        gt->next=noop;
        s->if_stmt->false_branch = noop;
        s->next = noop;

        return s;
    }
    else{
        syntax_error();
    }
}

struct StatementNode * Parser::parse_if_stmt() {
    Token t = lexer.GetToken();
    struct StatementNode * s = new StatementNode;
    if (t.token_type == IF){
        s->type = IF_STMT;
        s->if_stmt = parse_condition();
        s->if_stmt->true_branch = parse_body();
        struct StatementNode * noop = new StatementNode;
        noop->type = NOOP_STMT;

        struct StatementNode * temp;
        temp = s->if_stmt->true_branch;
        while (temp->next != NULL){
            temp = temp->next;
        }
        temp->next = noop;
        s->if_stmt->false_branch = noop;
        s->next = noop;
        return s;
    }
    else{
        syntax_error();
    }
}

struct IfStatement* Parser::parse_condition() {
    struct IfStatement * ifs = new IfStatement;
    struct ValueNode* v1;
    struct ValueNode* v2;
    ConditionalOperatorType t;
    v1 = parse_primary();
    t = parse_relop();
    v2 = parse_primary();
    ifs->condition_operand1 = v1;
    ifs->condition_operand2 = v2;
    ifs->condition_op = t;
    return ifs;
}

ConditionalOperatorType Parser::parse_relop() {
    Token t = lexer.GetToken();
    if (t.token_type == GREATER ){
        return CONDITION_GREATER;
    }
    else if (t.token_type == LESS){
        return CONDITION_LESS;
    }
    else if (t.token_type == NOTEQUAL){
        return CONDITION_NOTEQUAL;
    }
    else{
        syntax_error();
    }
}

struct StatementNode * Parser::parse_switch_stmt() {
    struct StatementNode * s;
    struct StatementNode * d_s;
    Token t = lexer.GetToken();
    if (t.token_type == SWITCH){
        Token t1 = lexer.GetToken();
        if (t1.token_type == ID){
            Token t2 = lexer.GetToken();
            if (t2.token_type == LBRACE){
                s = parse_case_list();
                Token t3 = lexer.GetToken();
                if (t3.token_type == DEFAULT){
                    lexer.UngetToken(t3);
                    d_s = parse_default_case();
                    Token t4 = lexer.GetToken();
                    if (t4.token_type == RBRACE){

                        // switch with default
                        struct StatementNode * default_case = new StatementNode;

                        default_case->type = IF_STMT;
                        struct IfStatement * ifs = new IfStatement;
                        ifs->condition_op = CONDITION_NOTEQUAL;
                        ifs->condition_operand1 = symbol_table[t1.lexeme];
                        ifs->condition_operand2 = symbol_table[t1.lexeme];
                        ifs->false_branch = d_s;
                        default_case->if_stmt = ifs;

                        struct StatementNode * noop = new StatementNode;
                        noop->type = NOOP_STMT;
                        default_case->if_stmt->true_branch = noop;

                        struct StatementNode * k_temp;
                        k_temp = s;
                        while (k_temp->next!=NULL){
                            k_temp = k_temp->next;
                        }
                        k_temp->next = default_case;
                        default_case->next = noop;

                        struct StatementNode * temp;
                        temp = s;

                        while (temp->next!=NULL){
                            temp->if_stmt->condition_operand1 = symbol_table[t1.lexeme];
                            temp->if_stmt->true_branch = temp->next;
                            struct StatementNode * tem;
                            tem = temp->if_stmt->false_branch;
                            while (tem->next!=NULL){
                                tem = tem->next;
                            }
                            struct StatementNode * gt = new StatementNode;
                            gt->type = GOTO_STMT;
                            struct GotoStatement * gt_node = new GotoStatement;
                            gt->goto_stmt = gt_node;
                            gt_node->target=noop;
                            gt->next = noop;
                            tem->next = gt;

                            temp = temp->next;
                        }
                        return s;
                    }
                    else{
                        syntax_error();
                    }
                }
                else if (t3.token_type == RBRACE){
                    struct StatementNode * temp;
                    struct StatementNode * k_temp;
                    temp = s;
                    k_temp = s;
                    while (k_temp->next!=NULL){
                        k_temp = k_temp->next;
                    }
                    struct StatementNode * noop = new StatementNode;
                    noop->type = NOOP_STMT;
                    k_temp->next = noop;

                    while (temp->next!=NULL){
                        temp->if_stmt->condition_operand1 = symbol_table[t1.lexeme];
                        temp->if_stmt->true_branch = temp->next;
                        struct StatementNode * tem;
                        tem = temp->if_stmt->false_branch;
                        while (tem->next!=NULL){
                            tem = tem->next;
                        }
                        struct StatementNode * gt = new StatementNode;
                        gt->type = GOTO_STMT;
                        struct GotoStatement * gt_node = new GotoStatement;
                        gt->goto_stmt = gt_node;
                        gt_node->target=k_temp->next;
                        gt->next = k_temp->next;
                        tem->next = gt;

                        temp = temp->next;
                    }
                    return s;
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

struct StatementNode * Parser::parse_for_stmt() {
    struct StatementNode * s = new StatementNode;
    s->type = IF_STMT;
    struct StatementNode * s1;
    struct StatementNode * s2;
    struct StatementNode * stl;
    Token t = lexer.GetToken();
    if (t.token_type == FOR){
        Token t1 = lexer.GetToken();
        if (t1.token_type == LPAREN){
            s1 = parse_assign_stmt();
            s->if_stmt = parse_condition();
            Token t2 = lexer.GetToken();
            if (t2.token_type == SEMICOLON){
                s2 = parse_assign_stmt();
                Token t3 = lexer.GetToken();
                if (t3.token_type == RPAREN){
                    s->if_stmt->true_branch = parse_body();
                    struct StatementNode * noop = new StatementNode;
                    noop->type = NOOP_STMT;
                    s->next = noop;
                    s->if_stmt->false_branch = noop;
                    stl = s->if_stmt->true_branch;
                    while (stl->next!=NULL){
                        stl = stl->next;
                    }
                    stl->next = s2;
                    stl = stl->next;
                    struct StatementNode * gt = new StatementNode;
                    gt->type = GOTO_STMT;
                    struct GotoStatement * gt_node = new GotoStatement;
                    gt->goto_stmt = gt_node;
                    gt_node->target=s;
                    gt->next = noop;
                    stl->next = gt;

                    s1->next = s;
                    return s1;
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

struct StatementNode * Parser::parse_case_list() {
    struct StatementNode * s;
    struct StatementNode * s_list;

    Token t = lexer.GetToken();
    if (t.token_type == CASE){
        lexer.UngetToken(t);
        s = parse_case();
        Token t1 = lexer.GetToken();
        if (t1.token_type == RBRACE || t1.token_type == DEFAULT){
            lexer.UngetToken(t1);
            return s;
        }
        else if (t1.token_type == CASE){
            lexer.UngetToken(t1);
            s_list = parse_case_list();
            s->next = s_list;
            return s;
        }
        else{
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
}

struct StatementNode * Parser::parse_case() {
    struct StatementNode * s = new StatementNode;
    s->type = IF_STMT;
    struct IfStatement * ifs = new IfStatement;
    s->if_stmt = ifs;
    Token t = lexer.GetToken();
    if (t.token_type == CASE){
        Token t1 = lexer.GetToken();
        if (t1.token_type == NUM){
            struct ValueNode * v = new ValueNode;
            v->name = t1.lexeme;
            v->value = stoi(t1.lexeme);
            ifs->condition_operand2 = v;
            ifs->condition_op = CONDITION_NOTEQUAL;
            Token t2 = lexer.GetToken();
            if (t2.token_type == COLON){
                ifs->false_branch = parse_body();
                return s;
            }
            else{
                syntax_error();
            }
        }
        syntax_error();
    }
    syntax_error();
}

struct StatementNode * Parser::parse_default_case() {
    struct StatementNode* s;
    Token t = lexer.GetToken();
    if (t.token_type == DEFAULT){
        Token t1 = lexer.GetToken();
        if (t1.token_type == COLON){
            s = parse_body();
            return s;
        }
        else{
            syntax_error();
        }
    }
    else{
        syntax_error();
    }
}

struct StatementNode * parse_generate_intermediate_representation(){
    struct StatementNode * s = new StatementNode;
    Parser *p = new Parser;
    return p->parse_program();
}