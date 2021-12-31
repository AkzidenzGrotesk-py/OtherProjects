#include <stdio.h>
#include <stdlib.h>

#include "font.h"

struct bmpsheet FONT;

int main() {
  bmpsheet_empty(&FONT);
  bmpsheet_fromfile(&FONT, ".bmpf");
  bmpsheet_srender(&FONT, "abcdef", 1, 1, '#', ' ');
  return 0;
}

