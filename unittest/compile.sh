gcc --std=c99 -Wall -c node_test.c -o node_test.o
gcc --std=c99 -Wall node_test.o ../object/node.o -o a.exe
