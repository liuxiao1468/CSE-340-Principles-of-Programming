/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
#include "compiler.h"
#include <map>
#include <vector>

using namespace std;
class Parser {
  private:
    LexicalAnalyzer lexer;

    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();
  public:

    struct StatementNode * parse_program();
    void parse_var_section();
    void parse_id_list();
    struct StatementNode * parse_body();
    struct StatementNode * parse_stmt_list();
    struct StatementNode * parse_stmt();
    struct StatementNode* parse_assign_stmt();
    void parse_expr();
    struct ValueNode* parse_primary();
    void parse_op();
    struct StatementNode * parse_print_stmt();
    struct StatementNode * parse_while_stmt();
    struct StatementNode * parse_if_stmt();
    struct IfStatement* parse_condition();
    ConditionalOperatorType parse_relop();
    struct StatementNode * parse_switch_stmt();
    struct StatementNode * parse_for_stmt();
    struct StatementNode * parse_case_list();
    struct StatementNode * parse_case();
    struct StatementNode * parse_default_case();

};

#endif

