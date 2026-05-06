#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

OUT_DIR="submission_bundle"
ZIP_NAME="Theory_Submission.zip"

rm -rf "$OUT_DIR" "$ZIP_NAME"
mkdir -p "$OUT_DIR"

# Core source and build config
cp -R code "$OUT_DIR/"
cp -R scripts "$OUT_DIR/"
cp -R tests "$OUT_DIR/"
cp -R tools "$OUT_DIR/"
cp CMakeLists.txt "$OUT_DIR/"
cp TheoryFinalProject.cpp "$OUT_DIR/"
cp TheoryGUI_single.cpp "$OUT_DIR/"
cp .gitignore "$OUT_DIR/"

# Docs useful for grading and running
cp RUN_INSTRUCTIONS.md "$OUT_DIR/"
cp build_instructions.md "$OUT_DIR/"

# Optional IDE/project files (keep if instructor builds on VS)
cp TheoryFinalProject.sln "$OUT_DIR/" 2>/dev/null || true
cp TheoryFinalProject.vcxproj "$OUT_DIR/" 2>/dev/null || true
cp TheoryFinalProject.vcxproj.filters "$OUT_DIR/" 2>/dev/null || true

# Create zip archive
(
  cd "$OUT_DIR"
  zip -r "../$ZIP_NAME" . >/dev/null
)

echo "Submission package created:"
echo "  Directory: $ROOT_DIR/$OUT_DIR"
echo "  Archive:   $ROOT_DIR/$ZIP_NAME"
