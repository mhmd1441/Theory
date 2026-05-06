# Final Demo Checklist (2-3 minutes)

## 1) Automaton Management
- Add a sample automaton (`states`, `alphabet`, `transitions`, `final states`).
- Show the **drawn automaton graph** (states + arrows + labels).
- Run **Test Automaton** with an accepted and rejected input.

## 2) Automaton Transformations
- Click **Convert NFA→DFA** and show generated report.
- Click **Minimize DFA** and show minimized-state report.
- (Optional) Use **Check DFA Equivalence** for two DFAs.

## 3) Program Analysis Pipeline
- Load example program and run:
  - `1) Lex`
  - `2) Parse`
  - `3) Semantic`
  - `4) Codegen`
- Show:
  - token list with line/column
  - parse tree text + interactive parse tree
  - symbol table (`name/type/scope`)
  - generated pseudo-Python code

## 4) Error Handling
- Click **Load Error Demo**.
- Run analysis and show readable parse/semantic diagnostics.

## 5) Verification Proof
- Run:
  - `./scripts/verify_all.sh`
- Mention this checks: build + tests + console smoke + automaton transform demo.

