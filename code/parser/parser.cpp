#define _CRT_SECURE_NO_WARNINGS
#include "parser.h"

// ===========================
// PARSE TREE NODE MANAGEMENT
// ===========================

Token *currentToken = NULL;

// ===========================
// PARSE TREE NODE MANAGEMENT
// ===========================

TreeNode *newNode(const char *sym)
{
        TreeNode *n = new TreeNode;
        n->symbol = new char[strlen(sym) + 1];
        strcpy(n->symbol, sym);
        n->childCount = 0;
        n->children = NULL;
        return n;
}

void addChild(TreeNode *parent, TreeNode *child)
{
        if (!child)
                return;
        TreeNode **newArray = new TreeNode *[parent->childCount + 1];
        for (int i = 0; i < parent->childCount; i++)
        {
                newArray[i] = parent->children[i];
        }
        newArray[parent->childCount] = child;
        delete[] parent->children;
        parent->children = newArray;
        parent->childCount++;
}

void advanceToken()
{
        if (currentToken && currentToken->type != T_EOF)
                currentToken = currentToken->next;
}

TokenType peekType()
{
        if (currentToken)
                return currentToken->type;
        return T_EOF;
}

// ========================================================================
// PARSING FUNCTIONS - SYNTAX ANALYZER - recursive descent parsing approach
// ========================================================================


TreeNode *parseProgram()
{
        TreeNode *node = newNode("Program");
        TreeNode *stmtList = parseStatementList();
        addChild(node, stmtList);
        return node;
}

TreeNode *parseStatementList()
{
        TreeNode *node = newNode("StatementList");
        if (peekType() == T_INT || peekType() == T_STRING ||
            peekType() == T_IF || peekType() == T_WHILE || peekType() == T_RETURN ||
            peekType() == T_IDENTIFIER || peekType() == T_LBRACE)
        {
                TreeNode *st = parseStatement();
                if (st == NULL)
                        return node;
                addChild(node, st);

                // Only recurse if we haven't reached EOF
                if (peekType() != T_EOF)
                {
                        TreeNode *stl = parseStatementList();
                        if (stl && stl->childCount > 0)
                                addChild(node, stl);
                }
        }
        return node;
}

TreeNode *parseStatement()
{
        TreeNode *node = newNode("Statement");
        TokenType t = peekType();

        // Handle declarations (int, string)
        if (t == T_INT || t == T_STRING)
        {
                // Add the type keyword
                if (t == T_INT)
                        addChild(node, newNode("int"));
                else
                        addChild(node, newNode("string"));
                advanceToken();

                // Get the identifier
                if (peekType() != T_IDENTIFIER)
                {
                        cout << "Syntax error: expected identifier after type declaration\n";
                        return NULL;
                }
                Token *identTok = currentToken;
                addChild(node, newNode(identTok->lexeme));
                advanceToken();

                // Optional initialization with =
                if (peekType() == T_ASSIGN)
                {
                        addChild(node, newNode("="));
                        advanceToken();

                        TreeNode *expr = parseExpression();
                        if (expr == NULL)
                                return NULL;
                        addChild(node, expr);
                }

                // Require semicolon
                if (peekType() != T_SEMICOLON)
                {
                        cout << "Syntax error: expected ';' after declaration\n";
                        return NULL;
                }
                addChild(node, newNode(";"));
                advanceToken();

                return node;
        }

        if (t == T_IF)
        {
                addChild(node, newNode("if"));
                advanceToken(); // consume 'if'

                if (peekType() != T_LPAREN)
                {
                        cout << "Syntax error: expected '('\n";
                        return NULL;
                }
                addChild(node, newNode("("));
                advanceToken();

                TreeNode *expr = parseExpression();
                addChild(node, expr);

                if (peekType() != T_RPAREN)
                {
                        cout << "Syntax error: expected ')'\n";
                        return NULL;
                }
                addChild(node, newNode(")"));
                advanceToken();

                TreeNode *stmt = parseStatement();
                addChild(node, stmt);

                if (peekType() == T_ELSE)
                {
                        addChild(node, newNode("else"));
                        advanceToken();
                        TreeNode *stmt2 = parseStatement();
                        addChild(node, stmt2);
                }
        }
        else if (t == T_WHILE)
        {
                addChild(node, newNode("while"));
                advanceToken();
                if (peekType() != T_LPAREN)
                {
                        cout << "Syntax error: expected '('\n";
                        return NULL;
                }
                addChild(node, newNode("("));
                advanceToken();

                TreeNode *expr = parseExpression();
                addChild(node, expr);

                if (peekType() != T_RPAREN)
                {
                        cout << "Syntax error: expected ')'\n";
                        return NULL;
                }
                addChild(node, newNode(")"));
                advanceToken();

                TreeNode *stmt = parseStatement();
                addChild(node, stmt);
        }
        else if (t == T_RETURN)
        {
                addChild(node, newNode("return"));
                advanceToken();
                TreeNode *expr = parseExpression();
                addChild(node, expr);
                if (peekType() != T_SEMICOLON)
                {
                        cout << "Syntax error: expected ';'\n";
                        return NULL;
                }
                addChild(node, newNode(";"));
                advanceToken();
        }
        else if (t == T_LBRACE)
        {
                addChild(node, newNode("{"));
                advanceToken();
                TreeNode *stl = parseStatementList();
                addChild(node, stl);
                if (peekType() != T_RBRACE)
                {
                        cout << "Syntax error: expected '}'\n";
                        return NULL;
                }
                addChild(node, newNode("}"));
                advanceToken();
        }
        else if (t == T_IDENTIFIER)
        {
                Token *identTok = currentToken;
                addChild(node, newNode(identTok->lexeme));
                advanceToken();
                if (peekType() != T_ASSIGN)
                {
                        cout << "Syntax error: expected '='\n";
                        return NULL;
                }
                addChild(node, newNode("="));
                advanceToken();
                TreeNode *expr = parseExpression();
                addChild(node, expr);
                if (peekType() != T_SEMICOLON)
                {
                        cout << "Syntax error: expected ';'\n";
                        return NULL;
                }
                addChild(node, newNode(";"));
                advanceToken();
        }
        else
        {
                cout << "Syntax error: unexpected token in Statement\n";
                return NULL;
        }

        return node;
}

TreeNode *parseExpression()
{
        TreeNode *node = newNode("Expression");
        TokenType t = peekType();
        if (t == T_IDENTIFIER)
        {
                TreeNode *idNode = newNode(currentToken->lexeme);
                addChild(node, idNode);
                advanceToken();
        }
        else if (t == T_INTEGER)
        {
                TreeNode *intNode = newNode(currentToken->lexeme);
                addChild(node, intNode);
                advanceToken();
        }
        else
        {
                cout << "Syntax error: expected identifier or integer in Expression\n";
                return NULL;
        }
        return node;
}
// ========================
// TREE PRINTING FUNCTIONS
// ========================

void printTree(TreeNode *root, int level)
{
        for (int i = 0; i < level; i++)
                cout << "  ";
        cout << root->symbol << "\n";
        for (int i = 0; i < root->childCount; i++)
        {
                printTree(root->children[i], level + 1);
        }
}