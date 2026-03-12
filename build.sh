rm -f *.o main
gcc -I./headers src/*.c src/clientsCommands/*.c src/serverCommands/*.c main.c -o main