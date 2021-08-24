#define OLC_PGE_APPLICATION
#include "olcPixelGameEngineC.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define WHEIGHT 1080 / 3
#define WWIDTH 1920 / 3
float dA = 1.0f;     // diffusion A
float dB = 0.5f;     // diffusion B
float feed = 0.055f; // feed rate
float k = 0.062f;    // kill rate

// -- STRUCTS
typedef struct {float a; float b; float da; float db;} cell;
typedef struct {cell c[WHEIGHT][WWIDTH];} dish;
dish grid, next;

// -- SWAPPING
void swapDishes(dish **a, dish **b){dish *tmp = *a;*a = *b;*b = tmp;}
void swap() {dish* pgrid = &grid;dish* pnext = &next;swapDishes(&pgrid, &pnext);grid = *pgrid;next = *pnext;}
float constrain(float n, float low, float high) {
  if (n > high) return high;
  else if (n < low) return low;
  return n;
}


// -- LAPLACE
float laplaceA(int x, int y) {return grid.c[y][x].a * -1.0f + grid.c[y][x-1].a * 0.2f + grid.c[y][x+1].a * 0.2f + grid.c[y+1][x].a * 0.2f + grid.c[y-1][x].a * 0.2f + grid.c[y-1][x-1].a * 0.05f + grid.c[y+1][x+1].a * 0.05f + grid.c[y-1][x+1].a * 0.05f + grid.c[y+1][x-1].a * 0.05f;}
float laplaceB(int x, int y) {return grid.c[y][x].b * -1.0f + grid.c[y][x-1].b * 0.2f + grid.c[y][x+1].b * 0.2f + grid.c[y+1][x].b * 0.2f + grid.c[y-1][x].b * 0.2f + grid.c[y-1][x-1].b * 0.05f + grid.c[y+1][x+1].b * 0.05f + grid.c[y-1][x+1].b * 0.05f + grid.c[y+1][x-1].b * 0.05f;}

// -- FRAME
bool OnUserUpdate(float fElapsedTime)
{

  if (PGE_GetMouse(0).bHeld) {
    for (int i = PGE_GetMouseY()-16; i < PGE_GetMouseY()+16; i ++) {
      for (int j = PGE_GetMouseX()-16; j < PGE_GetMouseX()+16; j ++) {
        if (i < 0 || i > WHEIGHT || j < 0 || j > WWIDTH) continue;
        grid.c[i][j].a = 0.0f; next.c[i][j].a = 0.0f;
        grid.c[i][j].b = 1.0f; next.c[i][j].b = 1.0f;
      }
    }
  }
  if (PGE_GetMouse(1).bHeld) {
    for (int i = PGE_GetMouseY()-16; i < PGE_GetMouseY()+16; i ++) {
      for (int j = PGE_GetMouseX()-16; j < PGE_GetMouseX()+16; j ++) {
        if (i < 0 || i > WHEIGHT || j < 0 || j > WWIDTH) continue;
        grid.c[i][j].b = 0.0f; next.c[i][j].b = 0.0f;
        grid.c[i][j].a = 1.0f; next.c[i][j].a = 1.0f;
      }
    }
  }

  // reaction
  float a, b;
  for (int x = 1; x < WWIDTH-1; x++) {
    for (int y = 1; y < WHEIGHT-1; y++) {
      a = grid.c[y][x].a;
      b = grid.c[y][x].b;
      float p = a * b * b;
      float newA = ((dA * laplaceA(x, y)) - p + (feed * (1 - a)));
      float newB = ((dB * laplaceB(x, y)) + p - ((k + feed) * b));
      next.c[y][x].a = constrain(a + newA, 0, 1);
      next.c[y][x].b = constrain(b + newB, 0, 1);
      next.c[y][x].da = newA;
      next.c[y][x].db = newB;

      float c = floorf((next.c[y][x].a - next.c[y][x].b) * 255);
      if (c > 255) c = 255; if (c < 0) c = 0;
      PGE_Draw(x, y,olc_PixelRGBA((int)(c),(int)(c),(int)(c),(int)c));
      //PGE_Draw(x, y,olc_PixelRGBA((next.c[y][x].da+next.c[y][x].db)*127,next.c[y][x].a*255,next.c[y][x].a*255,next.c[y][x].b*255));
    }
  }

  // swap
  swap();

  return true;
}

// -- DESTROY
bool OnUserDestroy() { return true; }

// -- SETUP
bool OnUserCreate() {
  for (int x = 0; x < WWIDTH; x++) {
    for (int y = 0; y < WHEIGHT; y++) {
      grid.c[y][x].a = 1.0f; grid.c[y][x].b = 0.0f;
      next.c[y][x].a = 1.0f; next.c[y][x].b = 0.0f;
    }
  }

  return true;
}

int main(int argc, char* argv[])
{
  PGE_SetAppName("Reaction/Diffusion Reaction");
  if(PGE_Construct(WWIDTH, WHEIGHT, 2, 2, false, false))
    PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);

  return 0;
}
