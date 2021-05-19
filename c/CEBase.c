#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// ******************* Customs ***************** //

// Screen Size
const int SCREEN_WIDTH = 200;
const int SCREEN_HEIGHT = 200;
CHAR_INFO SCREEN_ARR[40000]; // SCREEN_WIDTH * SCREEN_HEIGHT

// Pixel (text) Size
const int FONT_WIDTH = 4;
const int FONT_HEIGHT = 4;

// Window Title
char * SCREEN_TITLE = "Default";

// *******************  Globals ***************** //
struct KeyState {
  int pressed;
  int released;
  int held;
};

HANDLE CONSOLE_IN, CONSOLE_OUT;
CONSOLE_SCREEN_BUFFER_INFO csbi;
SMALL_RECT WINDOW_RECT = { 0, 0, (short)SCREEN_WIDTH - 1, (short)SCREEN_HEIGHT - 1 };
int ACTIVE = 1;
double tp1, tp2, elapsedTime, DELTATIME;
struct KeyState KEYS[256];
struct KeyState MOUSE[5];
short KEYS_OLD[256];
short KEYS_NEW[256];
int MOUSE_OLD[5];
int MOUSE_NEW[5];
int MOUSEX;
int MOUSEY;
int CONSOLE_FOCUS;

// *********** FORMATTING ********************/
const short FG_BLACK            = 0x0000;
const short FG_DARK_BLUE        = 0x0001;
const short FG_DARK_GREEN       = 0x0002;
const short FG_DARK_CYAN        = 0x0003;
const short FG_DARK_RED         = 0x0004;
const short FG_DARK_MAGENTA     = 0x0005;
const short FG_DARK_YELLOW      = 0x0006;
const short FG_GREY             = 0x0007;
const short FG_DARK_GREY        = 0x0008;
const short FG_BLUE             = 0x0009;
const short FG_GREEN            = 0x000A;
const short FG_CYAN             = 0x000B;
const short FG_RED              = 0x000C;
const short FG_MAGENTA          = 0x000D;
const short FG_YELLOW           = 0x000E;
const short FG_WHITE            = 0x000F;
const short BG_BLACK            = 0x0000;
const short BG_DARK_BLUE        = 0x0010;
const short BG_DARK_GREEN       = 0x0020;
const short BG_DARK_CYAN        = 0x0030;
const short BG_DARK_RED         = 0x0040;
const short BG_DARK_MAGENTA     = 0x0050;
const short BG_DARK_YELLOW      = 0x0060;
const short BG_GREY             = 0x0070;
const short BG_DARK_GREY        = 0x0080;
const short BG_BLUE             = 0x0090;
const short BG_GREEN            = 0x00A0;
const short BG_CYAN             = 0x00B0;
const short BG_RED              = 0x00C0;
const short BG_MAGENTA          = 0x00D0;
const short BG_YELLOW           = 0x00E0;
const short BG_WHITE            = 0x00F0;
const char PIXEL_SOLID          = '\xdb';
const char PIXEL_THREEQUARTERS  = '\xb2';
const char PIXEL_HALF           = '\xb1';
const char PIXEL_QUARTER        = '\xb0';

// ****************** Engine Functions ************************* //
// Initialize Engine - VERY VERY VERY STOLEN 100% STOLEN (converted from C++) >>> https://github.com/OneLoneCoder/videos/blob/master/olcConsoleGameEngine.h
void construct() {
  // Create Console
  CONSOLE_IN = GetStdHandle(STD_INPUT_HANDLE);
  CONSOLE_OUT = GetStdHandle(STD_OUTPUT_HANDLE);

  //SMALL_RECT WINDOW_RECT = { 0, 0, (short)SCREEN_WIDTH - 1, (short)SCREEN_HEIGHT - 1 };
  SetConsoleWindowInfo(CONSOLE_OUT, TRUE, &WINDOW_RECT);
  COORD coord = { (short)SCREEN_WIDTH, (short)SCREEN_HEIGHT };
  SetConsoleScreenBufferSize(CONSOLE_OUT, coord);
  SetConsoleActiveScreenBuffer(CONSOLE_OUT);

  CONSOLE_FONT_INFOEX cfi;
  cfi.cbSize = sizeof(cfi);
  cfi.nFont = 0;
  cfi.dwFontSize.X = FONT_WIDTH;
  cfi.dwFontSize.Y = FONT_HEIGHT;
  cfi.FontFamily = FF_DONTCARE;
  cfi.FontWeight = FW_NORMAL;

  wchar_t * font = L"Consolas";
  wcscpy_s(cfi.FaceName, _countof(cfi.FaceName), font);
  SetCurrentConsoleFontEx(CONSOLE_OUT, FALSE, &cfi);
  GetConsoleScreenBufferInfo(CONSOLE_OUT, &csbi);

  SetConsoleMode(CONSOLE_IN, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

  memset(SCREEN_ARR, 0, sizeof(CHAR_INFO) * SCREEN_WIDTH * SCREEN_HEIGHT);

  // Initialize Deltatime
  tp1 = clock();
  tp2 = clock();

  // Inputs

  memset(KEYS_NEW, 0, 256 * sizeof(short));
  memset(KEYS_OLD, 0, 256 * sizeof(short));
  memset(KEYS, 0, 256 * sizeof(short));
  MOUSEX = 0; MOUSEY = 0;
}


// **************** Drawing Routines
// Draw
void draw(int x, int y, short c, short col) {
  if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
  {
    SCREEN_ARR[y * SCREEN_WIDTH + x].Char.UnicodeChar = c;
    SCREEN_ARR[y * SCREEN_HEIGHT + x].Attributes = col;
  }
}

void clear() {memset(SCREEN_ARR, 0, sizeof(CHAR_INFO) * SCREEN_WIDTH * SCREEN_HEIGHT);}

int drawRawLine(int startx, int starty, int endx, int endy, short sym, short col) {
  int x1, y1, x2, y2, dx, dy, tx1, tx2, ty1, ty2, y, p;
  int is_steep, swapped, error, ystep, d, coord[2];
  x1 = startx; y1 = starty;
  x2 = endx; y2 = endy;
  dx = x2 - x1;
  dy = y2 - y1;
  d = sqrt(pow(dx, 2) + pow(dy, 2));

  is_steep = 0;
  if (abs(dy) > abs(dx)) is_steep = 1;
  if (is_steep == 1) {
    tx1 = x1; tx2 = x2; // ty1 = y1; ty2 = y2;
    x1 = y1; y1 = tx1;
    x2 = y2; y2 = tx2;
  }

  swapped = 0;
  if (x1 > x2) {
    tx1 = x1; ty1 = y1;
    x1 = x2; x2 = tx1;
    y1 = y2; y2 = ty1;
    swapped = 1;
  }

  dx = x2 - x1; dy = y2 - y1;

  error = dx / 2;
  if (y1 < y2) ystep = 1;
  else {ystep = -1;}

  y = y1;
  for (int x = x1; x < x2 + 1;x++) {
    if (is_steep == 1) {coord[0] = y; coord[1] = x;}
    else {coord[0] = x; coord[1] = y;}

    draw(coord[0], coord[1], sym, col);
    error -= abs(dy);
    if (error < 0) {
      y += ystep;
      error += dx;
    }
  }

  return 0;
}

int drawLine(int startx, int starty, int endx, int endy, int thickness, short sym, short col) {
  float slope;
  if (thickness == 1) {
    drawRawLine(startx, starty, endx, endy, sym, col);
    return 0;
  } else if (thickness == 0) {
    return 0;
  }

  if ((endy - starty) != 0 && (endx - startx) != 0) {
    slope = (float)(endy - starty) / (float)(endx - startx);
  } else {
    slope = 1;
  }

  if (slope >= 1 || slope <= -1) {
    for (int i = 0; i < thickness; i++)
      drawRawLine(startx+i, starty, endx+i, endy, sym, col);
  } else if (slope < 1 && slope > -1) {
    for (int i = 0; i < thickness; i++)
      drawRawLine(startx, starty+i, endx, endy+i, sym, col);
  }

  return 0;
}

int drawRect(int posx, int posy, int sizex, int sizey, int thickness, short sym, short fill, short scol, short fcol) {
  int b = 0;
  if (fill != ' ')
    for (int y = posy;y < posy + sizey; y++)
      drawLine(posx, y, posx + sizex + 1, y, 1, fill, fcol);

  if (thickness == 1) b = 1;

  drawLine(posx,         posy,             posx,             posy + sizey, thickness, sym, scol);
  drawLine(posx + sizex, posy,             posx,             posy,         thickness, sym, scol);

  drawLine(posx,         posy + sizey - thickness + 1,
          posx + sizex + b, posy + sizey - thickness + 1,
          thickness, sym, scol);
  drawLine(posx + sizex - thickness + 1, posy + sizey + b,
          posx + sizex - thickness + 1,     posy,
          thickness, sym, scol);

  return 0;
}

int fillBottomFlatTriangle(int v1x, int v1y, int v2x, int v2y, int v3x, int v3y, short fill, short fcol) {
  float invslope1 = (float)(v2x - v1x) / (float)(v2y - v1y);
  float invslope2 = (float)(v3x - v1x) / (float)(v3y - v1y);

  float curx1 = v1x; float curx2 = v1x;

  for (int y = v1y; y <= v2y; y++) {
    drawLine((int)curx1, y, (int)curx2, y, 1, fill, fcol);
    curx1 += invslope1; curx2 += invslope2;
  }

  return 0;
}

int fillTopFlatTriangle(int v1x, int v1y, int v2x, int v2y, int v3x, int v3y, short fill, short fcol) {
  float invslope1 = (float)(v3x - v1x) / (float)(v3y - v1y);
  float invslope2 = (float)(v3x - v2x) / (float)(v3y - v2y);

  float curx1 = v3x; float curx2 = v3x;

  for (int y = v3y; y > v1y; y--) {
    drawLine((int)curx1, y, (int)curx2, y, 1, fill, fcol);
    curx1 -= invslope1; curx2 -= invslope2;
  }

  return 0;
}

int drawTriangle(int pos1x, int pos1y, int pos2x, int pos2y, int pos3x, int pos3y, int thickness, short sym, short fill, short scol, short fcol) {
  int posys[3][2] = {{pos1x, pos1y}, {pos2x, pos2y}, {pos3x, pos3y}};
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (posys[j][1] > posys[i][1]) {
        int tmp[2] = {posys[i][0], posys[i][1]};
        posys[i][0] = posys[j][0];
        posys[i][1] = posys[j][1];
        posys[j][0] = tmp[0];
        posys[j][1] = tmp[1];
      }
    }
  }
  pos1x = posys[0][0]; pos1y = posys[0][1];
  pos2x = posys[1][0]; pos2y = posys[1][1];
  pos3x = posys[2][0]; pos3y = posys[2][1];

  if (fill != ' ') {
    if (pos2y == pos3y) {
      fillBottomFlatTriangle(pos1x, pos1y, pos2x, pos2y, pos3x, pos3y, fill, fcol);
    } else if (pos1y == pos2y) {
      fillTopFlatTriangle(pos1x, pos1y, pos2x, pos2y, pos3x, pos3y, fill, fcol);
    } else {
      int pos4x = (int)ceil(pos1x + ((float)(pos2y - pos1y) / (float)(pos3y - pos1y)) * (pos3x - pos1x));
      int pos4y = pos2y;

      fillBottomFlatTriangle(pos1x, pos1y, pos2x, pos2y, pos4x, pos4y, fill, fcol);
      fillTopFlatTriangle(pos2x, pos2y, pos4x, pos4y, pos3x, pos3y, fill, fcol);
    }
  }

  if (thickness == 0) {
    return 0;
  } else {
    drawLine(pos1x, pos1y, pos2x, pos2y, thickness, sym, scol);
    drawLine(pos2x, pos2y, pos3x, pos3y, thickness, sym, scol);
    drawLine(pos1x, pos1y, pos3x, pos3y, thickness, sym, scol);
  }

  return 0;
}

int drawCircle(int posx, int posy, int radius, short sym, short fill, short scol, short fcol) {
  if (radius < 1) return 0;
  int x = radius;
  int y = 0;
  int p = 1 - radius;

  if (fill != ' ') {
    drawLine(x + posx, y + posy, posx - x, y + posy, 1, fill, fcol);
    drawLine(y + posx, x + posy, y + posx, posy - x, 1, fill, fcol);
  }

  draw(x + posx, y + posy, sym, scol);
  if (radius > 0) {
    draw(posx - x, y + posy, sym, scol);
    draw(y + posx, x + posy, sym, scol);
    draw(y + posx, posy - x, sym, scol);
  }

  while (x > y) {
    y += 1;
    if (p <= 0) p = p + 2 * y + 1;
    else {
      x -= 1;
      p = p + 2 * y - 2 * x + 1;
    }

    if (x < y) break;

    if (fill != ' ') {
      drawLine(x + posx, y + posy, -x + posx, y + posy, 1, fill, fcol);
      drawLine(x + posx, -y + posy, -x + posx, -y + posy, 1, fill, fcol);
    }

    draw(x + posx, y + posy, sym, scol);
    draw(-x + posx, y + posy, sym, scol);
    draw(x + posx, -y + posy, sym, scol);
    draw(-x + posx, -y + posy, sym, scol);

    if (x != y) {
      if (fill != ' ') {
        drawLine(-y + posx, x + posy, -y + posx, -x + posy, 1, fill, fcol);
        drawLine(y + posx, x + posy, y + posx, -x + posy, 1, fill, fcol);
      }

      draw(y + posx, x + posy, sym, scol);
      draw(-y + posx, x + posy, sym, scol);
      draw(y + posx, -x + posy, sym, scol);
      draw(-y + posx, -x + posy, sym, scol);
    }
  }

  return 0;
}

// ******************** Customizable Functions ******************** //
// User function
int i = 0;
void user() {
  //clear();
  drawTriangle(MOUSEX, MOUSEY, 1, 1, 198, 198, 1, PIXEL_SOLID, L' ', FG_RED, FG_WHITE);
  drawTriangle(MOUSEX, MOUSEY, 1, 198, 198, 1, 1, PIXEL_SOLID, L' ', FG_BLUE, FG_WHITE);
  drawTriangle(MOUSEX, MOUSEY, 100, 198, 100, 1, 1, PIXEL_SOLID, L' ', FG_BLUE, FG_WHITE);
  drawTriangle(MOUSEX, MOUSEY, 198, 100, 1, 100, 1, PIXEL_SOLID, L' ', FG_BLUE, FG_WHITE);
}

// ******************* Mainloop ***************** //
int main() {
  construct();

  while (ACTIVE == 1) {
    while (ACTIVE == 1) {
      // Handle Deltatime
      tp2 = clock();
      elapsedTime = tp2 - tp1;
      tp1 = tp2;
      DELTATIME = elapsedTime / CLOCKS_PER_SEC;

      // Handle key inputs
      for (int i = 0; i < 256; i++) {
        KEYS_NEW[i] = GetAsyncKeyState(i);
        KEYS[i].pressed = 0;
        KEYS[i].released = 0;

        if (KEYS_NEW[i] != KEYS_OLD[i]) {
          if (KEYS_NEW[i] && 0x8000) {
            KEYS[i].pressed = !KEYS[i].held;
            KEYS[i].held = 1;
          }
          else {
            KEYS[i].released = 1;
            KEYS[i].held = 0;
          }
        }

        KEYS_OLD[i] = KEYS_NEW[i];
      }

      // Handle Mouse Inputs
      INPUT_RECORD inBuf[32];
      DWORD events = 0;
      GetNumberOfConsoleInputEvents(CONSOLE_IN, &events);
      if (events > 0)
        ReadConsoleInput(CONSOLE_IN, inBuf, events, &events);

      for (DWORD i = 0; i < events; i++) {
        switch (inBuf[i].EventType) {
          case FOCUS_EVENT:
            CONSOLE_FOCUS = inBuf[i].Event.FocusEvent.bSetFocus;
            break;
          case MOUSE_EVENT:
            switch (inBuf[i].Event.MouseEvent.dwEventFlags) {
              case MOUSE_MOVED:
                MOUSEX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
                MOUSEY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
                break;

              case 0:
                for (int m = 0; m < 5; m++)
                  MOUSE_NEW[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0 ? 1 : 0;
                break;

              default:
                break;
            }
          default:
            break;
        }
      }

      for (int m = 0; m < 5; m++) {
        MOUSE[m].pressed = 0;
        MOUSE[m].released = 0;

        if (MOUSE_NEW[m] != MOUSE_OLD[m]) {
          if (MOUSE_NEW[m]) {
            MOUSE[m].pressed = 1;
            MOUSE[m].held = 1;
          }
          else {
            MOUSE[m].released = 1;
            MOUSE[m].held = 0;
          }
        }

        MOUSE_OLD[m] = MOUSE_NEW[m];
      }


      // Run User Commands
      user();

      // Set Console Title
      if (DELTATIME != 0) {
        char s[256];
        sprintf_s(s, 256, "%s - FPS: %.0f", SCREEN_TITLE, 1.0 / DELTATIME);
        char * s2 = s;
        SetConsoleTitle(s2);
      }

      // Write to Console
      COORD ssize = { (short)SCREEN_WIDTH, (short)SCREEN_HEIGHT };
      COORD pos = { 0, 0 };
      WriteConsoleOutput(CONSOLE_OUT, SCREEN_ARR, ssize, pos, &WINDOW_RECT);
    }
  }
}
