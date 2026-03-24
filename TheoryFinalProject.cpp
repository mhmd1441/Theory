#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <cctype>

using namespace std;

// =================
// TOKEN TYPES ENUM
// =================

enum TokenType
{
    T_INT,
    T_STRING,
    T_IF,
    T_ELSE,
    T_WHILE,
    T_RETURN,
    T_IDENTIFIER,
    T_INTEGER,
    T_ASSIGN,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_SEMICOLON,
    T_EOF,
    T_UNKNOWN
};



// ==========================
// STRUCTURES AND DATA TYPES
// ==========================

struct transition
{
    char origin;
    char label;
    char target;
};

struct automat
{
    int id;
    char *alphabet;
    int alphabetCount;
    char *states;
    int stateCount;
    char qo;
    transition *delta;
    int transitionCount;
    char *stateterminal;
    int terminalCount;
};

struct Token
{
    TokenType type;
    char* lexeme;
    Token* next;
};

struct TreeNode
{
    char* symbol;
    TreeNode** children;
    int childCount;
};

struct Symbol
{
    char* name;
    Symbol* next;
};



// =======================================
// GLOBAL VARIABLES - AUTOMATA MANAGEMENT
// =======================================

automat *automata = NULL;
int automataCount = 0;
int automataCapacity = 2;
int nextAutomatonID = 1;



// ==================
// UTILITY FUNCTIONS
// ==================

bool findInArray(char c, char arr[], int len)
{
    for (int i = 0; i < len; i++)
    {
        if (arr[i] == c)
            return true;
    }
    return false;
}

int findAutomatonIndexByID(int id)
{
    for (int i = 0; i < automataCount; i++)
    {
    if (automata[i].id == id)
            return i;
    }
    return -1;
}

void resizeAutomataArray()
{
    if (automataCount >= automataCapacity)
    {
        automataCapacity *= 2;
        automat *newArr = new automat[automataCapacity];
      for (int i = 0; i < automataCount; i++)
        {
          newArr[i] = automata[i];
        }
      delete[] automata;
  automata = newArr;
    }
}



// ==============================
// AUTOMATA MANAGEMENT FUNCTIONS
// ==============================

void addAutomaton()
{
    resizeAutomataArray();
    automat A;
    A.id = nextAutomatonID++;

    int nStates, nAlpha, nTrans, nTerm;
    cout << "Enter number of states: ";
 cin >> nStates;
    A.stateCount = nStates;
    A.states = new char[nStates];
    cout << "Enter states: ";
    for (int i = 0; i < nStates; i++)
        cin >> A.states[i];

    do
    {
        cout << "Enter initial state: ";
  cin >> A.qo;
    } while (!findInArray(A.qo, A.states, A.stateCount));

    cout << "Enter number of symbols: ";
    cin >> nAlpha;
    A.alphabetCount = nAlpha;
    A.alphabet = new char[nAlpha];
    cout << "Enter alphabet symbols: ";
    for (int i = 0; i < nAlpha; i++)
        cin >> A.alphabet[i];

    cout << "Enter number of transitions: ";
    cin >> nTrans;
    A.transitionCount = nTrans;
    A.delta = new transition[nTrans];
    for (int i = 0; i < nTrans; i++)
    {
        bool valid;
    do
    {
valid = true;
        cout << "Transition " << i + 1 << " (origin label target):\n";
            cout << "  Delta origin: ";
 cin >> A.delta[i].origin;
        cout << "  Delta label: ";
      cin >> A.delta[i].label;
            cout << "  Delta target: ";
     cin >> A.delta[i].target;

   if (!findInArray(A.delta[i].origin, A.states, A.stateCount) ||
   !findInArray(A.delta[i].label, A.alphabet, A.alphabetCount) ||
     !findInArray(A.delta[i].target, A.states, A.stateCount))
    {
          valid = false;
   cout << "Invalid transition, re-enter.\n";
    }
        } while (!valid);
    }

    cout << "Enter number of terminal states: ";
    cin >> nTerm;
    A.terminalCount = nTerm;
    A.stateterminal = new char[nTerm];
  cout << "Enter terminal states: ";
  for (int i = 0; i < nTerm; i++)
    {
        bool valid;
        do
        {
    valid = true;
          cout << "Terminal state: ";
          cin >> A.stateterminal[i];
      if (!findInArray(A.stateterminal[i], A.states, A.stateCount))
    {
valid = false;
        cout << "Not a valid state.\n";
     }
        } while (!valid);
    }

    automata[automataCount++] = A;
    cout << "Automaton added with ID " << A.id << endl;
}

void deleteAutomaton()
{
    int id;
    cout << "Enter automaton ID to delete: ";
    cin >> id;
    int idx = findAutomatonIndexByID(id);
    if (idx == -1)
    {
        cout << "Automaton not found.\n";
        return;
    }
    delete[] automata[idx].states;
    delete[] automata[idx].alphabet;
    delete[] automata[idx].delta;
    delete[] automata[idx].stateterminal;

    for (int i = idx; i < automataCount - 1; i++)
    {
   automata[i] = automata[i + 1];
    }
    automataCount--;
    cout << "Automaton with ID " << id << " deleted.\n";
}

void searchAutomaton()
{
    int id;
    cout << "Enter automaton ID to search: ";
    cin >> id;
    int idx = findAutomatonIndexByID(id);
    if (idx == -1)
    {
      cout << "Not found.\n";
  return;
    }
    automat &A = automata[idx];
    cout << "Automaton ID: " << A.id << "\nInitial State: " << A.qo << "\nStates: ";
    for (int i = 0; i < A.stateCount; i++)
   cout << A.states[i] << " ";
    cout << "\nAlphabet: ";
    for (int i = 0; i < A.alphabetCount; i++)
        cout << A.alphabet[i] << " ";
    cout << "\nTransitions:\n";
    for (int i = 0; i < A.transitionCount; i++)
    {
      cout << A.delta[i].origin << " --" << A.delta[i].label << "--> " << A.delta[i].target << "\n";
    }
    cout << "Terminal States: ";
    for (int i = 0; i < A.terminalCount; i++)
    cout << A.stateterminal[i] << " ";
    cout << endl;
}



// ==============================
// AUTOMATA SIMULATION FUNCTIONS
// ==============================

bool simulate(automat &A, const char *input)
{
    char current = A.qo;

    for (int i = 0; input[i] != '\0'; i++)
    {
        char symbol = input[i];
        bool transitionFound = false;

        if (!findInArray(symbol, A.alphabet, A.alphabetCount))
        {
      cout << "Error: symbol '" << symbol << "' not in alphabet.\n";
     return false;
   }

        for (int t = 0; t < A.transitionCount; t++)
        {
      if (A.delta[t].origin == current &&
   A.delta[t].label == symbol)
    {
                current = A.delta[t].target;
      transitionFound = true;
   break;
            }
  }

        if (!transitionFound)
 {
            cout << "Error: no transition for " << current
    << " with symbol " << symbol << endl;
            return false;
    }
    }

    return findInArray(current, A.stateterminal, A.terminalCount);
}

void testAutomaton()
{
    int id;
    cout << "Enter automaton ID to test: ";
 cin >> id;

    int idx = findAutomatonIndexByID(id);
    if (idx == -1)
    {
        cout << "Automaton not found.\n";
        return;
    }

    automat &A = automata[idx];

  char input[256];
    cout << "Enter input string to test: ";
    cin >> input;

    // call simulator
    bool accepted = simulate(A, input);

    if (accepted)
        cout << "Result: STRING ACCEPTED by Automaton " << id << endl;
    else
        cout << "Result: STRING REJECTED by Automaton " << id << endl;
}



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
       int len = p - start;
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
     int len = p - start;
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



// ===========================
// PARSE TREE NODE MANAGEMENT
// ===========================

Token *currentToken = NULL;

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

TreeNode *parseProgram();
TreeNode *parseStatementList();
TreeNode *parseStatement();
TreeNode *parseExpression();

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

void semanticCheck(TreeNode *root, bool &errorFlag, bool isLHS = false)
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

void generateCode(TreeNode *root, int indent = 0);

void printIndent(int indent)
{
    for (int i = 0; i < indent; i++)
        cout << "    ";
}

void generateCode(TreeNode *root, int indent)
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
     const char *type = root->children[0]->symbol;
      const char *varName = (root->childCount > 1) ? root->children[1]->symbol : "";
 
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



// ========================
// TREE PRINTING FUNCTIONS
// ========================

void printTree(TreeNode *root, int level = 0)
{
    for (int i = 0; i < level; i++)
        cout << "  ";
    cout << root->symbol << "\n";
    for (int i = 0; i < root->childCount; i++)
    {
    printTree(root->children[i], level + 1);
    }
}



int main()
{
    automata = new automat[automataCapacity];

    int choice;
    do
    {
        // Display menu
        cout << "\nMenu:\n";
        cout << "1. Add Automaton\n";
        cout << "2. Delete Automaton\n";
        cout << "3. Search Automaton\n";
        cout << "4. Test Automaton\n";
        cout << "5. Tokenize and Parse Input Program\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(1000, '\n');

        switch (choice)
        {
        case 1:
            addAutomaton();
            break;

        case 2:
            deleteAutomaton();
            break;

        case 3:
            searchAutomaton();
            break;

        case 4:
            testAutomaton();
            break;

        case 5:
        {
            char inputLine[1024];
            cout << "Enter program (single line, pseudo-C):\n";
            cin.getline(inputLine, 1024);

            // Lexical analysis
            lexicalAnalysis(inputLine);

            cout << "Tokens:\n";
            for (Token* t = tokenListHead; t; t = t->next)
            {
                cout << "Token: " << t->type << "  Lexeme: " << t->lexeme << "\n";
            }
            cout << "\n";

            // Parsing
            currentToken = tokenListHead;
            TreeNode* parseTree = parseProgram();

            if (parseTree && peekType() == T_EOF)
            {
                cout << "Parse successful.\n\nParse Tree:\n";
                printTree(parseTree);

                bool errorFlag = false;
                semanticCheck(parseTree, errorFlag, false);

                if (!errorFlag)
                {
                    cout << "Semantic check passed.\n\nConverted Code (to pseudo-Python):\n";
                    generateCode(parseTree, 0);
                }
                else
                {
                    cout << "Semantic checks failed.\n";
                }
            }
            else
            {
                cout << "Parsing failed.\n";
            }

            // Cleanup symbol table
            while (symbolTable)
            {
                Symbol* tmp = symbolTable;
                symbolTable = symbolTable->next;
                delete[] tmp->name;
                delete tmp;
            }

            // Cleanup tokens
            while (tokenListHead)
            {
                Token* tmp = tokenListHead;
                tokenListHead = tokenListHead->next;
                delete[] tmp->lexeme;
                delete tmp;
            }
            tokenListTail = nullptr;

            break;
        }

        case 6:
            cout << "Exiting.\n";
            break;

        default:
            cout << "Invalid choice.\n";
            break;
        }
    } while (choice != 6);

    // Free automata memory
    for (int i = 0; i < automataCount; i++)
    {
        delete[] automata[i].states;
        delete[] automata[i].alphabet;
        delete[] automata[i].delta;
        delete[] automata[i].stateterminal;
    }
    delete[] automata;

    return 0;
}
