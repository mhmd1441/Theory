#pragma once
#include "../lexer/lexer.h"
#include <iostream>
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
TreeNode *newNode(const char *sym);
void addChild(TreeNode *parent, TreeNode *child);
void advanceToken();
void printTree(TreeNode *root, int level = 0);