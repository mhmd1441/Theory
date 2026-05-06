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

const char *tokenTypeName(TokenType t)
{
        switch (t)
        {
        case T_INT: return "INT";
        case T_STRING: return "STRING";
        case T_IF: return "IF";
        case T_ELSE: return "ELSE";
        case T_WHILE: return "WHILE";
        case T_RETURN: return "RETURN";
        case T_IDENTIFIER: return "IDENTIFIER";
        case T_INTEGER: return "INTEGER";
        case T_STRING_LITERAL: return "STRING_LITERAL";
        case T_ASSIGN: return "ASSIGN";
        case T_PLUS: return "PLUS";
        case T_MINUS: return "MINUS";
        case T_MUL: return "MUL";
        case T_DIV: return "DIV";
        case T_EQ: return "EQ";
        case T_NEQ: return "NEQ";
        case T_LT: return "LT";
        case T_GT: return "GT";
        case T_LE: return "LE";
        case T_GE: return "GE";
        case T_LPAREN: return "LPAREN";
        case T_RPAREN: return "RPAREN";
        case T_LBRACE: return "LBRACE";
        case T_RBRACE: return "RBRACE";
        case T_SEMICOLON: return "SEMICOLON";
        case T_COMMA: return "COMMA";
        case T_EOF: return "EOF";
        default: return "UNKNOWN";
        }
}

void addToken(TokenType type, const char *lex, int line, int col)
{
        Token *t = new Token;
        t->type = type;
        t->lexeme = new char[strlen(lex) + 1];
        strcpy(t->lexeme, lex);
        t->line = line;
        t->col = col;
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
        int line = 1;
        int col = 1;
        while (*p)
        {
                if (isspace((unsigned char)*p))
                {
                        if (*p == '\n')
                        {
                                line++;
                                col = 1;
                        }
                        else
                        {
                                col++;
                        }
                        p++;
                        continue;
                }

                if (isIdentifierStart(*p))
                {
                        int startLine = line;
                        int startCol = col;
                        const char *start = p;
                        p++;
                        col++;
                        while (isIdentifierChar(*p))
                        {
                                p++;
                                col++;
                        }
                        int len = (int)(p - start);
                        char *buf = new char[len + 1];
                        strncpy(buf, start, len);
                        buf[len] = '\0';
                        if (strcmp(buf, "int") == 0)
                                addToken(T_INT, buf, startLine, startCol);
                        else if (strcmp(buf, "string") == 0)
                                addToken(T_STRING, buf, startLine, startCol);
                        else if (strcmp(buf, "if") == 0)
                                addToken(T_IF, buf, startLine, startCol);
                        else if (strcmp(buf, "else") == 0)
                                addToken(T_ELSE, buf, startLine, startCol);
                        else if (strcmp(buf, "while") == 0)
                                addToken(T_WHILE, buf, startLine, startCol);
                        else if (strcmp(buf, "return") == 0)
                                addToken(T_RETURN, buf, startLine, startCol);
                        else
                                addToken(T_IDENTIFIER, buf, startLine, startCol);
                        delete[] buf;
                }
                else if (isdigit((unsigned char)*p))
                {
                        // integer
                        int startLine = line;
                        int startCol = col;
                        const char *start = p;
                        while (isdigit((unsigned char)*p))
                        {
                                p++;
                                col++;
                        }
                        int len = (int)(p - start);
                        char *buf = new char[len + 1];
                        strncpy(buf, start, len);
                        buf[len] = '\0';
                        addToken(T_INTEGER, buf, startLine, startCol);
                        delete[] buf;
                }
                else if (*p == '"')
                {
                        // string literal
                        int startLine = line;
                        int startCol = col;
                        p++; // consume opening quote
                        col++;
                        const char *start = p;
                        while (*p && *p != '"')
                        {
                                p++;
                                col++;
                        }
                        int len = (int)(p - start);
                        char *buf = new char[len + 3];
                        buf[0] = '"';
                        strncpy(buf + 1, start, len);
                        buf[len + 1] = '"';
                        buf[len + 2] = '\0';
                        addToken(T_STRING_LITERAL, buf, startLine, startCol);
                        delete[] buf;
                        if (*p == '"')
                        {
                                p++; // consume closing quote
                                col++;
                        }
                }
                else
                {
                        if ((*p == '=' && *(p + 1) == '=') ||
                            (*p == '!' && *(p + 1) == '=') ||
                            (*p == '!' && *(p + 1) == '<') ||
                            (*p == '!' && *(p + 1) == '>') ||
                            (*p == '<' && *(p + 1) == '=') ||
                            (*p == '>' && *(p + 1) == '='))
                        {
                                int startLine = line;
                                int startCol = col;
                                char op[3];
                                op[0] = *p;
                                op[1] = *(p + 1);
                                op[2] = '\0';
                                if (op[0] == '=')
                                        addToken(T_EQ, op, startLine, startCol);
                                else if (op[0] == '!')
                                {
                                        if (op[1] == '=')
                                                addToken(T_NEQ, op, startLine, startCol);
                                        else if (op[1] == '<')
                                                addToken(T_GE, op, startLine, startCol); // !<  == >=
                                        else if (op[1] == '>')
                                                addToken(T_LE, op, startLine, startCol); // !>  == <=
                                }
                                else if (op[0] == '<')
                                        addToken(T_LE, op, startLine, startCol);
                                else if (op[0] == '>')
                                        addToken(T_GE, op, startLine, startCol);
                                p += 2;
                                col += 2;
                                continue;
                        }

                        int startLine = line;
                        int startCol = col;
                        char c = *p;
                        p++;
                        col++;
                        char buf[2];
                        buf[0] = c;
                        buf[1] = '\0';
                        switch (c)
                        {
                        case '=':
                                addToken(T_ASSIGN, buf, startLine, startCol);
                                break;
                        case '+':
                                addToken(T_PLUS, buf, startLine, startCol);
                                break;
                        case '-':
                                addToken(T_MINUS, buf, startLine, startCol);
                                break;
                        case '*':
                                addToken(T_MUL, buf, startLine, startCol);
                                break;
                        case '/':
                                addToken(T_DIV, buf, startLine, startCol);
                                break;
                        case '<':
                                addToken(T_LT, buf, startLine, startCol);
                                break;
                        case '>':
                                addToken(T_GT, buf, startLine, startCol);
                                break;
                        case '(':
                                addToken(T_LPAREN, buf, startLine, startCol);
                                break;
                        case ')':
                                addToken(T_RPAREN, buf, startLine, startCol);
                                break;
                        case '{':
                                addToken(T_LBRACE, buf, startLine, startCol);
                                break;
                        case '}':
                                addToken(T_RBRACE, buf, startLine, startCol);
                                break;
                        case ';':
                                addToken(T_SEMICOLON, buf, startLine, startCol);
                                break;
                        case ',':
                                addToken(T_COMMA, buf, startLine, startCol);
                                break;
                        default:
                                addToken(T_UNKNOWN, buf, startLine, startCol);
                                break;
                        }
                }
        }
        addToken(T_EOF, "EOF", line, col);
}
