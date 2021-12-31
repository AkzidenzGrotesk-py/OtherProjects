#define __STDC_WANT_LIBC_EXT1__
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>

#include "font.h"

struct bmpsheet FONT;
char CURRENT;
int curX, curY;
const int FPS = 1000 / 10;
bool active = true;

void interaction() {
  // quit
  if (GetAsyncKeyState((unsigned short)'Q') & 0x8000)
    active = false;

  // move cursor
  if (curX > 0 && GetAsyncKeyState((unsigned short)'A') & 0x8000)
    curX--;
  if (curX < 7 && GetAsyncKeyState((unsigned short)'D') & 0x8000)
    curX++;
  if (curY < 7 && GetAsyncKeyState((unsigned short)'S') & 0x8000)
    curY++;
  if (curY > 0 && GetAsyncKeyState((unsigned short)'W') & 0x8000)
    curY--;

  // paint
  if (GetAsyncKeyState((unsigned short)'J') & 0x8000) {
    char bh[8] = {0};
    binchar(bh, FONT.s[(int)CURRENT]->l[curY]);
    bh[curX] = 1;
    for (int i = 0; i < 8; i++)
      bh[i] = bh[i] == 1 ? '1' : '0';
    FONT.s[(int)CURRENT]->l[curY] = (char) strtol(bh, NULL, 2);
  }
  if (GetAsyncKeyState((unsigned short)'K') & 0x8000) {
    char bh[8] = {0};
    binchar(bh, FONT.s[(int)CURRENT]->l[curY]);
    bh[curX] = 0;
    for (int i = 0; i < 8; i++)
      bh[i] = bh[i] == 1 ? '1' : '0';
    FONT.s[(int)CURRENT]->l[curY] = (char) strtol(bh, NULL, 2);
  }

  // change char
  if ((int)CURRENT < 255 && GetAsyncKeyState((unsigned short)'N') & 0x8000)
    CURRENT++;
  if ((int)CURRENT > 0 && GetAsyncKeyState((unsigned short)'M') & 0x8000)
    CURRENT--;

  // clear canvas
  if (GetAsyncKeyState((unsigned short)'C') & 0x8000)
    for (int i = 0; i < 8; i++)
      FONT.s[(int)CURRENT]->l[i] = 0;

  if (FONT.s[(int)CURRENT] == NULL)
    FONT.s[(int)CURRENT] = bmpchar_make();

  // save
  if (GetAsyncKeyState((unsigned short)'E') & 0x8000) {
    printf("Attempting to save to (.bmpf)\n");
    if (bmpsheet_tofile(&FONT, ".bmpf"))
      printf("Failed to save.");
    printf("Done.\n");
  }
  if (GetAsyncKeyState((unsigned short)'O') & 0x8000) {
    printf("Attempting to load from (.bmpf)\n");
    if (bmpsheet_fromfile(&FONT, ".bmpf"))
      printf("Failed to load.");
    printf("Done.\n");
  }
}

void render() {
  printf("\033[H*: %c (%i)      \n", CURRENT, CURRENT);
  for (int y = 0; y < 8; y++) {
    char bh[8] = {0};
    binchar(bh, FONT.s[(int)CURRENT]->l[y]);
    for (int x = 0; x < 8; x++) {
      if (x == curX && y == curY) printf("\033[7m");
      if (bh[x] == 1) printf("#");
      else printf(".");
      printf("\033[27m ");
    }
    printf("\n");
  }
  printf("= = = = = = = =\n");
}

int main() {
  printf("\033[1J\033[?25l");
  bmpsheet_empty(&FONT);
  CURRENT = 'h';
  curX = 0; curY = 0;
  while (active) {
    interaction();
    render();
    Sleep(FPS);
  }
  bmpsheet_free(&FONT);
  printf("\033[?25h");
  return 0;
}

