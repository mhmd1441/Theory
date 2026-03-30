#define _CRT_SECURE_NO_WARNINGS
#include "semantic.h"

// =====================================
// SEMANTIC ANALYSIS AND CODE GENERATION
// =====================================

Symbol *symbolTable = NULL;

// Helper function to add symbol to symbol table
void addSymbol(const char *name, const char *type)
{
        Symbol *newSymbol = new Symbol;
        newSymbol->name = new char[strlen(name) + 1];
        strcpy(newSymbol->name, name);
        newSymbol->next = symbolTable;
        symbolTable = newSymbol;
        cout << "Declared: " << type << " " << name << endl;
}

// Helper function to check if symbol exists
bool symbolExists(const char *name)
{
        Symbol *current = symbolTable;
        while (current)
        {
                if (strcmp(current->name, name) == 0)
                        return true;
                current = current->next;
        }
        return false;
}

void semanticCheck(TreeNode* root, bool& errorFlag, bool isLHS)
{
        if (!root || errorFlag)
                return;

        if (strcmp(root->symbol, "Statement") == 0)
        {
                // Check for declarations (int, string)
                if (root->childCount > 0 &&
                    (strcmp(root->children[0]->symbol, "int") == 0 ||
                     strcmp(root->children[0]->symbol, "string") == 0))
                {
                        const char *type = root->children[0]->symbol;
                        if (root->childCount > 1)
                        {
                                const char *varName = root->children[1]->symbol;
                                if (symbolExists(varName))
                                {
                                        cout << "Semantic error: variable '" << varName << "' already declared.\n";
                                        errorFlag = true;
                                        return;
                                }
                                addSymbol(varName, type);
                        }
                        return;
                }

                // Check for assignments
                if (root->childCount > 0 && root->children[0]->childCount == 0 && isIdentifierStart(root->children[0]->symbol[0]))
                {
                        if (root->childCount > 1 && strcmp(root->children[1]->symbol, "=") == 0)
                        {
                                const char *varName = root->children[0]->symbol;
                                if (!symbolExists(varName))
                                {
                                        cout << "Semantic error: variable '" << varName << "' used before declaration.\n";
                                        errorFlag = true;
                                        return;
                                }
                                semanticCheck(root->children[2], errorFlag, false);
                                return;
                        }
                }
        }
        for (int i = 0; i < root->childCount; i++)
        {
                semanticCheck(root->children[i], errorFlag, false);
        }
}

void generateCode(TreeNode* root, int indent)
{
    if (!root)
        return;
    if (strcmp(root->symbol, "Program") == 0)
    {
        for (int i = 0; i < root->childCount; i++)
            generateCode(root->children[i], indent);
        return;
    }

    if (strcmp(root->symbol, "StatementList") == 0)
    {
        for (int i = 0; i < root->childCount; i++)
            generateCode(root->children[i], indent);
        return;
    }

    if (strcmp(root->symbol, "Statement") == 0)
    {
        // Handle declarations (int, string) - convert to Python
        if (root->childCount > 0 &&
            (strcmp(root->children[0]->symbol, "int") == 0 ||
                strcmp(root->children[0]->symbol, "string") == 0))
        {
            printIndent(indent);
            const char* type = root->children[0]->symbol;
            const char* varName = (root->childCount > 1) ? root->children[1]->symbol : "";

            // Check if there's an assignment
            if (root->childCount > 2 && strcmp(root->children[2]->symbol, "=") == 0)
            {
                // Declaration with initialization: type var = value
                cout << varName << " = " << root->children[3]->children[0]->symbol << endl;
            }
            else
            {
                // Declaration without initialization: type var
                if (strcmp(type, "int") == 0)
                    cout << varName << " = 0" << endl;
                else if (strcmp(type, "string") == 0)
                    cout << varName << " = \"\"" << endl;
            }
            return;
        }

        if (root->childCount > 0 && strcmp(root->children[0]->symbol, "if") == 0)
        {
            printIndent(indent);
            cout << "if ";
            cout << root->children[2]->children[0]->symbol;
            cout << ":" << endl;
            generateCode(root->children[4], indent + 1);
            if (root->childCount > 5 && strcmp(root->children[5]->symbol, "else") == 0)
            {
                printIndent(indent);
                cout << "else:" << endl;
                generateCode(root->children[6], indent + 1);
            }
            return;
        }

        if (root->childCount > 0 && strcmp(root->children[0]->symbol, "while") == 0)
        {
            printIndent(indent);
            cout << "while ";
            cout << root->children[2]->children[0]->symbol;
            cout << ":" << endl;
            generateCode(root->children[4], indent + 1);
            return;
        }

        if (root->childCount > 0 && strcmp(root->children[0]->symbol, "return") == 0)
        {
            printIndent(indent);
            cout << "return " << root->children[1]->children[0]->symbol << endl;
            return;
        }

        if (root->childCount > 0 && strcmp(root->children[0]->symbol, "{") == 0)
        {
            generateCode(root->children[1], indent);
            return;
        }

        // Handle regular assignments
        if (root->childCount > 3 && strcmp(root->children[1]->symbol, "=") == 0)
        {
            printIndent(indent);
            cout << root->children[0]->symbol << " = " << root->children[2]->children[0]->symbol << endl;
            return;
        }

        return;
    }
    for (int i = 0; i < root->childCount; i++)
    {
        generateCode(root->children[i], indent);
    }
}

void printIndent(int indent)
{
        for (int i = 0; i < indent; i++)
                cout << "    ";
}
