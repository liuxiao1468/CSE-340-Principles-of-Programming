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

using namespace std;

struct stmt
{
    int statement_type;
    int LHS;
    int operator_type;
    int op1;
    int op2;
    string proc_new_code;
    struct stmt * next;
};

struct proc_table
{
    string proc_name;
    stmt * pc_code;
};
struct proc_table* proc_struct = new proc_table;


//struct stmt * st = new stmt;
//struct stmt* head = st;

int i=0;
int j=0;
int next_input=0;
int mem[1000] = { 0 }; // memory with all elements 0
int inputs[1000] = { 0 }; // memory with all elements 0

std::map <string, int> map_ID;
std::map <string, stmt*> proc_map;




void Parser::syntax_error()
{
    cout << "SYNTAX ERROR\n";
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

void Parser::parse_input()
{
    Token t = lexer.GetToken();
    if (t.token_type == MAIN || t.token_type == PROC)
    {
        lexer.UngetToken(t);
        parse_program();
        Token t = lexer.GetToken();
        if (t.token_type == NUM)
        {
            lexer.UngetToken(t);
            parse_inputs();
        }
        else 
        {
            cout << "1" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "2" << endl;
        syntax_error();
    }
}


void Parser::parse_program()
{
    Token t = lexer.GetToken();
    if (t.token_type == MAIN)
    {
        lexer.UngetToken(t);
        parse_main();
        return;
    }
    else if (t.token_type == PROC)
    {
        lexer.UngetToken(t);
        parse_proc_decl_section();
        Token t = lexer.GetToken();
        if (t.token_type == MAIN)
        {
            lexer.UngetToken(t);
            parse_main();
            return;
        }
        else
        {
            cout << "3" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "4" << endl;
        syntax_error();
    }
}

void Parser::parse_proc_decl_section()
{
    struct proc_table* temp;
    Token t = lexer.GetToken();
    if (t.token_type == PROC)
    {
        lexer.UngetToken(t);
        temp = parse_proc_decl();
        proc_map.insert(make_pair(temp->proc_name, temp->pc_code));
//        cout << "the procedure name"<< endl;
//        cout << temp->proc_name<< endl;
        Token t1 = lexer.GetToken();
        if (t1.token_type == PROC)
        {
            lexer.UngetToken(t1);
            parse_proc_decl_section();
        }
        else if (t1.token_type == MAIN)
        {
            lexer.UngetToken(t1);
            return;
        }
        else
        {
            cout << "5" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "6" << endl;
        syntax_error();
    }
}

struct proc_table*Parser::parse_proc_decl()
{
    struct stmt*temp;
    Token t = lexer.GetToken();
    if (t.token_type == PROC)
    {
    	Token t1 = lexer.GetToken();
    	if (t1.token_type == ID || t1.token_type == NUM)
    	{
    		lexer.UngetToken(t1);
    		parse_procedure_name();
	        Token t2 = lexer.GetToken();
	        if (t2.token_type == ID || t2.token_type == NUM || t2.token_type == INPUT || t2.token_type == OUTPUT || t2.token_type == DO)
	        {
	           lexer.UngetToken(t2);
	           temp = parse_procedure_body();
	           Token t3 = lexer.GetToken();
	           if (t3.token_type == ENDPROC)
	           {
                   proc_struct->proc_name=t1.lexeme;
	               proc_struct->pc_code = temp;
//                   while(temp)
//                   {
//                       cout<<"this is the Proc stl" <<endl;
//                       cout<< temp->statement_type << endl;
//                       cout<< temp->LHS<< endl;
//                       cout<< temp->operator_type<<endl;
//                       cout<< temp->op1<< endl;
//                       cout<< temp->op2<< endl;
//                       temp = temp->next;
//                   }
                   return proc_struct;
	           }
//                   cout<< "from here" << endl;
//                   cout<< temp->statement_type << endl;
//                   cout<< temp->LHS<< endl;
//                   cout<< temp->operator_type<<endl;
//                   cout<< temp->op1<< endl;
//                   cout<< temp->op2<< endl;
//                   cout << "to here"<< endl;

	           else
	           {
                    cout << "7" << endl;
	                syntax_error();
	           }
	        }
	        else
	        {
                cout << "8" << endl;
	            syntax_error();
	        }
		}
		else
		{
            cout << "9" << endl;
			syntax_error();
		}
    }
    else
    {
        cout << "10" << endl;
        syntax_error();
    }
}

void Parser::parse_procedure_name()
{
    Token t = lexer.GetToken();
    if (t.token_type == ID || t.token_type == NUM)
    {
        return;
    }
    else
    {
        cout << "11" << endl;
        syntax_error();
    }
}

struct stmt* Parser::parse_procedure_body()
{
    struct stmt*temp;
    Token t = lexer.GetToken();
    if (t.token_type == ID || t.token_type == NUM || t.token_type == INPUT || t.token_type == OUTPUT || t.token_type == DO)
    {
        lexer.UngetToken(t);
        temp = parse_statement_list();
        return temp;
    }
    else
    {
        cout << "12" << endl;
        syntax_error();
    }
}

//struct stmt* begin_stmt;
//int c =0;
struct stmt*Parser:: parse_statement_list()
{
    struct stmt* temp;
    struct stmt*stl;
    Token t = lexer.GetToken();
    if (t.token_type == ID || t.token_type == NUM || t.token_type == INPUT || t.token_type == OUTPUT || t.token_type == DO)
    {
        lexer.UngetToken(t);
        temp = parse_statement();
        Token t = lexer.GetToken();

        if (t.token_type == ID || t.token_type == NUM || t.token_type == INPUT || t.token_type == OUTPUT || t.token_type == DO)
        {
            Token t1 = lexer.GetToken();
            // t1.Print();
            if (t1.token_type == NUM || t1.token_type == END_OF_FILE)
            {
                lexer.UngetToken(t1);
                lexer.UngetToken(t);

                return temp;
            }
            else
            {
                lexer.UngetToken(t1);
                lexer.UngetToken(t);
                stl = parse_statement_list();
                temp->next = stl;
                return temp;
            }
        }
        else if (t.token_type == ENDPROC)
        {
            lexer.UngetToken(t);

            return temp;
        }
        else
        {
            cout << "13" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "14" << endl;
        syntax_error();
    }
}

struct stmt* Parser::parse_statement()
{
    struct stmt* temp;
    Token t = lexer.GetToken();
    // t.Print();
    if (t.token_type == ID)
    {
        Token t1 = lexer.GetToken();
        // t1.Print();
        if (t1.token_type == SEMICOLON)
        {
            lexer.UngetToken(t1);
            lexer.UngetToken(t);
            temp = parse_procedure_invocation();
            return temp;
        }
        else if (t1.token_type == EQUAL)
        {
            lexer.UngetToken(t1);
            lexer.UngetToken(t);
            temp = parse_assign_statement();
            return temp;

        }
        else
        {
            cout << "15" << endl;
            syntax_error();
        }
    }
    else if (t.token_type == NUM)
    {
       lexer.UngetToken(t);
       temp = parse_procedure_invocation();
       return temp;
    }
    else if (t.token_type == INPUT)
    {
        lexer.UngetToken(t);
        temp = parse_input_statement();
        return temp;
    }
    else if (t.token_type == OUTPUT)
    {
        lexer.UngetToken(t);
        temp = parse_output_statement();
        return temp;
    }
    else if (t.token_type == DO)
    {
        lexer.UngetToken(t);
        temp = parse_do_statement();
        return temp;
    }
    else 
    {
        cout << "16" << endl;
        syntax_error();
    }
}

struct stmt* Parser:: parse_input_statement()
{
    struct stmt* st = new stmt;
    Token t = lexer.GetToken();
    if (t.token_type == INPUT)
    {
        Token t1 = lexer.GetToken();
        if (t1.token_type == ID)
        {
            Token t2 = lexer.GetToken();
            if (t2.token_type == SEMICOLON)
            {
                st->statement_type=1;
                st->LHS = -1;
                st->operator_type = 5;
                if(map_ID.count(t1.lexeme) == 0)
                {
                    map_ID.insert(make_pair(t1.lexeme, i));
                    i++;
                    st->op1=map_ID[t1.lexeme];
                }
                else
                {
                    st->op1=map_ID[t1.lexeme];
                }
                st->op2=-1;
                return st;
            }
            else 
            {
                cout << "17" << endl;
                syntax_error();
            }
        }
        else
        {
            cout << "18" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "19" << endl;
        syntax_error();
    }
}

struct stmt* Parser::parse_output_statement()
{
    struct stmt* st = new stmt;
    Token t = lexer.GetToken();
    if (t.token_type == OUTPUT)
    {
        Token t1 = lexer.GetToken();
        if (t1.token_type == ID)
        {
            Token t2 = lexer.GetToken();
            if (t2.token_type == SEMICOLON)
            {
                st->statement_type=2;
                st->LHS = -1;
                st->operator_type = 5;
                if(map_ID.count(t1.lexeme) == 0)
                {
                    map_ID.insert(make_pair(t1.lexeme, i));
                    i++;
                    st->op1=map_ID[t1.lexeme];
                }
                else
                {
                    st->op1=map_ID[t1.lexeme];
                }
                st->op2=-1;
                return st;
            }
            else 
            {
                cout << "20" << endl;
                syntax_error();
            }
        }
        else
        {
            cout << "21" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "22" << endl;
        syntax_error();
    }
}

struct stmt* Parser::parse_procedure_invocation()
{
    struct stmt* st = new stmt;
    Token t = lexer.GetToken();
    if (t.token_type == ID || t.token_type == NUM)
    {
        lexer.UngetToken(t);
        parse_procedure_name();
        Token t1 = lexer.GetToken();
        if (t1.token_type == SEMICOLON)
        {
            st->statement_type=3;
            st->proc_new_code = t.lexeme;
            return st;
        }
        else
        {
            cout << "23" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "24" << endl;
        syntax_error();
    }
}

struct stmt* Parser::parse_do_statement()
{
    struct stmt* st = new stmt;
    Token t = lexer.GetToken();
    if (t.token_type == DO)
    {
        Token t1 = lexer.GetToken();
        if (t1.token_type == ID)
        {
            Token t2 = lexer.GetToken();
            if (t2.token_type == ID || t2.token_type == NUM)
            {
                lexer.UngetToken(t2);
                parse_procedure_invocation();
                st->statement_type=4;
                st->proc_new_code=t2.lexeme;
                st->LHS = map_ID[t1.lexeme];
                return st;
            }
            else
            {
                cout << "25" << endl;
                syntax_error();
            }
        }
        else
        {
            cout << "26" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "27" << endl;
        syntax_error();
    }
}



struct stmt* Parser::parse_assign_statement()
{
    struct stmt*st = new stmt;
    Token t = lexer.GetToken();
    if (t.token_type == ID)
    {
        Token t1 = lexer.GetToken();
        if (t1.token_type == EQUAL)
        {
            Token t2 = lexer.GetToken();
            if (t2.token_type == ID || t2.token_type == NUM)
            {
                // lexer.UngetToken(t2);
                // parse_expr();
                Token t3 = lexer.GetToken();
                if (t3.token_type == PLUS || t3.token_type == MINUS || t3.token_type == DIV || t3.token_type == MULT)
                {
                    Token t4 = lexer.GetToken();
                    if (t4.token_type == ID || t4.token_type == NUM)
                    {
                        Token t5 = lexer.GetToken();
                        if (t5.token_type == SEMICOLON)
                        {
                            st->statement_type=5;
                            if(map_ID.count(t.lexeme) == 0)
                            {
                                map_ID.insert(make_pair(t.lexeme, i));
                                st->LHS=map_ID[t.lexeme];
                                mem[i]=0;
                                i++;
                            }
                            else
                            {
                                st->LHS=map_ID[t.lexeme];
                            }
                            switch (t3.token_type)
                            {
                                case PLUS:
                                    st->operator_type =1;
                                    break;
                                case MINUS:
                                    st->operator_type =2;
                                    break;
                                case MULT:
                                    st->operator_type =3;
                                    break;
                                case DIV:
                                    st->operator_type =4;
                                    break;
                            }
                            if (t2.token_type ==ID)
                            {
                                if(map_ID.count(t2.lexeme) == 0)
                                {
                                    map_ID.insert(make_pair(t2.lexeme, i));
                                    st->op1=i;
                                    mem[i] = 0;
                                    i++;
                                }
                                else
                                {
                                    st->op1=map_ID[t2.lexeme];
                                }
                            }
                            else
                            {
                                st->op1=i;
                                mem[i] = stoi(t2.lexeme);
                                i++;
                            }

                            if (t4.token_type ==ID)
                            {
                                if(map_ID.count(t4.lexeme) == 0)
                                {
                                    map_ID.insert(make_pair(t4.lexeme, i));
                                    st->op2=i;
                                    mem[i] = 0;
                                    i++;
                                }
                                else
                                {
                                    st->op2=map_ID[t4.lexeme];
                                }
                            }
                            else
                            {
                                st->op2=i;
                                mem[i] = stoi(t4.lexeme);
                                i++;
                            }

                            return st;
                        }
                        else
                        {
                            syntax_error();
                        }
                    }
                    else
                    {
                        syntax_error();
                    }

                }
                else if (t3.token_type == SEMICOLON)
                {
                    st->statement_type=5;
                    st->operator_type=0;
                    if(map_ID.count(t.lexeme) == 0)
                    {
                        map_ID.insert(make_pair(t.lexeme, i));
                        st->LHS=map_ID[t.lexeme];
                        mem[i]=0;
                        i++;
                    }
                    else
                    {
                        st->LHS=map_ID[t.lexeme];
                    }
                    if (t2.token_type ==ID)
                    {
                        if(map_ID.count(t2.lexeme) == 0)
                        {
                            map_ID.insert(make_pair(t2.lexeme, i));
                            st->op1=i;
                            mem[i] = 0;
                            i++;
                        }
                        else
                        {
                            st->op1=map_ID[t2.lexeme];
                        }
                    }
                    else
                    {
                        st->op1=i;
                        mem[i] = stoi(t2.lexeme);
                        i++;
                    }
                    st->op2 = -1;
                    return st;
                }
                else
                {
                    cout << "28" << endl;
                    syntax_error();
                }
            }
            else
            {
                cout << "29" << endl;
                syntax_error();
            }
        }
        else
        {
            cout << "30" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "31" << endl;
        syntax_error();
    }
}

void Parser::parse_expr()
{
    Token t = lexer.GetToken();
    if (t.token_type == ID || t.token_type == NUM)
    {
        lexer.UngetToken(t);
        parse_primary();
        Token t = lexer.GetToken();
        if (t.token_type == END_OF_FILE)
        {
            return;
        }
        else if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == DIV || t.token_type == MULT)
        {
            lexer.UngetToken(t);
            parse_operator();
            Token t = lexer.GetToken();
            if (t.token_type == ID || t.token_type == NUM)
            {
                lexer.UngetToken(t);
                parse_primary();
            }
            else
            {
                cout << "32" << endl;
                syntax_error();
            }
        }
        else
        {
            cout << "33" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "34" << endl;
        syntax_error();
    }
}

void Parser::parse_operator()
{
    Token t = lexer.GetToken();
    if (t.token_type == PLUS)
    {
        return;
    }
    else if (t.token_type == MINUS)
    {
        return;
    }
    else if (t.token_type == MULT)
    {
        return;
    }
    else if (t.token_type == DIV)
    {
        return;
    }
    else
    {
        cout << "35" << endl;
        syntax_error();
    }
}

void Parser::parse_primary()
{
    Token t = lexer.GetToken();
    if (t.token_type == ID)
    {
        return;
    }
    else if (t.token_type == NUM)
    {
        return;
    }
    else
    {
        cout << "36" << endl;
        syntax_error();
    }
}


struct stmt* start;

void Parser::parse_main()
{
    struct stmt *temp;
    Token t = lexer.GetToken();

    if (t.token_type == MAIN)
    {
        temp = parse_procedure_body();
//        cout<< "from here" << endl;
//        cout<< temp->statement_type << endl;
//        cout<< temp->LHS<< endl;
//        cout<< temp->operator_type<<endl;
//        cout<< temp->op1<< endl;
//        cout<< temp->op2<< endl;
//        cout << "to here"<< endl;
        start = temp;
        return;

    }
    else
    {
        cout << "37" << endl;
        syntax_error();
    }
}



void Parser::parse_inputs()
{
    Token t = lexer.GetToken();
//    cout << t.lexeme << endl;
    if (t.token_type == NUM)
    {
        Token t1 = peek();
        if (t1.token_type == END_OF_FILE)
        {
            inputs[next_input]=stoi(t.lexeme);
            return;
        }
        else if (t1.token_type == NUM)
        {
            inputs[next_input++]=stoi(t.lexeme);
            parse_inputs();
        }
        else
        {
            cout << "38" << endl;
            syntax_error();
        }
    }
    else
    {
        cout << "39" << endl;
        syntax_error();
    }
}

int Parser::execute_program(struct stmt* start)
{
    struct stmt* pc;
//    struct proc_table* proc_struct;
    pc = start;
    while (pc!=NULL)
    {
        switch(pc->statement_type)
        {
            case 1:
                mem[pc->op1] = inputs[next_input];
                next_input++;
                break;
            case 2:
                cout << mem[pc->op1] << " ";
                break;
            case 3:
                execute_program(proc_map[pc->proc_new_code]);
                break;
            case 4:
                int count;
                count = mem[pc->LHS];
                while(count)
                {
                    execute_program(proc_map[pc->proc_new_code]);
                    count--;
                }
                break;
            case 5:
                switch (pc->operator_type)
                {
                    case 0:
                        mem[pc->LHS] = mem[pc->op1];
                    case 1:
                        mem[pc->LHS] = mem[pc->op1] + mem[pc->op2];
                        break;
                    case 2:
                        mem[pc->LHS] = mem[pc->op1] - mem[pc->op2];
                        break;
                    case 3:
                        mem[pc->LHS] = mem[pc->op1] * mem[pc->op2];
                        break;
                    case 4:
                        mem[pc->LHS] = mem[pc->op1] / mem[pc->op2];
                        break;
                }
                break;
        }
        pc = pc->next;
    }
    return 0;
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;
    
    // token = lexer.GetToken();
    // token.Print();
    Parser *p = new Parser;
    p->parse_input();
    next_input = 0;
    p->execute_program(start);

//    cout << "inputs"<<  endl;
//    for (j=0; j<=5; j++)
//    {
//        cout << mem[j] << " ";
//    }


//    while(start)
//    {
//        cout<<"this is the main start" <<endl;
//        cout<< start->statement_type << endl;
//        cout<< start->LHS<< endl;
//        cout<< start->operator_type<<endl;
//        cout<< start->op1<< endl;
//        cout<< start->op2<< endl;
//        start = start->next;
//    }


}
