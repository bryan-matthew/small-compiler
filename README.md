
![Logo](https://i.ibb.co/YBwQhnB/small-compiler.png)


# C/ C++ Small Compiler
A small compiler built in C/ C++ that will read an input program and represent it in a linked list.
A node of the linked list represents one instruction.
# Supported Grammar for the Compiler

The grammar for this project is the following:
- program → var section body inputs
- var section → id list SEMICOLON
- id list → ID COMMA id list | ID
- body → LBRACE stmt list RBRACE
- stmt list → stmt stmt list | stmt
- stmt → assign stmt | while stmt | if stmt | switch stmt | for stmt
- stmt → output stmt | input stmt
- assign stmt → ID EQUAL primary SEMICOLON
- assign stmt → ID EQUAL expr SEMICOLON
- expr → primary op primary
- primary → ID | NUM
- op → PLUS | MINUS | MULT | DIV
- output stmt → output ID SEMICOLON
- input stmt → input ID SEMICOLON
- while stmt → WHILE condition body
- if stmt → IF condition body
- condition → primary relop primary
- relop → GREATER | LESS | NOTEQUAL
- switch stmt → SWITCH ID LBRACE case list RBRACE
- switch stmt → SWITCH ID LBRACE case list default case RBRACE
- for stmt → FOR LPAREN assign stmt condition SEMICOLON assign stmt RPAREN body
- case list → case case list | case
- case → CASE NUM COLON body
- default case → DEFAULT COLON body
- inputs → num list
- num list → NUM
- num list → NUM num list
## Author(s)
- [@bryan-matthew](https://www.github.com/bryan-matthew)

