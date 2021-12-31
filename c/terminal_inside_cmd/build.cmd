@echo off
echo (0) - Compiling [33memulate.c[0m -^> emulate.o
gcc -Wall -Wextra -pedantic -std=c11 -funsigned-char -Werror=implicit-function-declaration -Werror=format-security -c emulate.c
echo (1) - Compiling [33mfont.c[0m -^> font.o
gcc -Wall -Wextra -pedantic -std=c11 -funsigned-char -Werror=implicit-function-declaration -Werror=format-security -c font.c
echo (@) - Linking [33memulate.o, font.o, [0m -^> emulate.exe
gcc emulate.o font.o -o emulate
