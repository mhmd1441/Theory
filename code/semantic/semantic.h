#pragma once
#include "../parser/parser.h"
#include <iostream>
#include <string>
using namespace std;

struct Symbol
{
    char* name;
    char* type;
    int scopeLevel;
    Symbol* next;
};

extern Symbol* symbolTable;

void addSymbol(const char* name, const char* type);
bool symbolExists(const char* name);
const char* getSymbolType(const char* name);
bool symbolExistsInCurrentScope(const char* name);
void enterScope();
void exitScope();
void resetSemanticState();
void semanticCheck(TreeNode* root, bool& errorFlag, bool isLHS = false);
void printIndent(int indent);
void generateCode(TreeNode* root, int indent = 0);

// Course-aligned attribute output: set of declared identifiers (synthesized attribute)
std::string declaredIdentifiersReport();