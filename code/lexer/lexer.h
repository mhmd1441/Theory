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
        T_ASSIGN,
        T_LPAREN,
        T_RPAREN,
        T_LBRACE,
        T_RBRACE,
        T_SEMICOLON,
        T_EOF,
        T_UNKNOWN
};

struct Token
{
        TokenType type;
        char *lexeme;
        Token *next;
};

extern Token *tokenListHead;
extern Token *tokenListTail;

void addToken(TokenType type, const char *lex);
bool isIdentifierStart(char c);
bool isIdentifierChar(char c);
void lexicalAnalysis(const char *input);