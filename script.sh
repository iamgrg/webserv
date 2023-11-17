for file in $(find . -name "*.cpp" -o -name "*.hpp"); do 
    echo $file; 
    grep "#include" $file; 
    echo ""; 
done
