#pragma once
#include "../parser/parser.h"
#include <iostream>
using namespace std;

struct Symbol
{
    char* name;
    Symbol* next;
};

extern Symbol* symbolTable;

void addSymbol(const char* name, const char* type);
bool symbolExists(const char* name);
void semanticCheck(TreeNode* root, bool& errorFlag, bool isLHS = false);
void printIndent(int indent);
void generateCode(TreeNode* root, int indent = 0);