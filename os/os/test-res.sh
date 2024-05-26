gcc -o pizza_shop 3220178-3220280-3220216-pizza.c -pthread

if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

./pizza_shop 100 10 > result.txt

if [ $? -ne 0 ]; then
    echo "Execution failed"
    exit 1
fi

cat result.txt
