#!/bin/bash

# Check for -y flag
SKIP_CONFIRM=false
if [[ "$1" == "-y" ]]; then
    SKIP_CONFIRM=true
fi

echo "WARNING: This script will format all .cpp, .h, .c, .hpp files in the lib, tests, and shoot-demo directories."
echo "This script will overwrite the files with the formatted version."

if [[ "$SKIP_CONFIRM" == false ]]; then
    read -p "Do you want to continue? (y/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Aborted."
        exit 1
    fi
fi

if [ ! -f .clang-format ]; then
    echo "ERROR: .clang-format file not found."
    exit 1
fi

if ! command -v clang-format &>/dev/null; then
    echo "ERROR: clang-format not found."
    exit 1
fi

echo "Formatting files..."

find lib/includes lib/src \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" \) | xargs clang-format -i

if [ -d "shoot-demo" ]; then
    find shoot-demo \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" \) | xargs clang-format -i
fi

if [ -d "tests" ]; then
    find tests \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" \) | xargs clang-format -i
fi

echo "Done."

exit 0
