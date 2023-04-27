all:
	gcc -I./include -L./lib -L. -o main.exe src/main.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf