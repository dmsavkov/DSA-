#!/bin/bash

set -eo pipefail

CHECKS='-*,clang-analyzer-*,performance-*,bugprone-*'
REPO_ROOT="$(git rev-parse --show-toplevel)"
cd "$REPO_ROOT"

FILES=$(git ls-files '*.cpp' '*.cxx' '*.cc' | tr '\n' ' ')

if [[ -z "$FILES" ]]; then
  echo "No source files to analyze."
  exit 0
fi

COMPILE_DB=()
if [[ -f compile_commands.json ]]; then
  COMPILE_DB=(-p "$REPO_ROOT")
else
  echo "compile_commands.json not found; running clang-tidy with -std=c++17"
fi

for f in $FILES; do
  echo "Running clang-tidy on $f"
  if ((${#COMPILE_DB[@]})); then
    clang-tidy "${COMPILE_DB[@]}" -checks="$CHECKS" "$f" --quiet 2>&1
  else
    clang-tidy -checks="$CHECKS" "$f" --quiet -- -std=c++17 2>&1
  fi
done
