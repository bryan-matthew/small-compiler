/*
 * Bryan Matthew
 *
 * Do not share this file with anyone
 */
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "VAR", "FOR", "IF", "WHILE", "SWITCH", "CASE", "DEFAULT", "INPUT", "OUTPUT", "ARRAY",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN", "LBRACE", "RBRACE",
    "NOTEQUAL", "GREATER", "LESS",
    "NUM", "ID", "ERROR"
};

#define KEYWORDS_COUNT 9
string keyword[] = { "VAR", "FOR", "IF", "WHILE", "SWITCH", "CASE", "DEFAULT", "input", "output", "ARRAY" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;

    //Token token = GetTokenMain();
    //index = 0;

    //while (token.token_type != END_OF_FILE)
    //{
        //tokenList.push_back(token);     // push token into internal list
        //token = GetTokenMain();        // and get next token from standatd input
    //}
    // pushes END_OF_FILE is not pushed on the token list
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;

    input.GetChar(c);
    if (isdigit(c)) {
        if (c == '0') {
            tmp.lexeme = "0";
        } else {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if (!input.EndOfInput()) {
                input.UngetChar(c);
            }
        }
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}


// peek requires that the argument "howFar" be positive.
Token LexicalAnalyzer::peek(int howFar)
{
    if (howFar <= 0) {      // peeking backward or in place is not allowed
        cout << "LexicalAnalyzer:peek:Error: non positive argument\n";
        exit(-1);
    }

    int peekIndex = index + howFar - 1;
    if (peekIndex > (tokenList.size()-1)) { // if peeking too far
        Token token;                        // return END_OF_FILE
        token.lexeme = "";
        token.line_no = line_no;
        token.token_type = END_OF_FILE;
        return token;
    } else
        return tokenList[peekIndex];
}

Token LexicalAnalyzer::GetTokenMain() 
{
    char c;
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '+':   tmp.token_type = PLUS;      return tmp;
        case '-':   tmp.token_type = MINUS;     return tmp;
        case '/':   tmp.token_type = DIV;       return tmp;
        case '*':   tmp.token_type = MULT;      return tmp;
        case '=':   tmp.token_type = EQUAL;     return tmp;
        case ':':   tmp.token_type = COLON;     return tmp;
        case ',':   tmp.token_type = COMMA;     return tmp;
        case ';':   tmp.token_type = SEMICOLON; return tmp;
        case '[':   tmp.token_type = LBRAC;     return tmp;
        case ']':   tmp.token_type = RBRAC;     return tmp;
        case '(':   tmp.token_type = LPAREN;    return tmp;
        case ')':   tmp.token_type = RPAREN;    return tmp;
        case '{':   tmp.token_type = LBRACE;    return tmp;
        case '}':   tmp.token_type = RBRACE;    return tmp;
        case '>':   tmp.token_type = GREATER;   return tmp;
        case '<':
            input.GetChar(c);
            if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}


TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokenList.push_back(tok);;
    return tok.token_type;
}


// GetToken() accesses tokenList from the tokenList that is populated when a 
// lexer object is instantiated
Token LexicalAnalyzer::GetToken()
{
    // if there are tokenList that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokenList.empty()) {
        tmp = tokenList.back();
        tokenList.pop_back();
        return tmp;
    }

    return GetTokenMain();
    
}

