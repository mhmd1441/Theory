#define _CRT_SECURE_NO_WARNINGS
#include "automaton.h"
#include <set>
#include <map>
#include <vector>
#include <sstream>
#include <algorithm>



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
        cout << "\nTransition Table:\nState";
        for (int a = 0; a < A.alphabetCount; a++)
                cout << "\t" << A.alphabet[a];
        cout << "\n";
        for (int s = 0; s < A.stateCount; s++)
        {
                cout << A.states[s];
                if (A.states[s] == A.qo)
                        cout << "(start)";
                if (findInArray(A.states[s], A.stateterminal, A.terminalCount))
                        cout << "(final)";
                for (int a = 0; a < A.alphabetCount; a++)
                {
                        cout << "\t";
                        bool found = false;
                        for (int t = 0; t < A.transitionCount; t++)
                        {
                                if (A.delta[t].origin == A.states[s] && A.delta[t].label == A.alphabet[a])
                                {
                                        if (found)
                                                cout << ",";
                                        cout << A.delta[t].target;
                                        found = true;
                                }
                        }
                        if (!found)
                                cout << "-";
                }
                cout << "\n";
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

        string trace;
        string errorMessage;
        bool accepted = simulateWithTrace(A, input, trace, errorMessage);

        cout << "\n" << trace;
        if (!errorMessage.empty())
                cout << "Simulation error: " << errorMessage << "\n";
        if (accepted)
                cout << "Result: STRING ACCEPTED by Automaton " << id << endl;
        else
                cout << "Result: STRING REJECTED by Automaton " << id << endl;
}

bool isDFA(const automat &A)
{
        // no epsilon transitions and no double definitions for same (origin,label)
        for (int i = 0; i < A.transitionCount; i++)
        {
                if (A.delta[i].label == 'e')
                        return false;
        }
        for (int i = 0; i < A.transitionCount; i++)
        {
                for (int j = i + 1; j < A.transitionCount; j++)
                {
                        if (A.delta[i].origin == A.delta[j].origin &&
                            A.delta[i].label == A.delta[j].label)
                        {
                                return false;
                        }
                }
        }
        return true;
}

static vector<char> alphabetWithoutEpsilon(const automat &A)
{
        vector<char> alpha;
        alpha.reserve(A.alphabetCount);
        for (int i = 0; i < A.alphabetCount; i++)
        {
                if (A.alphabet[i] != 'e')
                        alpha.push_back(A.alphabet[i]);
        }
        sort(alpha.begin(), alpha.end());
        alpha.erase(unique(alpha.begin(), alpha.end()), alpha.end());
        return alpha;
}

static set<char> epsilonClosure(const automat &A, const set<char> &start)
{
        set<char> closure = start;
        vector<char> stack;
        for (char s : start)
                stack.push_back(s);
        while (!stack.empty())
        {
                char s = stack.back();
                stack.pop_back();
                for (int i = 0; i < A.transitionCount; i++)
                {
                        if (A.delta[i].origin == s && A.delta[i].label == 'e')
                        {
                                char t = A.delta[i].target;
                                if (closure.insert(t).second)
                                        stack.push_back(t);
                        }
                }
        }
        return closure;
}

bool simulateWithTrace(const automat &A, const string &input, string &trace, string &errorMessage)
{
        ostringstream out;
        char current = A.qo;
        out << "Step-by-step simulation\n";
        out << "Input: " << input << "\n";
        out << "Start state: " << current << "\n\n";
        for (size_t i = 0; i < input.size(); i++)
        {
                char symbol = input[i];
                bool transitionFound = false;
                if (!findInArray(symbol, A.alphabet, A.alphabetCount))
                {
                        errorMessage = string("Symbol '") + symbol + "' not in alphabet";
                        out << "Error at step " << i << ": " << errorMessage << "\n";
                        trace = out.str();
                        return false;
                }
                for (int t = 0; t < A.transitionCount; t++)
                {
                        if (A.delta[t].origin == current && A.delta[t].label == symbol)
                        {
                                out << "Step " << i + 1 << ": read '" << symbol << "'\n";
                                out << current << " --" << symbol << "--> " << A.delta[t].target << "\n\n";
                                current = A.delta[t].target;
                                transitionFound = true;
                                break;
                        }
                }
                if (!transitionFound)
                {
                        errorMessage = string("No transition for state '") + current + "' with symbol '" + symbol + "'";
                        out << "Error at step " << i + 1 << ": " << errorMessage << "\n";
                        trace = out.str();
                        return false;
                }
        }
        bool accepted = findInArray(current, A.stateterminal, A.terminalCount);
        out << "End state: " << current << "\n";
        out << current << (accepted ? " is a final state\n" : " is not a final state\n");
        out << "Result: " << (accepted ? "ACCEPTED" : "REJECTED") << "\n";
        trace = out.str();
        errorMessage.clear();
        return accepted;
}

string epsilonClosureReport(const automat &A, const string &statesCsv)
{
        set<char> seed;
        for (char c : statesCsv)
        {
                if (c == ',' || c == ' ' || c == '\t' || c == '\n' || c == '\r')
                        continue;
                seed.insert(c);
        }
        set<char> cls = epsilonClosure(A, seed);
        ostringstream out;
        out << "epsilon-closure({";
        bool first = true;
        for (char c : seed) { if (!first) out << ","; out << c; first = false; }
        out << "}) = {";
        first = true;
        for (char c : cls) { if (!first) out << ","; out << c; first = false; }
        out << "}\n";
        return out.str();
}

static set<char> moveOnSymbol(const automat &A, const set<char> &states, char symbol)
{
        set<char> result;
        // NOTE: Do not iterate directly over `states` if `result` might alias it (future refactors).
        // Copy to a stable container to avoid any iterator invalidation surprises.
        vector<char> st(states.begin(), states.end());
        for (char s : st)
        {
                for (int i = 0; i < A.transitionCount; i++)
                {
                        if (A.delta[i].origin == s && A.delta[i].label == symbol)
                                result.insert(A.delta[i].target);
                }
        }
        return result;
}

static string subsetKey(const set<char> &s)
{
        string k;
        for (char c : s)
                k.push_back(c);
        return k;
}

static bool subsetIsFinal(const automat &A, const set<char> &subset)
{
        for (int i = 0; i < A.terminalCount; i++)
        {
                if (subset.count(A.stateterminal[i]) > 0)
                        return true;
        }
        return false;
}

automat convertNfaToDfa(const automat &nfa, string &report)
{
        report.clear();
        ostringstream out;

        vector<char> alpha = alphabetWithoutEpsilon(nfa);
        if (alpha.empty())
                out << "Warning: alphabet is empty after removing epsilon 'e'.\n";

        // BFS over subsets
        map<string, char> subsetToName;
        vector<set<char>> subsets;
        vector<char> names;

        set<char> start0;
        start0.insert(nfa.qo);
        set<char> start = epsilonClosure(nfa, start0);
        string startKey = subsetKey(start);

        char nextName = 'A';
        subsetToName[startKey] = nextName++;
        subsets.push_back(start);
        names.push_back(subsetToName[startKey]);

        out << "NFA→DFA subset construction\n";
        out << "Start subset: {" << startKey << "} => " << subsetToName[startKey] << "\n\n";

        vector<transition> dfaTransitions;

        for (size_t idx = 0; idx < subsets.size(); idx++)
        {
                if (subsets.size() > 1024)
                {
                        out << "Abort: too many DFA subset-states (>1024). Check NFA definition.\n";
                        break;
                }
                // IMPORTANT: do not keep a reference into `subsets` while we may push_back
                // new subsets (vector reallocation would invalidate references).
                set<char> S = subsets[idx];
                char fromName = subsetToName[subsetKey(S)];
                for (char c : alpha)
                {
                        set<char> m = moveOnSymbol(nfa, S, c);
                        set<char> dest = epsilonClosure(nfa, m);
                        if (dest.empty())
                                continue;
                        string dk = subsetKey(dest);
                        if (!subsetToName.count(dk))
                        {
                                if (subsetToName.size() > 1024)
                                {
                                        out << "Abort: too many subset states (>1024).\n";
                                        break;
                                }
                                subsetToName[dk] = nextName <= 'Z' ? nextName++ : nextName++; // still increments, but may go past 'Z'
                                subsets.push_back(dest);
                                names.push_back(subsetToName[dk]);
                        }
                        transition tr;
                        tr.origin = fromName;
                        tr.label = c;
                        tr.target = subsetToName[dk];
                        dfaTransitions.push_back(tr);
                }
        }

        // Build DFA automat
        automat dfa;
        dfa.id = nextAutomatonID++;
        dfa.alphabetCount = (int)alpha.size();
        dfa.alphabet = new char[dfa.alphabetCount];
        for (int i = 0; i < dfa.alphabetCount; i++)
                dfa.alphabet[i] = alpha[i];

        dfa.stateCount = (int)names.size();
        dfa.states = new char[dfa.stateCount];
        for (int i = 0; i < dfa.stateCount; i++)
                dfa.states[i] = names[i];

        dfa.qo = subsetToName[startKey];

        dfa.transitionCount = (int)dfaTransitions.size();
        dfa.delta = new transition[dfa.transitionCount];
        for (int i = 0; i < dfa.transitionCount; i++)
                dfa.delta[i] = dfaTransitions[i];

        vector<char> finals;
        for (const auto &kv : subsetToName)
        {
                // reconstruct subset by key
                set<char> subset;
                for (char c : kv.first)
                        subset.insert(c);
                if (subsetIsFinal(nfa, subset))
                        finals.push_back(kv.second);
        }
        sort(finals.begin(), finals.end());
        finals.erase(unique(finals.begin(), finals.end()), finals.end());
        dfa.terminalCount = (int)finals.size();
        dfa.stateterminal = new char[dfa.terminalCount];
        for (int i = 0; i < dfa.terminalCount; i++)
                dfa.stateterminal[i] = finals[i];

        out << "DFA states mapping:\n";
        for (const auto &kv : subsetToName)
        {
                out << "  " << kv.second << " = {" << kv.first << "}";
                if (kv.second == dfa.qo)
                        out << "  (start)";
                out << "\n";
        }
        out << "\nSubset construction step-table:\n";
        out << "  State  | subset\n";
        out << "  ----------------\n";
        for (const auto &kv : subsetToName)
        {
                out << "  " << kv.second << "      | {" << kv.first << "}\n";
        }
        out << "\nDFA transitions:\n";
        for (int i = 0; i < dfa.transitionCount; i++)
        {
                out << "  " << dfa.delta[i].origin << " --" << dfa.delta[i].label << "--> " << dfa.delta[i].target << "\n";
        }
        out << "\nDFA final states: ";
        for (int i = 0; i < dfa.terminalCount; i++)
                out << dfa.stateterminal[i] << " ";
        out << "\n";

        report = out.str();
        return dfa;
}

static int stateIndex(const automat &A, char s)
{
        for (int i = 0; i < A.stateCount; i++)
                if (A.states[i] == s)
                        return i;
        return -1;
}

static int dfaMoveIndex(const automat &A, int stateIdx, char symbol)
{
        char origin = A.states[stateIdx];
        for (int i = 0; i < A.transitionCount; i++)
        {
                if (A.delta[i].origin == origin && A.delta[i].label == symbol)
                        return stateIndex(A, A.delta[i].target);
        }
        return -1;
}

static int dfaMoveIndexByState(const automat &A, char state, char symbol)
{
        for (int i = 0; i < A.transitionCount; i++)
        {
                if (A.delta[i].origin == state && A.delta[i].label == symbol)
                        return stateIndex(A, A.delta[i].target);
        }
        return -1;
}

bool areDfaEquivalentExact(const automat &a, const automat &b, string &report)
{
        ostringstream out;
        if (!isDFA(a) || !isDFA(b))
        {
                out << "Both automata must be DFAs for exact equivalence.\n";
                report = out.str();
                return false;
        }

        vector<char> alpha = alphabetWithoutEpsilon(a);
        vector<char> beta = alphabetWithoutEpsilon(b);
        for (char c : beta)
                if (find(alpha.begin(), alpha.end(), c) == alpha.end())
                        alpha.push_back(c);
        sort(alpha.begin(), alpha.end());

        int s0 = stateIndex(a, a.qo);
        int t0 = stateIndex(b, b.qo);
        if (s0 < 0 || t0 < 0)
        {
                out << "Invalid initial state.\n";
                report = out.str();
                return false;
        }

        set<pair<int, int>> visited;
        vector<pair<int, int>> q;
        map<pair<int, int>, pair<int, int>> parent;
        map<pair<int, int>, char> parentSym;
        q.push_back({s0, t0});
        visited.insert({s0, t0});

        size_t head = 0;
        const size_t kMaxVisited = 200000;
        while (head < q.size())
        {
                auto cur = q[head++];
                bool finA = findInArray(a.states[cur.first], a.stateterminal, a.terminalCount);
                bool finB = findInArray(b.states[cur.second], b.stateterminal, b.terminalCount);
                if (finA != finB)
                {
                        // reconstruct witness
                        string witness;
                        auto p = cur;
                        while (!(p.first == s0 && p.second == t0))
                        {
                                char c = parentSym[p];
                                witness.push_back(c);
                                p = parent[p];
                        }
                        reverse(witness.begin(), witness.end());
                        out << "Not equivalent. Witness string: '" << witness << "'\n";
                        report = out.str();
                        return false;
                }
                for (char c : alpha)
                {
                        int na = dfaMoveIndex(a, cur.first, c);
                        int nb = dfaMoveIndex(b, cur.second, c);
                        // totalize by dead state (-1)
                        pair<int, int> nxt = {na, nb};
                        if (!visited.count(nxt))
                        {
                                visited.insert(nxt);
                                parent[nxt] = cur;
                                parentSym[nxt] = c;
                                q.push_back(nxt);
                                if (visited.size() > kMaxVisited)
                                {
                                        out << "Aborted equivalence check: too many product states.\n";
                                        report = out.str();
                                        return false;
                                }
                        }
                }
        }
        out << "Equivalent (exact product-state check).\n";
        report = out.str();
        return true;
}

automat minimizeDfa(const automat &dfa, string &report)
{
        report.clear();
        ostringstream out;
        if (!isDFA(dfa))
        {
            out << "Minimization requires a DFA (no epsilon, no double definitions).\n";
            report = out.str();
            // Return a shallow copy-like new automaton (same as input) is risky; instead build a copy.
        }

        vector<char> alpha = alphabetWithoutEpsilon(dfa);

        // Initial partition: finals vs non-finals
        vector<int> group(dfa.stateCount, 0);
        for (int i = 0; i < dfa.stateCount; i++)
        {
                if (findInArray(dfa.states[i], dfa.stateterminal, dfa.terminalCount))
                        group[i] = 1;
        }

        bool changed = true;
        int iterGuard = 0;
        while (changed && iterGuard < 200)
        {
                iterGuard++;
                changed = false;
                map<string, int> signatureToNewGroup;
                vector<int> newGroup(dfa.stateCount, 0);
                int nextG = 0;
                for (int i = 0; i < dfa.stateCount; i++)
                {
                        // signature: current group + for each symbol target group
                        string sig = to_string(group[i]) + "|";
                        for (char c : alpha)
                        {
                                int t = dfaMoveIndex(dfa, i, c);
                                sig += (t >= 0 ? to_string(group[t]) : string("X"));
                                sig += ",";
                        }
                        if (!signatureToNewGroup.count(sig))
                                signatureToNewGroup[sig] = nextG++;
                        newGroup[i] = signatureToNewGroup[sig];
                }
                if (newGroup != group)
                {
                        group = newGroup;
                        changed = true;
                }
        }
        if (iterGuard >= 200)
        {
                out << "Warning: minimization refinement hit iteration limit (200). Result may be incomplete.\n";
        }

        int groupCount = 0;
        for (int g : group)
                groupCount = max(groupCount, g + 1);

        // Name each group with a letter
        vector<char> groupName(groupCount, 0);
        for (int g = 0; g < groupCount; g++)
                groupName[g] = (g < 26) ? (char)('A' + g) : (char)('a' + (g - 26) % 26);

        // Build minimized DFA
        automat minDfa;
        minDfa.id = nextAutomatonID++;
        minDfa.alphabetCount = (int)alpha.size();
        minDfa.alphabet = new char[minDfa.alphabetCount];
        for (int i = 0; i < minDfa.alphabetCount; i++)
                minDfa.alphabet[i] = alpha[i];

        minDfa.stateCount = groupCount;
        minDfa.states = new char[minDfa.stateCount];
        for (int i = 0; i < minDfa.stateCount; i++)
                minDfa.states[i] = groupName[i];

        int startIdx = stateIndex(dfa, dfa.qo);
        minDfa.qo = groupName[group[startIdx]];

        // transitions: for each group representative, define moves
        vector<transition> newTrans;
        for (int g = 0; g < groupCount; g++)
        {
                int rep = -1;
                for (int i = 0; i < dfa.stateCount; i++)
                        if (group[i] == g) { rep = i; break; }
                if (rep < 0) continue;
                for (char c : alpha)
                {
                        int t = dfaMoveIndex(dfa, rep, c);
                        if (t < 0) continue;
                        transition tr;
                        tr.origin = groupName[g];
                        tr.label = c;
                        tr.target = groupName[group[t]];
                        newTrans.push_back(tr);
                }
        }

        // dedupe transitions
        sort(newTrans.begin(), newTrans.end(), [](const transition &a, const transition &b) {
                if (a.origin != b.origin) return a.origin < b.origin;
                if (a.label != b.label) return a.label < b.label;
                return a.target < b.target;
        });
        newTrans.erase(unique(newTrans.begin(), newTrans.end(), [](const transition &a, const transition &b) {
                return a.origin == b.origin && a.label == b.label && a.target == b.target;
        }), newTrans.end());

        minDfa.transitionCount = (int)newTrans.size();
        minDfa.delta = new transition[minDfa.transitionCount];
        for (int i = 0; i < minDfa.transitionCount; i++)
                minDfa.delta[i] = newTrans[i];

        // final groups: any group containing a final state
        vector<char> minFinals;
        for (int i = 0; i < dfa.stateCount; i++)
        {
                if (findInArray(dfa.states[i], dfa.stateterminal, dfa.terminalCount))
                        minFinals.push_back(groupName[group[i]]);
        }
        sort(minFinals.begin(), minFinals.end());
        minFinals.erase(unique(minFinals.begin(), minFinals.end()), minFinals.end());
        minDfa.terminalCount = (int)minFinals.size();
        minDfa.stateterminal = new char[minDfa.terminalCount];
        for (int i = 0; i < minDfa.terminalCount; i++)
                minDfa.stateterminal[i] = minFinals[i];

        out << "DFA minimization result\n";
        out << "Groups (old -> new):\n";
        for (int i = 0; i < dfa.stateCount; i++)
        {
                out << "  " << dfa.states[i] << " -> " << groupName[group[i]] << "\n";
        }
        out << "Minimized states: ";
        for (int i = 0; i < minDfa.stateCount; i++)
                out << minDfa.states[i] << " ";
        out << "\nMinimized final states: ";
        for (int i = 0; i < minDfa.terminalCount; i++)
                out << minDfa.stateterminal[i] << " ";
        out << "\n";

        report = out.str();
        return minDfa;
}
