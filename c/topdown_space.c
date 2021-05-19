#define OLC_PGE_APPLICATION
#include "olcPixelGameEngineC.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

bool dark_mode = true;
float camera_pos[2];
float init_mouse[2];
int cp_loc = 0;
int g_cp_loc = -1;
int point_loc[256][2];
int speed, lock_shiftX, lock_shiftY;
olc_Pixel background, unit_lines, tens_lines, center_lines;

void camera(float deltatime) {
  // Camera Controls
  speed = 400;
  if (PGE_GetKey(olc_CTRL).bHeld) speed *= 2;
  if (PGE_GetKey(olc_UP).bHeld) camera_pos[1] += speed * deltatime;
  if (PGE_GetKey(olc_DOWN).bHeld) camera_pos[1] -= speed * deltatime;
  if (PGE_GetKey(olc_LEFT).bHeld) camera_pos[0] += speed * deltatime;
  if (PGE_GetKey(olc_RIGHT).bHeld) camera_pos[0] -= speed * deltatime;

  if (PGE_GetKey(olc_SHIFT).bHeld) {
    if (PGE_GetMouseWheel() > 0) camera_pos[0] += speed * 25 * deltatime;
  	if (PGE_GetMouseWheel() < 0) camera_pos[0] -= speed * 25 * deltatime;

    if (PGE_GetMouse(0).bPressed) {
      init_mouse[0] = camera_pos[0] - (float)PGE_GetMouseX();
      init_mouse[1] = camera_pos[1] - (float)PGE_GetMouseY();
    }
    if (PGE_GetMouse(0).bHeld) {
      camera_pos[0] = init_mouse[0] + (float)PGE_GetMouseX();
      camera_pos[1] = init_mouse[1] + (float)PGE_GetMouseY();
    }
  } else {
    if (PGE_GetMouse(0).bHeld) {
      bool noplace = false;
      int plx = (round(-(camera_pos[0] - PGE_GetMouseX()) / 10) * 10);
      int ply = (round(-(camera_pos[1] - PGE_GetMouseY()) / 10) * 10);
      if (PGE_GetMouse(0).bPressed) {
        for (int i = 0; i < cp_loc; i++) {
          if (point_loc[i][0] == plx && point_loc[i][1] == ply) {
            noplace = true;
            g_cp_loc = i;
          }
        }
        if (!noplace) {
          point_loc[cp_loc][0] = plx;
          point_loc[cp_loc][1] = ply;
          if (cp_loc < 255) cp_loc++;
        }
      }
      else {
        if (g_cp_loc >= 0) {
          point_loc[g_cp_loc][0] = plx;
          point_loc[g_cp_loc][1] = ply;
        }
      }
    } else {g_cp_loc = -1;}
    if (PGE_GetMouse(1).bPressed) {
      if (cp_loc > 0) cp_loc --;
    }
    if (PGE_GetMouseWheel() > 0) camera_pos[1] += speed * 25 * deltatime;
  	if (PGE_GetMouseWheel() < 0) camera_pos[1] -= speed * 25 * deltatime;
  }
}

void gridlines(float deltatime) {
  // Handle Unit Lines
  lock_shiftX = (int)ceil(camera_pos[0]) % 10; lock_shiftY = (int)ceil(camera_pos[1]) % 10;
  for (int x = lock_shiftX; x < PGE_ScreenWidth() + lock_shiftX + 10; x += 10) // Vertical
    PGE_DrawLine(x, 0, x, PGE_ScreenHeight(), unit_lines, olc_SOLID);
  for (int y = lock_shiftY; y < PGE_ScreenHeight() + lock_shiftY + 10; y += 10) // Horizontal
    PGE_DrawLine(0, y, PGE_ScreenWidth(), y, unit_lines, olc_SOLID);

  // Handle Tens Lines
  lock_shiftX = (int)ceil(camera_pos[0]) % 100; lock_shiftY = (int)ceil(camera_pos[1]) % 100;
  for (int x = lock_shiftX; x < PGE_ScreenWidth() + lock_shiftX + 100; x += 100) // Vertical
    PGE_DrawLine(x, 0, x, PGE_ScreenHeight(), tens_lines, olc_SOLID);
  for (int y = lock_shiftY; y < PGE_ScreenHeight() + lock_shiftY + 100; y += 100) // Horizontal
    PGE_DrawLine(0, y, PGE_ScreenWidth(), y, tens_lines, olc_SOLID);

  // Handle Center Lines
  PGE_DrawLine(0, camera_pos[1], PGE_ScreenWidth(), camera_pos[1], center_lines, olc_SOLID);
  PGE_DrawLine(camera_pos[0], 0, camera_pos[0], PGE_ScreenHeight(), center_lines, olc_SOLID);
}

bool OnUserCreate() {
  // Camera Position Initiation
  camera_pos[0] = PGE_ScreenWidth() / 2;
  camera_pos[1] = PGE_ScreenHeight() / 2;

  // Theme Initiation
  background = !dark_mode ? olc_PixelRGB(220, 220, 220) : olc_PixelRGB(0, 0, 0);
  unit_lines = !dark_mode ? olc_PixelRGB(150, 150, 150) : olc_PixelRGB(30, 30, 30);
  tens_lines = !dark_mode ? olc_PixelRGB(200, 100, 100) : olc_PixelRGB(75, 75, 75);
  center_lines = !dark_mode ? olc_PixelRGB(0, 0, 0) : olc_PixelRGB(200, 200, 200);

  return true;
}

bool OnUserUpdate(float deltatime) {
  PGE_Clear(background);

  camera(deltatime);
  gridlines(deltatime);

  int lastpoint[2];
  lastpoint[0] = point_loc[cp_loc - 1][0];
  lastpoint[1] = point_loc[cp_loc - 1][1];
  for (int i = 0; i < cp_loc; i++) {
    if (i == 0) {PGE_FillCircle(point_loc[i][0] + camera_pos[0], point_loc[i][1] + camera_pos[1], 2, olc_PixelRGB(25, 25, 255));}
    else if (i == cp_loc - 1) {PGE_FillCircle(point_loc[i][0] + camera_pos[0], point_loc[i][1] + camera_pos[1], 2, olc_PixelRGB(25, 255, 25));}
    else {PGE_FillCircle(point_loc[i][0] + camera_pos[0], point_loc[i][1] + camera_pos[1], 2, olc_PixelRGB(255, 25, 25));}

    if (cp_loc > 1) {
      PGE_DrawLine(point_loc[i][0] + camera_pos[0], point_loc[i][1] + camera_pos[1], lastpoint[0] + camera_pos[0], lastpoint[1] + camera_pos[1], olc_PixelRGB(255, 0, 0), olc_SOLID);

      lastpoint[0] = point_loc[i][0];
      lastpoint[1] = point_loc[i][1];
    }
  }

  return !PGE_GetKey(olc_ESCAPE).bPressed;
}

bool OnUserDestroy()
{ return true; }

int main(int argc, char* argv[])
{
  PGE_SetAppName("Topdown Space");
  if(PGE_Construct(1280, 720, 1, 1, false, false))
    PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);

  return 0;
}
