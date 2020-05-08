#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <iostream>
#include "compiler.h"

struct StatementNode * parse_generate_intermediate_representation()
{
    // Sample program for demonstration purpose only
    // Replace the following with a parser that reads the program from stdin &
    // creates appropriate data structures to be executed by execute_program()
    // This is the imaginary input for the following construction:
//     a, b;
//     {
//        a = 10;
//        b = 1;
//        WHILE a > 0
//        {
//            b = b * a;
//            a = a - 1;
//        }
//        print b;
//     }

    struct ValueNode * A = new ValueNode;
    struct ValueNode * B = new ValueNode;
    struct ValueNode * ONE = new ValueNode;
    struct ValueNode * TWO = new ValueNode;
    struct ValueNode * NINE = new ValueNode;
    struct ValueNode * TEN = new ValueNode;
    struct ValueNode * ZERO = new ValueNode;
    struct StatementNode * s1 = new StatementNode;
    struct StatementNode * s2 = new StatementNode;
    struct StatementNode * s3 = new StatementNode;
    struct StatementNode * s4 = new StatementNode;
    struct StatementNode * s5 = new StatementNode;
    struct StatementNode * s6 = new StatementNode;
    struct StatementNode * s7 = new StatementNode;
    struct StatementNode * s8 = new StatementNode;
    struct StatementNode * s9 = new StatementNode;
    struct StatementNode * s10 = new StatementNode;
    struct StatementNode * s11 = new StatementNode;
    struct StatementNode * s12 = new StatementNode;
    struct StatementNode * s13 = new StatementNode;
    struct StatementNode * s20 = new StatementNode;
    struct StatementNode * s21 = new StatementNode;
    struct StatementNode * s22 = new StatementNode;
    struct StatementNode * s23 = new StatementNode;

    A->name = "a";      A->value = 0;
    B->name = "b";      B->value = 0;
    ONE->name = "";     ONE->value = 1;
    TWO->name = "";     TWO->value = 2;
    NINE->name = "";     NINE->value = 9;
    TEN->name = "";     TEN->value = 10;
    ZERO->name = "";    ZERO->value = 0;

    s1->type = ASSIGN_STMT;
    s1->assign_stmt = new AssignmentStatement;
    s1->assign_stmt->left_hand_side = A;
    s1->assign_stmt->op = OPERATOR_NONE;
    s1->assign_stmt->operand1 = TEN;
    s1->assign_stmt->operand2 = NULL;
    s1->next = s2;

    s2->type = ASSIGN_STMT;
    s2->assign_stmt = new AssignmentStatement;
    s2->assign_stmt->left_hand_side = B;
    s2->assign_stmt->op = OPERATOR_NONE;
    s2->assign_stmt->operand1 = ONE;
    s2->assign_stmt->operand2 = NULL;
    s2->next = s3;

    s3->type = IF_STMT;
    s3->if_stmt = new IfStatement;
    s3->if_stmt->condition_op = CONDITION_NOTEQUAL;
    s3->if_stmt->condition_operand1 = A;
    s3->if_stmt->condition_operand2 = ONE;
    s3->if_stmt->true_branch = s20;
    s3->if_stmt->false_branch = s4;
    s3->next = s20;

    s4->type = PRINT_STMT;
    s4->print_stmt = new PrintStatement;
    s4->print_stmt->id = B;
    s4->next = s20;

//    s8->type = GOTO_STMT;
//    s8->goto_stmt = new GotoStatement;
//    s8->goto_stmt->target = s7;    // Jump to the if statement
//    s8->next = s7;

    s20->type = NOOP_STMT;
    s20->next = s5;

    s5->type = IF_STMT;
    s5->if_stmt = new IfStatement;
    s5->if_stmt->condition_op = CONDITION_NOTEQUAL;
    s5->if_stmt->condition_operand1 = A;
    s5->if_stmt->condition_operand2 = NINE;
    s5->if_stmt->true_branch = s7;
    s5->if_stmt->false_branch = s6;
    s5->next = s7;

    s6->type = PRINT_STMT;
    s6->print_stmt = new PrintStatement;
    s6->print_stmt->id = B;
    s6->next = s7;

//    s10->type = GOTO_STMT;
//    s10->goto_stmt = new GotoStatement;
//    s10->goto_stmt->target = s7;    // Jump to the if statement
//    s10->next = s7;

//    s6->type = ASSIGN_STMT;
//    s6->assign_stmt = new AssignmentStatement;
//    s6->assign_stmt->left_hand_side = A;
//    s6->assign_stmt->op = OPERATOR_PLUS;
//    s6->assign_stmt->operand1 = A;
//    s6->assign_stmt->operand2 = ONE;
//    s6->next = s8;

//    s21->type = NOOP_STMT;
//    s21->next = s9;

//    s9->type = IF_STMT;
//    s9->if_stmt = new IfStatement;
//    s9->if_stmt->condition_op = CONDITION_NOTEQUAL;
//    s9->if_stmt->condition_operand1 = A;
//    s9->if_stmt->condition_operand2 = TWO;
//    s9->if_stmt->true_branch = s22;
//    s9->if_stmt->false_branch = s10;
//    s9->next = s22;
//
//    s10->type = ASSIGN_STMT;
//    s10->assign_stmt = new AssignmentStatement;
//    s10->assign_stmt->left_hand_side = A;
//    s10->assign_stmt->op = OPERATOR_NONE;
//    s10->assign_stmt->operand1 = TWO;
//    s10->assign_stmt->operand2 = NULL;
//    s10->next = s8;
//
//    s22->type = NOOP_STMT;
//    s22->next = s12;
//
//    s12->type = IF_STMT;
//    s12->if_stmt = new IfStatement;
//    s12->if_stmt->condition_op = CONDITION_NOTEQUAL;
//    s12->if_stmt->condition_operand1 = ONE;
//    s12->if_stmt->condition_operand2 = ONE;
//    s12->if_stmt->true_branch = s23;
//    s12->if_stmt->false_branch = s11;
//    s12->next = s23;
//
//
//    s11->type = PRINT_STMT;
//    s11->print_stmt = new PrintStatement;
//    s11->print_stmt->id = A;
//    s11->next = s8;
//
//    s23->type = NOOP_STMT;
//    s23->next = s7;



    s7->type = NOOP_STMT;
    s7->next = s13;

    s13->type = PRINT_STMT;
    s13->print_stmt = new PrintStatement;
    s13->print_stmt->id = B;
    s13->next = NULL;



//    s3->type = IF_STMT;
//    s3->if_stmt = new IfStatement;
//    s3->if_stmt->condition_op = CONDITION_GREATER;
//    s3->if_stmt->condition_operand1 = A;
//    s3->if_stmt->condition_operand2 = ZERO;
//    s3->if_stmt->true_branch = s4;
//    s3->if_stmt->false_branch = s7;
//    s3->next = s7;
//
//    s4->type = ASSIGN_STMT;
//    s4->assign_stmt = new AssignmentStatement;
//    s4->assign_stmt->left_hand_side = B;
//    s4->assign_stmt->op = OPERATOR_MULT;
//    s4->assign_stmt->operand1 = B;
//    s4->assign_stmt->operand2 = A;
//    s4->next = s5;
//
//    s5->type = ASSIGN_STMT;
//    s5->assign_stmt = new AssignmentStatement;
//    s5->assign_stmt->left_hand_side = A;
//    s5->assign_stmt->op = OPERATOR_MINUS;
//    s5->assign_stmt->operand1 = A;
//    s5->assign_stmt->operand2 = ONE;
//    s5->next = s6;
//
//    s6->type = GOTO_STMT;
//    s6->goto_stmt = new GotoStatement;
//    s6->goto_stmt->target = s3;    // Jump to the if statement
//    s6->next = s7;
//
//    s7->type = NOOP_STMT;
//    s7->next = s8;
//
//    s8->type = PRINT_STMT;
//    s8->print_stmt = new PrintStatement;
//    s8->print_stmt->id = B;
//    s8->next = NULL;

    return s1;
}
