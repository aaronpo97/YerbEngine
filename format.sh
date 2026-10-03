#!/bin/bash
#
# Runs clang-format over the engine, the games, and the tests.
#
# Usage: ./format.sh [--check]
#   --check  report files that need formatting and exit non-zero, leaving
#            them untouched (for CI)

set -euo pipefail

cd "$(dirname "$0")"

SOURCE_DIRS="lib pong-demo shoot-demo tests"

if [ ! -f .clang-format ]; then
    echo "ERROR: .clang-format file not found."
    exit 1
fi

if ! command -v clang-format &>/dev/null; then
    echo "ERROR: clang-format not found."
    exit 1
fi

find_sources() {
    find $SOURCE_DIRS -type f \
        \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" \
           -o -name "*.cc" -o -name "*.cxx" \) -print0
}

if [[ "${1:-}" == "--check" ]]; then
    find_sources | xargs -0 clang-format --dry-run --Werror
    echo "Sources in $SOURCE_DIRS are formatted correctly."
    exit 0
fi

find_sources | xargs -0 clang-format -i
echo "Formatted sources in: $SOURCE_DIRS"
