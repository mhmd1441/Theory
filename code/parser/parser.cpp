#define _CRT_SECURE_NO_WARNINGS
#include "parser.h"

// ===========================
// PARSE TREE NODE MANAGEMENT
// ===========================

Token *currentToken = NULL;
string parserLastError = "";
static string parserTrace = "";

static void trace(const string &msg)
{
        parserTrace += msg + "\n";
}

static void setParserError(const string &msg)
{
        if (!parserLastError.empty())
                return;
        if (currentToken)
        {
                parserLastError = "Syntax error at (" + to_string(currentToken->line) + "," + to_string(currentToken->col) +
                                 "): " + msg + " (got '" + string(currentToken->lexeme) + "')";
                if (msg.find("';'") != string::npos)
                        parserLastError += "\nHint: add a semicolon ';' at the end of the statement.";
                else if (msg.find("')'") != string::npos)
                        parserLastError += "\nHint: check parenthesis balance near this expression.";
                else if (msg.find("'='") != string::npos)
                        parserLastError += "\nHint: assignment format is: identifier = expression ;";
                else if (msg.find("identifier") != string::npos)
                        parserLastError += "\nHint: identifier names should start with a letter or underscore.";
        }
        else
        {
                parserLastError = "Syntax error: " + msg;
        }
}

bool hasParserError()
{
        return !parserLastError.empty();
}

void clearParserError()
{
        parserLastError.clear();
        parserTrace.clear();
}

const string &getParserTrace()
{
        return parserTrace;
}

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
        {
                trace(string("consume token: ") + currentToken->lexeme);
                currentToken = currentToken->next;
        }
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
        clearParserError();
        trace("rule: Program -> StatementList");
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
                trace("rule: StatementList -> Statement StatementList");
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
                trace("rule: Statement -> (int|string) id (= Expression)? ;");
                // Add the type keyword
                if (t == T_INT)
                        addChild(node, newNode("int"));
                else
                        addChild(node, newNode("string"));
                advanceToken();

                // Get the identifier
                if (peekType() != T_IDENTIFIER)
                {
                        setParserError("expected identifier after type");
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
                        setParserError("expected ';' after declaration");
                        return NULL;
                }
                addChild(node, newNode(";"));
                advanceToken();

                return node;
        }

        if (t == T_IF)
        {
                trace("rule: Statement -> if ( Expression ) Statement (else Statement)?");
                addChild(node, newNode("if"));
                advanceToken(); // consume 'if'

                if (peekType() != T_LPAREN)
                {
                        setParserError("expected '(' after if");
                        return NULL;
                }
                addChild(node, newNode("("));
                advanceToken();

                TreeNode *expr = parseExpression();
                addChild(node, expr);

                if (peekType() != T_RPAREN)
                {
                        setParserError("expected ')' after condition");
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
                trace("rule: Statement -> while ( Expression ) Statement");
                addChild(node, newNode("while"));
                advanceToken();
                if (peekType() != T_LPAREN)
                {
                        setParserError("expected '(' after while");
                        return NULL;
                }
                addChild(node, newNode("("));
                advanceToken();

                TreeNode *expr = parseExpression();
                addChild(node, expr);

                if (peekType() != T_RPAREN)
                {
                        setParserError("expected ')' after condition");
                        return NULL;
                }
                addChild(node, newNode(")"));
                advanceToken();

                TreeNode *stmt = parseStatement();
                addChild(node, stmt);
        }
        else if (t == T_RETURN)
        {
                trace("rule: Statement -> return Expression ;");
                addChild(node, newNode("return"));
                advanceToken();
                TreeNode *expr = parseExpression();
                addChild(node, expr);
                if (peekType() != T_SEMICOLON)
                {
                        setParserError("expected ';' after return");
                        return NULL;
                }
                addChild(node, newNode(";"));
                advanceToken();
        }
        else if (t == T_LBRACE)
        {
                trace("rule: Statement -> { StatementList }");
                addChild(node, newNode("{"));
                advanceToken();
                TreeNode *stl = parseStatementList();
                addChild(node, stl);
                if (peekType() != T_RBRACE)
                {
                        setParserError("expected '}' to close block");
                        return NULL;
                }
                addChild(node, newNode("}"));
                advanceToken();
        }
        else if (t == T_IDENTIFIER)
        {
                trace("rule: Statement -> id = Expression ;");
                Token *identTok = currentToken;
                addChild(node, newNode(identTok->lexeme));
                advanceToken();
                if (peekType() != T_ASSIGN)
                {
                        setParserError("expected '=' in assignment");
                        return NULL;
                }
                addChild(node, newNode("="));
                advanceToken();
                TreeNode *expr = parseExpression();
                addChild(node, expr);
                if (peekType() != T_SEMICOLON)
                {
                        setParserError("expected ';' after assignment");
                        return NULL;
                }
                addChild(node, newNode(";"));
                advanceToken();
        }
        else
        {
                setParserError("unexpected token in statement");
                return NULL;
        }

        return node;
}

TreeNode *parseExpression()
{
        trace("rule: Expression -> Equality");
        return parseEquality();
}

TreeNode *parseEquality()
{
        TreeNode *left = parseComparison();
        if (!left)
                return NULL;

        while (peekType() == T_EQ || peekType() == T_NEQ)
        {
                TreeNode *expr = newNode("Expression");
                addChild(expr, left);
                addChild(expr, newNode(currentToken->lexeme));
                advanceToken();
                TreeNode *right = parseComparison();
                if (!right)
                        return NULL;
                addChild(expr, right);
                left = expr;
        }
        return left;
}

TreeNode *parseComparison()
{
        TreeNode *left = parseTerm();
        if (!left)
                return NULL;

        while (peekType() == T_LT || peekType() == T_GT || peekType() == T_LE || peekType() == T_GE)
        {
                TreeNode *expr = newNode("Expression");
                addChild(expr, left);
                addChild(expr, newNode(currentToken->lexeme));
                advanceToken();
                TreeNode *right = parseTerm();
                if (!right)
                        return NULL;
                addChild(expr, right);
                left = expr;
        }
        return left;
}

TreeNode *parseTerm()
{
        TreeNode *left = parseFactor();
        if (!left)
                return NULL;

        while (peekType() == T_PLUS || peekType() == T_MINUS)
        {
                TreeNode *expr = newNode("Expression");
                addChild(expr, left);
                addChild(expr, newNode(currentToken->lexeme));
                advanceToken();
                TreeNode *right = parseFactor();
                if (!right)
                        return NULL;
                addChild(expr, right);
                left = expr;
        }
        return left;
}

TreeNode *parseFactor()
{
        TreeNode *left = parsePrimary();
        if (!left)
                return NULL;

        while (peekType() == T_MUL || peekType() == T_DIV)
        {
                TreeNode *expr = newNode("Expression");
                addChild(expr, left);
                addChild(expr, newNode(currentToken->lexeme));
                advanceToken();
                TreeNode *right = parsePrimary();
                if (!right)
                        return NULL;
                addChild(expr, right);
                left = expr;
        }
        return left;
}

TreeNode *parsePrimary()
{
        TokenType t = peekType();
        if (t == T_IDENTIFIER || t == T_INTEGER || t == T_STRING_LITERAL)
        {
                TreeNode *n = newNode(currentToken->lexeme);
                advanceToken();
                return n;
        }

        if (t == T_LPAREN)
        {
                advanceToken();
                TreeNode *inner = parseExpression();
                if (!inner)
                        return NULL;
                if (peekType() != T_RPAREN)
                {
                        setParserError("expected ')' in expression");
                        return NULL;
                }
                advanceToken();
                return inner;
        }

        setParserError("invalid token in expression");
        return NULL;
}
// ========================
// TREE PRINTING FUNCTIONS
// ========================

void printTree(TreeNode *root, int level)
{
        if (!root)
                return;
        for (int i = 0; i < level; i++)
                cout << "  ";
        cout << root->symbol << "\n";
        for (int i = 0; i < root->childCount; i++)
        {
                printTree(root->children[i], level + 1);
        }
}

void freeTree(TreeNode *root)
{
        if (!root)
                return;
        for (int i = 0; i < root->childCount; i++)
        {
                freeTree(root->children[i]);
        }
        delete[] root->children;
        delete[] root->symbol;
        delete root;
}