#pragma once
#include "../lexer/lexer.h"
#include <iostream>
#include <string>
using namespace std;

struct TreeNode
{
        char *symbol;
        TreeNode **children;
        int childCount;
};
TokenType peekType();
extern Token *currentToken;
TreeNode *parseProgram();
TreeNode *parseStatementList();
TreeNode *parseStatement();
TreeNode *parseExpression();
TreeNode *parseEquality();
TreeNode *parseComparison();
TreeNode *parseTerm();
TreeNode *parseFactor();
TreeNode *parsePrimary();
TreeNode *newNode(const char *sym);
void addChild(TreeNode *parent, TreeNode *child);
void advanceToken();
void printTree(TreeNode *root, int level = 0);
void freeTree(TreeNode *root);

extern string parserLastError;
bool hasParserError();
void clearParserError();
const string &getParserTrace();