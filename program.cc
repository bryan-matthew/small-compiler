#include "compiler.h"

#include "program.h"

#include <iostream>

#include <cstdio>

#include <cstdlib>
#include <string>
#include <vector>

#include <algorithm>

using namespace std;

vector<string> variables;

LexicalAnalyzer lexer;

void Parser::syntax_error()

{

    cout << "Syntax Error\n"
         << endl;

    exit(1);
}

Token Parser::peek()

{

    Token token = lexer.GetToken();

    lexer.UngetToken(token);

    return token;
}

bool checkList(vector<string> list, string str, int &index)
{
    vector<string> temp_list;
    temp_list = list;
    unsigned i = 0;
    while (i < temp_list.size())
    {
        if (temp_list[i].compare(str) == 0)
        {
            index = i;
            return true;
        }
        i++;
    }
    return false;
}

bool exists(vector<string> list, string check)

{
    int index = 0;
    return checkList(list, check, index);
}

int location(string find)

{
    int index = 0;
    if (checkList(variables, find, index))
    {
        return index;
    }
    else
    {
        exit(1);
    }
}

bool check_for_case(string case_num)

{
    int index = 0;

    bool check = false;
    return checkList(variables, case_num, index);
}

struct InstructionNode *parse_generate_intermediate_representation()

{

    struct InstructionNode *program = new InstructionNode();
    Parser parser;

    program = parser.parse_program();

    return program;
}

struct InstructionNode *Parser::parse_program()

{

    struct InstructionNode *body = new InstructionNode;

    parse_var_section();

    body = parse_body();

    parse_inputs();

    return body;
}

void Parser::parse_var_section()

{

    variables = parse_id_list();
    int i = 0;
    while (i < variables.size())
    {
        mem[next_available] = 0;
        next_available++;
        i++;
    }

    Token t = lexer.GetToken();

    // if(t.token_type == SEMICOLON) {
    // cout << "Semicolon check";
    // }

    if (t.token_type != SEMICOLON)

    {

        syntax_error();
    }
}

vector<string> Parser::parse_id_list()

{

    Token t = lexer.GetToken();

    vector<string> list;

    vector<string> temp;

    if (!exists(list, t.lexeme))

    {

        list.push_back(t.lexeme);
    }

    if (t.token_type != ID)

    {

        syntax_error();
    }

    t = peek();

    if (t.token_type == COMMA)

    {

        t = lexer.GetToken();

        temp = parse_id_list();

        for (int i = 0; i < list.size(); i++)

        {

            for (int j = 0; j < temp.size(); j++)

            {

                if (!exists(list, temp[j]))

                {

                    list.push_back(temp[j]);
                }
            }
        }

        return list;
    }

    else if (t.token_type == SEMICOLON)

    {

        return list;
    }

    else

    {

        syntax_error();
    }
}

bool Parser::checkToken(Token myToken, TokenType expectedToken)
{
    if (myToken.token_type == expectedToken)
    {
        return true;
    }
    return false;
}

struct InstructionNode *Parser::parse_body()

{

    struct InstructionNode *instl;

    Token t = lexer.GetToken();
    if (checkToken(t, LBRACE) == false)
    {
        syntax_error();
    }

    instl = parse_stmt_list();
    t = lexer.GetToken();
    if (checkToken(t, RBRACE) == false)
    {
        syntax_error();
    }

    return instl;
}

struct InstructionNode *Parser::parse_stmt_list()

{

    struct InstructionNode *instl1;

    struct InstructionNode *instl2;

    instl1 = parse_stmt();

    Token t = peek();

    if (t.token_type == ID || t.token_type == SWITCH ||
        t.token_type == WHILE || t.token_type == INPUT ||
        t.token_type == OUTPUT ||

        t.token_type == FOR || t.token_type == IF)

    {

        instl2 = parse_stmt_list();

        struct InstructionNode *temp = instl1;

        while (temp->next)

        {

            temp = temp->next;
        }

        temp->next = instl2;

        return instl1;
    }

    else if (t.token_type == RBRACE)

    {

        return instl1;
    }

    else

    {

        syntax_error();
    }
    return nullptr;
}

struct InstructionNode *Parser::parse_stmt()

{

    struct InstructionNode *inst = new InstructionNode;

    Token t = peek();

    switch (t.token_type)
    {
    case ID:
        inst = parse_assign_stmt();
        break;
    case IF:
        inst = parse_if_stmt();
        break;
    case FOR:
        inst = parse_for_stmt();
        break;
    case SWITCH:
        inst = parse_switch_stmt();
        break;
    case WHILE:
        inst = parse_while_stmt();
        break;
    case INPUT:
        inst = parse_input_stmt();
        break;
    case OUTPUT:
        inst = parse_output_stmt();
        break;
    default:
        syntax_error();
        break;
    }

    return inst;
}

struct InstructionNode *Parser::parse_assign_stmt()

{

    struct InstructionNode *inst = new InstructionNode;

    struct InstructionNode *temp = new InstructionNode;

    inst->type = ASSIGN;

    Token t = lexer.GetToken();

    if (t.token_type != ID)

    {

        syntax_error();
    }

    string to_store = t.lexeme;

    inst->assign_inst.left_hand_side_index = location(to_store);

    t = lexer.GetToken();

    if (t.token_type != EQUAL)

    {

        syntax_error();
    }

    Token t1 = lexer.GetToken();

    Token t2 = lexer.GetToken();

    if (t1.token_type == NUM || t1.token_type == ID)

    {

        if (t2.token_type == PLUS || t2.token_type == MINUS

            || t2.token_type == MULT || t2.token_type == DIV)

        {

            lexer.UngetToken(t2);

            lexer.UngetToken(t1);

            temp = parse_expr();

            inst->assign_inst.op = temp->assign_inst.op;

            inst->assign_inst.operand1_index = temp->assign_inst.operand1_index;

            inst->assign_inst.operand2_index = temp->assign_inst.operand2_index;
        }

        else if (t2.token_type == SEMICOLON)

        {

            lexer.UngetToken(t2);

            lexer.UngetToken(t1);

            temp = parse_primary();

            inst->assign_inst.operand1_index = temp->assign_inst.operand1_index;

            inst->assign_inst.op = OPERATOR_NONE;
        }

        else

        {

            syntax_error();
        }
    }

    t = lexer.GetToken();

    if (t.token_type != SEMICOLON)

    {

        syntax_error();
    }

    inst->next = nullptr;

    return inst;
}

struct InstructionNode *Parser::parse_expr()

{

    struct InstructionNode *inst = new InstructionNode;

    struct InstructionNode *temp = new InstructionNode;

    temp = parse_primary();

    inst->assign_inst.operand1_index = temp->assign_inst.operand1_index;

    int op = parse_op();

    switch (op)
    {
    case OPERATOR_PLUS:
        inst->assign_inst.op = OPERATOR_PLUS;
        break;
    case OPERATOR_MINUS:
        inst->assign_inst.op = OPERATOR_MINUS;
        break;
    case OPERATOR_DIV:
        inst->assign_inst.op = OPERATOR_DIV;
        break;
    case OPERATOR_MULT:
        inst->assign_inst.op = OPERATOR_MULT;
        break;
    default:
        inst->assign_inst.op = OPERATOR_NONE;
        break;
    }

    temp = parse_primary();

    inst->assign_inst.operand2_index = temp->assign_inst.operand1_index;

    return inst;
}

struct InstructionNode *Parser::parse_primary()

{

    struct InstructionNode *temp = new InstructionNode;

    Token t = lexer.GetToken();

    if (checkToken(t, ID) == true)
    {
        temp->assign_inst.operand1_index = location(t.lexeme);
    }

    else if (checkToken(t, NUM) == true)

    {

        if (!exists(variables, t.lexeme))

        {

            variables.push_back(t.lexeme);

            mem[next_available] = stoi(t.lexeme);

            next_available++;
        }

        temp->assign_inst.operand1_index = location(t.lexeme);
    }

    else

    {

        syntax_error();
    }

    return temp;
}

int Parser::parse_op()

{

    Token t = lexer.GetToken();

    switch (t.token_type)
    {
    case PLUS:
        return OPERATOR_PLUS;
        break;
    case MINUS:
        return OPERATOR_MINUS;
        break;
    case MULT:
        return OPERATOR_MULT;
        break;
    case DIV:
        return OPERATOR_DIV;
        break;
    default:
        syntax_error();
        break;
    }
    return 0;
}

struct InstructionNode *Parser::parse_output_stmt()

{

    struct InstructionNode *inst = new InstructionNode;

    inst->type = OUT;

    Token t = lexer.GetToken();

    // could be simplified to Or??
    if (checkToken(t, OUTPUT) == false)
    {
        syntax_error();
    }

    t = lexer.GetToken();

    if (checkToken(t, ID) == false)

    {

        syntax_error();
    }

    inst->output_inst.var_index = location(t.lexeme);

    t = lexer.GetToken();

    if (checkToken(t, SEMICOLON) == false)

    {

        syntax_error();
    }

    inst->next = nullptr;

    return inst;
}

struct InstructionNode *Parser::parse_input_stmt()

{

    struct InstructionNode *inst = new InstructionNode;

    inst->type = IN;

    Token t = lexer.GetToken();

    if (checkToken(t, INPUT) == false)

    {

        syntax_error();
    }

    t = lexer.GetToken();
    if (checkToken(t, ID) == false)

    {

        syntax_error();
    }

    inst->input_inst.var_index = location(t.lexeme);

    t = lexer.GetToken();

    if (checkToken(t, SEMICOLON) == false)

    {

        syntax_error();
    }

    inst->next = nullptr;

    return inst;
}

struct InstructionNode *Parser::parse_while_stmt()

{

    struct InstructionNode *inst = new InstructionNode;

    struct InstructionNode *temp = new InstructionNode;

    Token t = lexer.GetToken();

    if (checkToken(t, WHILE) == false)

    {

        syntax_error();
    }

    inst->type = CJMP;

    temp = parse_condition();

    inst->cjmp_inst.condition_op = temp->cjmp_inst.condition_op;

    inst->cjmp_inst.operand1_index = temp->cjmp_inst.operand1_index;

    inst->cjmp_inst.operand2_index = temp->cjmp_inst.operand2_index;

    inst->next = parse_body();

    struct InstructionNode *jmpNode = new InstructionNode;

    jmpNode->type = JMP;

    jmpNode->next = nullptr;

    jmpNode->jmp_inst.target = inst;

    temp = inst;

    while (temp->next)

    {

        temp = temp->next;
    }

    temp->next = jmpNode;

    struct InstructionNode *noopNode = new InstructionNode;

    noopNode->type = NOOP;

    noopNode->next = nullptr;

    jmpNode->next = noopNode;

    inst->cjmp_inst.target = noopNode;

    return inst;
}

struct InstructionNode *Parser::parse_if_stmt()

{

    struct InstructionNode *inst = new InstructionNode;

    struct InstructionNode *temp = new InstructionNode;

    Token t = lexer.GetToken();

    if (checkToken(t, IF) == false)

    {

        syntax_error();
    }

    inst->type = CJMP;

    temp = parse_condition();

    inst->cjmp_inst.condition_op = temp->cjmp_inst.condition_op;

    inst->cjmp_inst.operand1_index = temp->cjmp_inst.operand1_index;

    inst->cjmp_inst.operand2_index = temp->cjmp_inst.operand2_index;

    inst->next = parse_body();

    struct InstructionNode *noopNode = new InstructionNode;

    noopNode->next = nullptr;

    noopNode->type = NOOP;

    temp = inst;

    while (temp->next != nullptr)

    {

        temp = temp->next;
    }

    temp->next = noopNode;

    inst->cjmp_inst.target = noopNode;

    return inst;
}

struct InstructionNode *Parser::parse_condition()

{

    struct InstructionNode *inst = new InstructionNode;

    struct InstructionNode *temp = new InstructionNode;

    inst->type = CJMP;

    temp = parse_primary();

    inst->cjmp_inst.operand1_index = temp->cjmp_inst.operand1_index;

    temp = parse_relop();

    inst->cjmp_inst.condition_op = temp->cjmp_inst.condition_op;

    temp = parse_primary();

    inst->cjmp_inst.operand2_index = temp->cjmp_inst.operand1_index;

    return inst;
}

struct InstructionNode *Parser::parse_relop()

{

    struct InstructionNode *inst = new InstructionNode;

    Token t = lexer.GetToken();
    if (checkToken(t, NOTEQUAL) == true)

    {

        inst->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    }
    else if (checkToken(t, GREATER) == true)

    {

        inst->cjmp_inst.condition_op = CONDITION_GREATER;
    }

    else if (checkToken(t, LESS) == true)
    {
        inst->cjmp_inst.condition_op = CONDITION_LESS;
    }
    else
    {
        syntax_error();
    }

    return inst;
}

struct InstructionNode *Parser::parse_switch_stmt()

{

    struct InstructionNode *inst = new InstructionNode;

    struct InstructionNode *body = new InstructionNode;

    struct InstructionNode *noopNode = new InstructionNode;

    struct InstructionNode *defaultCase = new InstructionNode;

    struct InstructionNode *temp = new InstructionNode;

    struct InstructionNode *temp1 = new InstructionNode;

    int variable;

    inst->type = CJMP;

    inst->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

    noopNode->type = NOOP;

    noopNode->next = nullptr;

    Token t = lexer.GetToken();

    if (checkToken(t, SWITCH) == false)

    {
        syntax_error();
    }

    t = lexer.GetToken();

    if (checkToken(t, ID) == false)

    {
        syntax_error();
    }

    variable = location(t.lexeme);

    inst->cjmp_inst.operand1_index = variable;

    t = lexer.GetToken();

    if (checkToken(t, LBRACE) == false)

    {
        syntax_error();
    }

    body = parse_case_list();

    temp = body;

    while (temp->next)

    {

        temp->cjmp_inst.operand1_index = variable;

        temp->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

        temp1 = temp->cjmp_inst.target;

        while (temp1->next)

        {
            temp1 = temp1->next;
        }

        temp1->jmp_inst.target = noopNode;

        temp = temp->next;
    }

    temp->cjmp_inst.operand1_index = variable;

    temp->cjmp_inst.condition_op = CONDITION_NOTEQUAL;

    temp1 = temp->cjmp_inst.target;

    while (temp1->next)

    {

        temp1 = temp1->next;
    }

    temp1->jmp_inst.target = noopNode;

    inst = body;

    t = peek();

    if (checkToken(t, LBRAC) == true)

    {
        t = lexer.GetToken();

        return inst;
    }

    else if (checkToken(t, DEFAULT) == true)

    {

        defaultCase = parse_default_case();

        temp = inst;

        while (temp->next)

        {

            temp = temp->next;
        }

        temp1 = temp->cjmp_inst.target;

        while (temp1->next)

        {

            temp1 = temp1->next;
        }

        temp1->next = noopNode;

        t = lexer.GetToken();

        if (checkToken(t, RBRACE))

        {

            syntax_error();
        }

        return inst;
    }

    return inst;
}

struct InstructionNode *Parser::parse_case_list()

{

    struct InstructionNode *inst = new InstructionNode;

    struct InstructionNode *caseNode = new InstructionNode;

    inst = parse_case();

    Token t = peek();

    if (checkToken(t, CASE) == true)

    {

        caseNode = parse_case_list();

        inst->next = caseNode;

        return inst;
    }

    else if (checkToken(t, RBRACE) == true)

    {

        inst->next = nullptr;

        return inst;
    }
    return nullptr;
}

struct InstructionNode *Parser::parse_case()

{

    struct InstructionNode *body = new InstructionNode;

    struct InstructionNode *inst = new InstructionNode;

    struct InstructionNode *jmpNode = new InstructionNode;

    struct InstructionNode *temp = new InstructionNode;

    inst->type = CJMP;

    inst->next = nullptr;

    jmpNode->type = JMP;

    jmpNode->jmp_inst.target = nullptr;

    jmpNode->next = nullptr;

    Token t = lexer.GetToken();

    if (checkToken(t, CASE) == false)

    {

        syntax_error();
    }

    t = lexer.GetToken();

    if (checkToken(t, NUM) == false)

    {

        syntax_error();
    }

    if (check_for_case(t.lexeme))

    {

        inst->cjmp_inst.operand2_index = location(t.lexeme);
    }

    else

    {

        inst->cjmp_inst.operand2_index = 1000;
    }

    t = lexer.GetToken();

    if (checkToken(t, COLON) == false)

    {
        syntax_error();
    }
    body = parse_body();

    temp = body;
    while (temp->next)
    {
        temp = temp->next;
    }

    temp->next = jmpNode;

    inst->cjmp_inst.target = body;

    return inst;
}

struct InstructionNode *Parser::parse_default_case()
{
    struct InstructionNode *body = new InstructionNode;

    Token t = lexer.GetToken();
    if (checkToken(t, DEFAULT) == false)

    {
        syntax_error();
    }

    t = lexer.GetToken();

    if (checkToken(t, COLON) == false)

    {

        syntax_error();
    }

    body = parse_body();
    return body;
}

struct InstructionNode *Parser::parse_for_stmt()
{
    struct InstructionNode *inst = new InstructionNode;

    struct InstructionNode *temp = new InstructionNode;

    struct InstructionNode *condition = new InstructionNode;

    struct InstructionNode *assignment1 = new InstructionNode;

    struct InstructionNode *assignment2 = new InstructionNode;

    struct InstructionNode *body = new InstructionNode;

    struct InstructionNode *noopNode = new InstructionNode;

    struct InstructionNode *jmpNode = new InstructionNode;

    noopNode->next = nullptr;
    noopNode->type = NOOP;
    jmpNode->type = JMP;
    Token t = lexer.GetToken();

    if (checkToken(t, FOR) == false)
    {
        syntax_error();
    }

    t = lexer.GetToken();

    if (checkToken(t, LPAREN) == false)

    {
        syntax_error();
    }

    assignment1 = parse_assign_stmt();
    condition = parse_condition();
    condition->type = CJMP;
    t = lexer.GetToken();

    if (checkToken(t, SEMICOLON) == false)
    {
        syntax_error();
    }

    assignment2 = parse_assign_stmt();
    assignment2->type = ASSIGN;
    t = lexer.GetToken();
    if (checkToken(t, RPAREN) == false)
    {
        syntax_error();
    }

    body = parse_body();

    temp = body;

    while (temp->next)
    {
        temp = temp->next;
    }

    temp->next = assignment2;

    condition->cjmp_inst.target = noopNode;

    condition->next = body;

    assignment2->next = jmpNode;

    jmpNode->jmp_inst.target = condition;

    jmpNode->next = noopNode;

    inst = assignment1;

    inst->next = condition;

    return inst;
}

void Parser::parse_inputs()
{
    parse_num_list();
}

void Parser::parse_num_list()
{
    Token t = lexer.GetToken();

    if (checkToken(t, NUM) == false)

    {
        syntax_error();
    }

    inputs.push_back(stoi(t.lexeme));

    t = peek();

    if (checkToken(t, NUM) == true)

    {
        parse_num_list();
    }

    else if (checkToken(t, END_OF_FILE) == true)

    {
        return;
    }
}
