/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
#include <map>

class Parser {
  private:
    LexicalAnalyzer lexer;

    void syntax_error();
    Token expect(TokenType expected_type);
    Token peek();
  public:
  	void parse_input();
  	void parse_program();
  	void parse_proc_decl_section();
    struct proc_table* parse_proc_decl();
    void parse_procedure_name();
    struct stmt* parse_procedure_body();
    struct stmt* parse_statement_list();
    struct stmt* parse_statement();
    struct stmt* parse_input_statement();
    struct stmt* parse_output_statement();
    struct stmt* parse_procedure_invocation();
    struct stmt* parse_do_statement();
    struct stmt* parse_assign_statement();
  	void parse_expr();
  	void parse_operator();
  	void parse_primary();
  	void parse_main();
  	void parse_inputs();
    int execute_program(struct stmt* start);
//  	std::map <std::string, int> m_operator_type;
//    std::string p="PLUS";

/*    std::m_operator_type.insert(make_pair(p, 1));
    std::m_operator_type.insert(make_pair("MINUS", 2));
    std::m_operator_type.insert(make_pair("MULT", 3));
    std::m_operator_type.insert(make_pair("DIV", 4));
    std::m_operator_type.insert(make_pair("NOOP", 5));*/



};

#endif

