#define _CRT_SECURE_NO_WARNINGS
#include "automaton.h"



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
