#define _CRT_SECURE_NO_WARNINGS
#include "lexer.h"

// ========================
// GLOBAL TOKEN VARIABLES
// ========================

Token *tokenListHead = NULL;
Token *tokenListTail = NULL;

// ===========================
// LEXICAL ANALYSIS FUNCTIONS
// ===========================

void addToken(TokenType type, const char *lex)
{
        Token *t = new Token;
        t->type = type;
        t->lexeme = new char[strlen(lex) + 1];
        strcpy(t->lexeme, lex);
        t->next = NULL;
        if (!tokenListHead)
        {
                tokenListHead = t;
                tokenListTail = t;
        }
        else
        {
                tokenListTail->next = t;
                tokenListTail = t;
        }
}

bool isIdentifierStart(char c) { return isalpha((unsigned char)c) || c == '_'; }
bool isIdentifierChar(char c) { return isalnum((unsigned char)c) || c == '_'; }

void lexicalAnalysis(const char *input)
{
        while (tokenListHead)
        {
                Token *temp = tokenListHead;
                tokenListHead = tokenListHead->next;
                delete[] temp->lexeme;
                delete temp;
        }
        tokenListTail = NULL;

        const char *p = input;
        while (*p)
        {
                if (isspace((unsigned char)*p))
                {
                        p++;
                        continue;
                }

                if (isIdentifierStart(*p))
                {
                        const char *start = p;
                        p++;
                        while (isIdentifierChar(*p))
                                p++;
                        int len = (int)(p - start);
                        char *buf = new char[len + 1];
                        strncpy(buf, start, len);
                        buf[len] = '\0';
                        if (strcmp(buf, "int") == 0)
                                addToken(T_INT, buf);
                        else if (strcmp(buf, "string") == 0)
                                addToken(T_STRING, buf);
                        else if (strcmp(buf, "if") == 0)
                                addToken(T_IF, buf);
                        else if (strcmp(buf, "else") == 0)
                                addToken(T_ELSE, buf);
                        else if (strcmp(buf, "while") == 0)
                                addToken(T_WHILE, buf);
                        else if (strcmp(buf, "return") == 0)
                                addToken(T_RETURN, buf);
                        else
                                addToken(T_IDENTIFIER, buf);
                        delete[] buf;
                }
                else if (isdigit((unsigned char)*p))
                {
                        // integer
                        const char *start = p;
                        while (isdigit((unsigned char)*p))
                                p++;
                        int len = (int)(p - start);
                        char *buf = new char[len + 1];
                        strncpy(buf, start, len);
                        buf[len] = '\0';
                        addToken(T_INTEGER, buf);
                        delete[] buf;
                }
                else
                {
                        char c = *p;
                        p++;
                        char buf[2];
                        buf[0] = c;
                        buf[1] = '\0';
                        switch (c)
                        {
                        case '=':
                                addToken(T_ASSIGN, buf);
                                break;
                        case '(':
                                addToken(T_LPAREN, buf);
                                break;
                        case ')':
                                addToken(T_RPAREN, buf);
                                break;
                        case '{':
                                addToken(T_LBRACE, buf);
                                break;
                        case '}':
                                addToken(T_RBRACE, buf);
                                break;
                        case ';':
                                addToken(T_SEMICOLON, buf);
                                break;
                        default:
                                addToken(T_UNKNOWN, buf);
                                break;
                        }
                }
        }
        addToken(T_EOF, "EOF");
}
