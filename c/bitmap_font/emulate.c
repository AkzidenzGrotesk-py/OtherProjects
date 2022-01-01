#include <stdio.h>
#include <stdlib.h>

#include "font.h"

struct bmpsheet FONT;

int main() {
  system("cls");
  bmpsheet_empty(&FONT);
  bmpsheet_fromfile(&FONT, "bit8.bmpf");
  bmpsheet_srender(&FONT, "Hello,\n world!", 1, 1, '\xDB', ' ');
  bmpsheet_free(&FONT);
  return 0;
}

