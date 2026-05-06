#define _CRT_SECURE_NO_WARNINGS
#include "semantic.h"
#include <string>
#include <set>
#include <sstream>

// =====================================
// SEMANTIC ANALYSIS AND CODE GENERATION
// =====================================

Symbol *symbolTable = NULL;
static int currentScopeLevel = 0;

std::string declaredIdentifiersReport()
{
        std::set<std::string> names;
        Symbol *cur = symbolTable;
        while (cur)
        {
                names.insert(cur->name);
                cur = cur->next;
        }
        std::ostringstream out;
        out << "Declared identifiers set S = {";
        bool first = true;
        for (const auto &n : names)
        {
                if (!first) out << ", ";
                out << n;
                first = false;
        }
        out << "}";
        return out.str();
}

static bool isOperatorSymbol(const char *s)
{
        return strcmp(s, "+") == 0 || strcmp(s, "-") == 0 ||
               strcmp(s, "*") == 0 || strcmp(s, "/") == 0 ||
               strcmp(s, "==") == 0 || strcmp(s, "!=") == 0 ||
               strcmp(s, "!<") == 0 || strcmp(s, "!>") == 0 ||
               strcmp(s, "<") == 0 || strcmp(s, ">") == 0 ||
               strcmp(s, "<=") == 0 || strcmp(s, ">=") == 0;
}

static const char *inferExprType(TreeNode *node, bool &errorFlag)
{
        if (!node || errorFlag)
                return "unknown";

        if (strcmp(node->symbol, "Expression") != 0 && node->childCount == 0)
        {
                if (isdigit((unsigned char)node->symbol[0]))
                        return "int";
                if (node->symbol[0] == '"' || isalpha((unsigned char)node->symbol[0]) || node->symbol[0] == '_')
                {
                        if (isalpha((unsigned char)node->symbol[0]) || node->symbol[0] == '_')
                        {
                                const char *t = getSymbolType(node->symbol);
                                if (!t)
                                {
                                        cout << "Semantic error: variable '" << node->symbol << "' used before declaration.\n";
                                        errorFlag = true;
                                        return "unknown";
                                }
                                return t;
                        }
                        return "string";
                }
        }

        if (node->childCount == 3 && isOperatorSymbol(node->children[1]->symbol))
        {
                const char *leftType = inferExprType(node->children[0], errorFlag);
                const char *rightType = inferExprType(node->children[2], errorFlag);
                if (errorFlag)
                        return "unknown";

                const char *op = node->children[1]->symbol;
                if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 || strcmp(op, "/") == 0)
                {
                        if (strcmp(leftType, "int") != 0 || strcmp(rightType, "int") != 0)
                        {
                                cout << "Semantic error: arithmetic operator '" << op << "' requires integer operands.\n";
                                errorFlag = true;
                                return "unknown";
                        }
                        return "int";
                }
                if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 ||
                    strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
                    strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0)
                {
                        if (strcmp(leftType, rightType) != 0)
                        {
                                cout << "Semantic error: comparison between incompatible types.\n";
                                errorFlag = true;
                                return "unknown";
                        }
                        return "int";
                }
        }

        if (strcmp(node->symbol, "Expression") == 0 && node->childCount == 1)
                return inferExprType(node->children[0], errorFlag);

        if (node->childCount > 0)
                return inferExprType(node->children[0], errorFlag);

        return "unknown";
}

static string expressionToString(TreeNode *node)
{
        if (!node)
                return "";
        if (node->childCount == 0)
                return node->symbol;
        if (node->childCount == 3 && isOperatorSymbol(node->children[1]->symbol))
        {
                const char *op = node->children[1]->symbol;
                string opOut = op;
                if (opOut == "!<")
                        opOut = ">=";
                else if (opOut == "!>")
                        opOut = "<=";
                return "(" + expressionToString(node->children[0]) + " " + opOut + " " +
                       expressionToString(node->children[2]) + ")";
        }
        string result;
        for (int i = 0; i < node->childCount; i++)
        {
                if (!result.empty())
                        result += " ";
                result += expressionToString(node->children[i]);
        }
        return result;
}

// Helper function to add symbol to symbol table
void addSymbol(const char *name, const char *type)
{
        Symbol *newSymbol = new Symbol;
        newSymbol->name = new char[strlen(name) + 1];
        newSymbol->type = new char[strlen(type) + 1];
        strcpy(newSymbol->name, name);
        strcpy(newSymbol->type, type);
        newSymbol->scopeLevel = currentScopeLevel;
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

const char *getSymbolType(const char *name)
{
        Symbol *current = symbolTable;
        while (current)
        {
                if (strcmp(current->name, name) == 0)
                        return current->type;
                current = current->next;
        }
        return NULL;
}

bool symbolExistsInCurrentScope(const char *name)
{
        Symbol *current = symbolTable;
        while (current)
        {
                if (current->scopeLevel != currentScopeLevel)
                {
                        current = current->next;
                        continue;
                }
                if (strcmp(current->name, name) == 0)
                        return true;
                current = current->next;
        }
        return false;
}

void enterScope()
{
        currentScopeLevel++;
}

void exitScope()
{
        Symbol *cur = symbolTable;
        while (cur && cur->scopeLevel == currentScopeLevel)
        {
                Symbol *tmp = cur;
                cur = cur->next;
                delete[] tmp->name;
                delete[] tmp->type;
                delete tmp;
        }
        symbolTable = cur;
        if (currentScopeLevel > 0)
                currentScopeLevel--;
}

void resetSemanticState()
{
        // Free all symbols regardless of scope ordering and reset scope level.
        Symbol *cur = symbolTable;
        while (cur)
        {
                Symbol *tmp = cur;
                cur = cur->next;
                delete[] tmp->name;
                delete[] tmp->type;
                delete tmp;
        }
        symbolTable = NULL;
        currentScopeLevel = 0;
}

void semanticCheck(TreeNode* root, bool& errorFlag, bool isLHS)
{
        if (!root || errorFlag)
                return;

        if (strcmp(root->symbol, "Statement") == 0)
        {
                // New scope for blocks: Statement -> "{" StatementList "}"
                if (root->childCount > 0 && strcmp(root->children[0]->symbol, "{") == 0)
                {
                        enterScope();
                        for (int i = 0; i < root->childCount; i++)
                        {
                                semanticCheck(root->children[i], errorFlag, false);
                                if (errorFlag)
                                        break;
                        }
                        exitScope();
                        return;
                }

                // Check for declarations (int, string)
                if (root->childCount > 0 &&
                    (strcmp(root->children[0]->symbol, "int") == 0 ||
                     strcmp(root->children[0]->symbol, "string") == 0))
                {
                        const char *type = root->children[0]->symbol;
                        if (root->childCount > 1)
                        {
                                const char *varName = root->children[1]->symbol;
                                if (symbolExistsInCurrentScope(varName))
                                {
                                        cout << "Semantic error: variable '" << varName << "' already declared in this scope.\n";
                                        errorFlag = true;
                                        return;
                                }
                                addSymbol(varName, type);
                                if (root->childCount > 3 && strcmp(root->children[2]->symbol, "=") == 0)
                                {
                                        const char *exprType = inferExprType(root->children[3], errorFlag);
                                        if (!errorFlag && strcmp(type, exprType) != 0)
                                        {
                                                cout << "Semantic error: cannot assign expression of type '" << exprType
                                                     << "' to variable '" << varName << "' of type '" << type << "'.\n";
                                                errorFlag = true;
                                                return;
                                        }
                                }
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
                                const char *varType = getSymbolType(varName);
                                const char *exprType = inferExprType(root->children[2], errorFlag);
                                if (!errorFlag && varType && strcmp(varType, exprType) != 0)
                                {
                                        cout << "Semantic error: type mismatch in assignment to '" << varName << "'.\n";
                                        errorFlag = true;
                                        return;
                                }
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
                cout << varName << " = " << expressionToString(root->children[3]) << endl;
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
            cout << expressionToString(root->children[2]);
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
            cout << expressionToString(root->children[2]);
            cout << ":" << endl;
            generateCode(root->children[4], indent + 1);
            return;
        }

        if (root->childCount > 0 && strcmp(root->children[0]->symbol, "return") == 0)
        {
            printIndent(indent);
            cout << "return " << expressionToString(root->children[1]) << endl;
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
            cout << root->children[0]->symbol << " = " << expressionToString(root->children[2]) << endl;
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
