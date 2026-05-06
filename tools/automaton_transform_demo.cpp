#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>

#include "../code/automaton/automaton.h"

using namespace std;

int main()
{
    // Ensure automaton module globals are initialized for demos
    automata = new automat[automataCapacity]();

    // Small NFA with epsilon split:
    // q0 -e-> q1 (accepts a*)
    // q0 -e-> q2 (accepts b*)
    automat nfa;
    nfa.id = 0;
    nfa.alphabetCount = 3;
    nfa.alphabet = new char[nfa.alphabetCount]{'a', 'b', 'e'};
    nfa.stateCount = 3;
    nfa.states = new char[nfa.stateCount]{'0', '1', '2'};
    nfa.qo = '0';
    nfa.transitionCount = 4;
    nfa.delta = new transition[nfa.transitionCount]{
        {'0', 'e', '1'},
        {'0', 'e', '2'},
        {'1', 'a', '1'},
        {'2', 'b', '2'},
    };
    nfa.terminalCount = 2;
    nfa.stateterminal = new char[nfa.terminalCount]{'1', '2'};

    string report;
    automat dfa = convertNfaToDfa(nfa, report);
    cout << "=== NFA → DFA report ===\n" << report << "\n";
    cout << "DFA check: " << (isDFA(dfa) ? "DFA" : "Not DFA") << "\n\n";

    string report2;
    automat minDfa = minimizeDfa(dfa, report2);
    cout << "=== DFA minimization report ===\n" << report2 << "\n";
    cout << "Min DFA check: " << (isDFA(minDfa) ? "DFA" : "Not DFA") << "\n";

    // Cleanup
    delete[] nfa.alphabet;
    delete[] nfa.states;
    delete[] nfa.delta;
    delete[] nfa.stateterminal;

    delete[] dfa.alphabet;
    delete[] dfa.states;
    delete[] dfa.delta;
    delete[] dfa.stateterminal;

    delete[] minDfa.alphabet;
    delete[] minDfa.states;
    delete[] minDfa.delta;
    delete[] minDfa.stateterminal;

    delete[] automata;
    return 0;
}

