#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Takes file 'file.c' and converts to 'file.o'
char *dotc_doto(char *filename) {
  int sz = (int)strlen(filename);
  char *copy = malloc((sz+1) * sizeof(char));
  strcpy_s(copy, (sz+1) * sizeof(char), filename);
  copy[sz-1] = 'o';
  return copy;
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printf("\033[31marguments required:\033[0m output_name, executable_name and files...");
    exit(1);
  }

  char *output_name = argv[1];
  char *exec_name = argv[2];
  char *standard_opts = "-Wall -Wextra -pedantic -std=c11 -funsigned-char -Werror=implicit-function-declaration -Werror=format-security";

  printf("Attempting to open file %s", output_name);
  FILE *outfile;
  if (fopen_s(&outfile, output_name, "w") != 0) {
    printf(",\033[31m failed.\033[0m\n");
    exit(1);
  }
  printf(", done.\n");
  fprintf_s(outfile, "@echo off\n");
  if (argc == 4) {
    // Single file, skipping compilation to object file
    printf("Generating line for %s -> %s.exe", argv[3], exec_name);
    fprintf_s(outfile, "echo (@) - Compiling \033[33m%s\033[0m -^> %s.exe\n",
              argv[3], exec_name);
    fprintf_s(outfile, "gcc %s -o %s %s\n",
              argv[3], exec_name, standard_opts);
    printf(", done.\n");
  } else {
    // Multiple files, compile to object and link
    char *objn[argc-3];
    for (int i = 3; i < argc; i++) {
      printf("Generating line for %s -> %s", argv[i], dotc_doto(argv[i]));
      fprintf_s(outfile, "echo (%i) - Compiling \033[33m%s\033[0m -^> %s\n",
          i - 3, argv[i], dotc_doto(argv[i]));
      fprintf_s(outfile, "gcc %s -c %s\n",
                standard_opts, argv[i]);
      objn[i - 3] = dotc_doto(argv[i]);
      printf(", done.\n");
    }
    printf("Generating linkage line -> %s.exe", exec_name);
    fprintf_s(outfile, "echo (@) - Linking \033[33m");
    for (int i = 0; i < argc - 3; i++)
      fprintf_s(outfile, "%s, ", objn[i]);
    fprintf_s(outfile, "\033[0m -^> %s.exe\n",
              exec_name);
    fprintf_s(outfile, "gcc ");
    for (int i = 0; i < argc - 3; i++)
      fprintf_s(outfile, "%s ", objn[i]);
    fprintf_s(outfile, "-o %s\n",
              exec_name);
    printf(", done.\n");
  }
  fclose(outfile);
  printf("Completed.\n");

  return 0;
}
