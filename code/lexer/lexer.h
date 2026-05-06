#pragma once
#include <iostream>
#include <cstring>
#include <cctype>
using namespace std;

enum TokenType
{
        T_INT,
        T_STRING,
        T_IF,
        T_ELSE,
        T_WHILE,
        T_RETURN,
        T_IDENTIFIER,
        T_INTEGER,
        T_STRING_LITERAL,
        T_ASSIGN,
        T_PLUS,
        T_MINUS,
        T_MUL,
        T_DIV,
        T_EQ,
        T_NEQ,
        T_LT,
        T_GT,
        T_LE,
        T_GE,
        T_LPAREN,
        T_RPAREN,
        T_LBRACE,
        T_RBRACE,
        T_SEMICOLON,
        T_COMMA,
        T_EOF,
        T_UNKNOWN
};

struct Token
{
        TokenType type;
        char *lexeme;
        int line;
        int col;
        Token *next;
};

extern Token *tokenListHead;
extern Token *tokenListTail;

const char *tokenTypeName(TokenType t);
void addToken(TokenType type, const char *lex, int line, int col);
bool isIdentifierStart(char c);
bool isIdentifierChar(char c);
void lexicalAnalysis(const char *input);