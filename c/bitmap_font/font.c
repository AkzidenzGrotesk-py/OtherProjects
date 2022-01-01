#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "font.h"

char *binchar(char *c, unsigned char n) {
  // static char c[8] = {0,0,0,0,0,0,0,0};
  int i = 7;
  while (n > 0) {c[i--] = n % 2;n /= 2;}
  return c;
}

struct bmpchar *bmpchar_make() {
  struct bmpchar* b = (struct bmpchar *) malloc(sizeof(struct bmpchar));
  for (int i = 0; i < 8; i++)
    b->l[i] = 0;
  return b;
}

void bmpchar_render(struct bmpchar *self, int column, int row, char on, char off) {
  for (int i = 0; i < 8; i++) {
    char bh[8] = {0};
    binchar(bh, self->l[i]);
    for (int j = 0; j < 8; j++)
      printf("\033[%i;%iH%c", row+i, column+j, bh[j] ? on : off);
    printf("\n");
  }
}

void bmpsheet_empty(struct bmpsheet *self) {
  for (int i = 0; i < 256; i++)
    for (int j = 0; j < 8; j++)
      self->s[i] = NULL;
}

void bmpsheet_crender(struct bmpsheet *self, char c, int column, int row, char on, char off) {
  if (self->s[(int)c] == NULL) return;
  bmpchar_render(self->s[(int)c], column, row, on, off);
}

void bmpsheet_srender(struct bmpsheet *self, char *str, int column, int row, int maxcol, char on, char off) {
  int ocol = column;
  for (int i = 0; i < (int) strlen(str); i++) {
    if (str[i] == '\n') {row += 8; column = ocol; continue;}
    bmpsheet_crender(self, str[i], column, row, on, off);
    column += 8;
    if (column > maxcol) {column = ocol; row += 8;}
  }
}

int bmpsheet_tofile(struct bmpsheet *self, char *name) {
  FILE *f;
  if (fopen_s(&f, name, "w"))
    return 1;
  for (int i = 0; i < 256; i++) {
    if (self->s[i] != NULL) {
      fprintf_s(f, "%c", (char)i);
      for (int j = 0; j < 8; j++)
        fprintf_s(f, "%c", self->s[i]->l[j]);
      fprintf_s(f, "\n");
    }
  }
  fclose(f);
  return 0;
}

int bmpsheet_fromfile(struct bmpsheet *buf, char *name) {
  FILE *f;
  char line[32];
  if (fopen_s(&f, name, "rb"))
    return 1;
  while (fgets(line, sizeof(line), f)) {
    if (line[0] == '\n') break;
    if (buf->s[(int)line[0]] == NULL)
      buf->s[(int)line[0]] = bmpchar_make();
    for (int i = 1; i < 9; i++)
      buf->s[(int)line[0]]->l[i-1] = line[i];
  }
  fclose(f);
  return 0;
}

void bmpsheet_free(struct bmpsheet* self) {
  for (int i = 0; i < 256; i++)
    if (self->s[i] != NULL)
      free(self->s[i]);
}

/* int main() {
  struct bmpchar A = { 24,24,60,36,102,126,195,129 };
  bmpchar_render(&A);
  return 0;
} */
