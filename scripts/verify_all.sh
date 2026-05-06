#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

echo "[1/5] Configure project"
cmake -S . -B build

echo "[2/5] Build all targets"
cmake --build build -j4

echo "[3/5] Run automated tests (CTest)"
ctest --test-dir build --output-on-failure

echo "[4/5] Build and smoke-test console app"
g++ -std=c++17 \
  TheoryFinalProject.cpp \
  code/automaton/automaton.cpp \
  code/lexer/lexer.cpp \
  code/parser/parser.cpp \
  code/semantic/semantic.cpp \
  -o build/TheoryConsole

# Smoke test: launch and immediately exit menu with option 6.
printf "6\n" | ./build/TheoryConsole >/dev/null

echo "[5/5] Run automaton transformation demo"
./build/AutomatonTransformDemo

echo
echo "Verification completed successfully."
