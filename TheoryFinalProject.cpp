#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include <cctype>
#include "code/automaton/automaton.h"
#include "code/lexer/lexer.h"
#include "code/parser/parser.h"
#include "code/semantic/semantic.h"

using namespace std;

int main()
{
    automata = new automat[automataCapacity]();
	int choice;
    do
    {
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

            lexicalAnalysis(inputLine);

            cout << "Tokens:\n";
            for (Token *t = tokenListHead; t; t = t->next)
            {
                cout << "Token: " << tokenTypeName(t->type) << "  Lexeme: " << t->lexeme
                     << "  @(" << t->line << "," << t->col << ")\n";
            }
            cout << "\n";

            currentToken = tokenListHead;
            TreeNode *parseTree = parseProgram();

            if (parseTree && peekType() == T_EOF)
            {
                cout << "Parse successful.\n\nParse Tree:\n";
                printTree(parseTree);

                bool errorFlag = false;
                semanticCheck(parseTree, errorFlag, false);

                if (!errorFlag)
                {
                    cout << "Semantic check passed.\n\nConverted Code (to pseudo-Python):\n";
                    cout << declaredIdentifiersReport() << "\n\n";
                    generateCode(parseTree, 0);
                }
                else
                {
                    cout << "Semantic checks failed.\n";
                }
                freeTree(parseTree);
            }
            else
            {
                cout << "Parsing failed.\n";
                if (hasParserError())
                {
                    string raw = parserLastError;
                    string firstLine = raw.substr(0, raw.find('\n'));
                    string nearToken = "?";
                    string expected = firstLine;
                    size_t gotPos = firstLine.find("(got '");
                    if (gotPos != string::npos)
                    {
                        size_t tokenStart = gotPos + 6;
                        size_t tokenEnd = firstLine.find("')", tokenStart);
                        if (tokenEnd != string::npos)
                                nearToken = firstLine.substr(tokenStart, tokenEnd - tokenStart);
                        expected = firstLine.substr(0, gotPos);
                    }
                    size_t colonPos = expected.find("): ");
                    if (colonPos != string::npos)
                            expected = expected.substr(colonPos + 3);
                    if (!expected.empty())
                            expected[0] = (char)toupper((unsigned char)expected[0]);

                    cout << "Syntax error near token '" << nearToken << "'\n";
                    cout << expected << "\n";
                    cout << raw << "\n";
                }
                if (parseTree)
                        freeTree(parseTree);
            }

            resetSemanticState();

            while (tokenListHead)
            {
                Token *tmp = tokenListHead;
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
