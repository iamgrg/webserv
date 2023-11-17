#!/bin/bash
for file in $(find . -name "*.cpp" -o -name "*.hpp"); do 
    echo $file; 
    clang-format -i "$file";
done

