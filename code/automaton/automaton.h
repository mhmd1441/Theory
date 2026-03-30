#pragma once
#include <iostream>
using namespace std;

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

extern automat *automata;
extern int automataCount;
extern int automataCapacity;
extern int nextAutomatonID;

bool findInArray(char c, char arr[], int len);
int findAutomatonIndexByID(int id);
void resizeAutomataArray();
void addAutomaton();
void deleteAutomaton();
void searchAutomaton();
bool simulate(automat &A, const char *input);
void testAutomaton();