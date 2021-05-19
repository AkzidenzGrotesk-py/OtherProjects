#define OLC_PGE_APPLICATION
#include "olcPixelGameEngineC.h"
#include <stdlib.h>
#include <math.h>

float camera_pos[2] = { 0, 0 };
float player_pos[2] = { 0, 0 };
float init_mouse[2] = { 0, 0 };
int shape[][2] = {
  {   0,     -30   },
  {   30,    -30   },
  {   30,    -75   },
  {   200,   -75   },
  {   200,   75    },
  {   30,    75    },
  {   30,    30    },
  {   0,     30    }
};

void controls(float deltatime) {
  int speed = 400;
  if (PGE_GetKey(olc_CTRL).bHeld) speed *= 2;
  if (PGE_GetKey(olc_S).bHeld) player_pos[1] += speed * deltatime;
  if (PGE_GetKey(olc_W).bHeld) player_pos[1] -= speed * deltatime;
  if (PGE_GetKey(olc_D).bHeld) player_pos[0] += speed * deltatime;
  if (PGE_GetKey(olc_A).bHeld) player_pos[0] -= speed * deltatime;

  if (PGE_GetMouse(0).bPressed) {
    init_mouse[0] = camera_pos[0] - (float)PGE_GetMouseX();
    init_mouse[1] = camera_pos[1] - (float)PGE_GetMouseY();
  }
  if (PGE_GetMouse(0).bHeld) {
    camera_pos[0] = init_mouse[0] + (float)PGE_GetMouseX();
    camera_pos[1] = init_mouse[1] + (float)PGE_GetMouseY();
  }
}

void render(float deltatime) {
  olc_Pixel dark_gray = olc_PixelRGB(50, 50, 50);
  PGE_DrawLine(0, camera_pos[1], PGE_ScreenWidth(), camera_pos[1], dark_gray, olc_SOLID);
  PGE_DrawLine(camera_pos[0], 0, camera_pos[0], PGE_ScreenHeight(), dark_gray, olc_SOLID);
  PGE_DrawCircle(camera_pos[0], camera_pos[1], 600, dark_gray, (uint8_t)olc_SOLID);
  PGE_DrawCircle(camera_pos[0], camera_pos[1], 1200, dark_gray, (uint8_t)olc_SOLID);
  PGE_DrawCircle(camera_pos[0], camera_pos[1], 2400, dark_gray, (uint8_t)olc_SOLID);
  PGE_DrawCircle(camera_pos[0], camera_pos[1], 4800, dark_gray, (uint8_t)olc_SOLID);
  PGE_DrawCircle(camera_pos[0], camera_pos[1], 9600, dark_gray, (uint8_t)olc_SOLID);
  PGE_FillCircle(camera_pos[0], camera_pos[1], 2, olc_WHITE);

  int lastpoint[2];
  lastpoint[0] = shape[7][0];
  lastpoint[1] = shape[7][1];
  for (int k = 0; k < 8; k++) {
    PGE_DrawLine(lastpoint[0] + camera_pos[0] + 320, lastpoint[1]  + camera_pos[1], shape[k][0] + camera_pos[0] + 320, shape[k][1] + camera_pos[1], olc_PixelRGB(200, 25, 25), olc_SOLID);

    lastpoint[0] = shape[k][0];
    lastpoint[1] = shape[k][1];
  }

  PGE_DrawTriangle(player_pos[0] + camera_pos[0], player_pos[1] - 12 + camera_pos[1], player_pos[0] + 6 + camera_pos[0], player_pos[1] + 6 + camera_pos[1], player_pos[0] - 6 + camera_pos[0], player_pos[1] + 6 + camera_pos[1], olc_WHITE);
}

bool OnUserUpdate(float deltatime)
{
  PGE_Clear(olc_PixelRGB(0, 0, 0));
  controls(deltatime);
  render(deltatime);
  return true;
}

bool OnUserCreate()
{
  camera_pos[0] = PGE_ScreenWidth() / 2;
  camera_pos[1] = PGE_ScreenHeight() / 2;
  return true;
}

bool OnUserDestroy() {return true;}

int main(int argc, char* argv[])
{
  PGE_SetAppName("Lazer RPG");
  if (PGE_Construct(1280, 720, 1, 1, false, false))
    PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);

  return 0;
}
