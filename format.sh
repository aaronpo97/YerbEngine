#!/bin/bash
echo "WARNING: This script will format all .cpp, .h, .c, .hpp files in the YerbEngine, src, and demo directories."
echo "This script will overwrite the files with the formatted version."

read -p "Do you want to continue? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    echo "Aborted."
    exit 1
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

find YerbEngine src -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" | xargs clang-format -i

if [ -d "shoot-demo" ]; then
    find shoot-demo -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.c" | xargs clang-format -i
fi


echo "Done."

exit 0
