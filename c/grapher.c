#define OLC_PGE_APPLICATION
#include "../olcPixelGameEngineC.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

typedef struct { int x; int y; } int2;
typedef struct { float x; float y; float z; } float3;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_SCALE 4
#define F(x, a, h, k) (a) * powf((x) - (h), 2) + (k) // [ a * ( x - h ) ^ 2 + k ]  ->  [ -a * ( x - h ) ^ 2 + k ]

int2 half_screen;
float3 values;
bool noline = true;
float res = 0.1;

bool OnUserCreate() { 
    half_screen.x = (int)(SCREEN_WIDTH / 2);
    half_screen.y = (int)(SCREEN_HEIGHT / 2);

    values.x = 1;
    values.y = 0;
    values.z = 0;

    return true; 
}

bool OnUserUpdate(float fElapsedTime)
{
    PGE_Clear(olc_BLACK);

    int2 last_point = { -half_screen.x, F(half_screen.x, values.x, values.y, values.z) };
    bool last_point_drawn = false;
    for (float x = -half_screen.x; x < half_screen.x; x += res) {
        float y = F(x, values.x, values.y, values.z);
        int ay = y + half_screen.y;

        if (ay > 0 && ay < SCREEN_HEIGHT) { 
            int ax = x + half_screen.x;
            PGE_Draw(ax, ay, olc_RED); 
            if (!noline) PGE_DrawLine(ax, ay, last_point.x + half_screen.x, last_point.y + half_screen.y, olc_RED, -1);
            last_point_drawn = true;
        }
        else if (last_point_drawn) {
            int ax = x + half_screen.x;
            if (!noline) PGE_DrawLine(ax, ay, last_point.x + half_screen.x, last_point.y + half_screen.y, olc_RED, -1);
            last_point_drawn = false;
        }

        last_point.x = x; last_point.y = y;
    }

    PGE_FillCircle(half_screen.x, half_screen.y, 1, olc_WHITE);
    PGE_FillCircle(values.y + half_screen.x, values.z + half_screen.y, 1, olc_WHITE);

    bool up = PGE_GetKey(olc_UP).bHeld;
    bool down = PGE_GetKey(olc_DOWN).bHeld;
    float change = 5 * fElapsedTime;

    if (PGE_GetKey(olc_X).bHeld) {
        if (up) values.x += change;
        if (down) values.x -= change;
    }
    if (PGE_GetKey(olc_Y).bHeld) {
        if (up) values.y += change;
        if (down) values.y -= change;
    }
    if (PGE_GetKey(olc_Z).bHeld) {
        if (up) values.z += change;
        if (down) values.z -= change;
    }

    /*PGE_DrawString(16, 16, "Current Values:", olc_YELLOW, 1);

    char t[48]; snprintf(t, sizeof(t), "x = %f", values.x);
    PGE_DrawString(16, 16 + 12 * 1, t, olc_WHITE, 1);

    snprintf(t, sizeof(t), "y = %f", values.y);
    PGE_DrawString(16, 16 + 12 * 2, t, olc_WHITE, 1);

    snprintf(t, sizeof(t), "z = %f", values.z);
    PGE_DrawString(16, 16 + 12 * 3, t, olc_WHITE, 1);

    PGE_DrawString(16, 16 + 12 * 5, "hold [X/Y/Z] keys and use arrow keys", olc_WHITE, 1);
    PGE_DrawString(16, 16 + 12 * 6, "to increase or decrease value.", olc_WHITE, 1);

    PGE_DrawString(16, 16 + 12 * 8, "*top-right is -x, -y quadrant", olc_WHITE, 1);*/

    return !PGE_GetKey(olc_ESCAPE).bPressed;
}

bool OnUserDestroy() { return true; }

int main(int argc, char* argv[])
{
    PGE_SetAppName("Grapher");
    if (PGE_Construct(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_SCALE, SCREEN_SCALE, false, false))
        PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);

    return 0;
}
