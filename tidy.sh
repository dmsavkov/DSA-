#!/bin/bash

set -eo pipefail

CHECKS='-*,clang-analyzer-*,performance-*,bugprone-*'
REPO_ROOT="$(git rev-parse --show-toplevel)"
cd "$REPO_ROOT"

TASK_DIR="${1:-}"
if [[ -z "$TASK_DIR" ]]; then
    echo "Usage: tidy.sh <task-directory>" >&2
    exit 1
fi

TASK_REL="$(realpath --relative-to="$REPO_ROOT" "$TASK_DIR")"
FILES=$(git ls-files "$TASK_REL" | grep -E '\.(cpp|cxx|cc)$' || true)

if [[ -z "$FILES" ]]; then
    echo "No source files to analyze under ${TASK_REL}."
    exit 0
fi

COMPILE_DB=()
if [[ -f "$TASK_DIR/compile_commands.json" ]]; then
    COMPILE_DB=(-p "$TASK_DIR")
elif [[ -f "$REPO_ROOT/compile_commands.json" ]]; then
    COMPILE_DB=(-p "$REPO_ROOT")
else
    echo "compile_commands.json not found; running clang-tidy with -std=c++17"
fi

EIGEN_INC="${REPO_ROOT}/external/eigen"
LAZY_INC="${REPO_ROOT}/external/lazycsv/include"

for f in $FILES; do
    abs_file="$REPO_ROOT/$f"
    echo "Running clang-tidy on $f"
    clang-tidy "${COMPILE_DB[@]}" -checks="$CHECKS" "$abs_file" \
        --extra-arg="-std=c++17" \
        --extra-arg="-isystem${EIGEN_INC}" \
        --extra-arg="-isystem${LAZY_INC}" \
        --quiet 2>&1
done
