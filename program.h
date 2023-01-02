#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include "lexer.h"
#include "compiler.h"
using namespace std;



class Parser
{
    private:

        LexicalAnalyzer lexer;
        Token peek();

    public:
        void syntax_error();

        struct InstructionNode* parse_program();

        void parse_var_section();

        vector<string> parse_id_list();

        struct InstructionNode* parse_body();

        struct InstructionNode* parse_stmt_list();

        struct InstructionNode* parse_stmt();

        struct InstructionNode* parse_assign_stmt();

        struct InstructionNode* parse_expr();

        struct InstructionNode* parse_primary();

        int parse_op();

        struct InstructionNode* parse_output_stmt();

        struct InstructionNode* parse_input_stmt();

        struct InstructionNode* parse_while_stmt();

        struct InstructionNode* parse_if_stmt();

        struct InstructionNode* parse_condition();

        struct InstructionNode* parse_relop();

        struct InstructionNode* parse_switch_stmt();

        struct InstructionNode* parse_for_stmt();

        struct InstructionNode* parse_case_list();

        struct InstructionNode* parse_case();

        struct InstructionNode* parse_default_case();

        void parse_inputs();

        void parse_num_list();
        bool checkToken(Token, TokenType);
};

#endif