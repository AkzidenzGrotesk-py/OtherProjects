#define OLC_PGE_APPLICATION
#include "C:\Users\Kazuto (^^)\Desktop\3d\olcPixelGameEngineC.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define WWIDTH 1280
#define WHEIGHT 720
#define PIXEL_SCALE 1

float dA = 1.0f;     // diffusion A
float dB = 0.5f;     // diffusion B
float feed = 0.055f; //0.055f; // feed rate
float k = 0.062f; //0.062f;    // kill rate
int brushSize = 32;  // brush size
int wscale = 1;
int hscale = 1;

bool paused = false; 
bool presetMenu = false;
int selection = 0;
bool nopausemenu = false;

// -- STRUCTS
typedef struct { float a; float b; float da; float db; } cell;
typedef struct { cell c[WHEIGHT][WWIDTH]; } dish;
dish grid, next;

// -- SWAPPING
void swapDishes(dish** a, dish** b) { dish* tmp = *a; *a = *b; *b = tmp; }
void swap() { dish* pgrid = &grid; dish* pnext = &next; swapDishes(&pgrid, &pnext); grid = *pgrid; next = *pnext; }
float constrain(float n, float low, float high) {
    if (n > high) return high;
    else if (n < low) return low;
    return n;
}

// -- HOVER
bool mouseInSquare(int ax, int ay, int bx, int by) {
    if (PGE_GetMouseX() > ax && PGE_GetMouseX() < bx)
        if (PGE_GetMouseY() > ay && PGE_GetMouseY() < by)
            return true;
    return false;
}


// -- LAPLACE
float laplaceA(int x, int y) { return grid.c[y][x].a * -1.0f + grid.c[y][x - 1].a * 0.2f + grid.c[y][x + 1].a * 0.2f + grid.c[y + 1][x].a * 0.2f + grid.c[y - 1][x].a * 0.2f + grid.c[y - 1][x - 1].a * 0.05f + grid.c[y + 1][x + 1].a * 0.05f + grid.c[y - 1][x + 1].a * 0.05f + grid.c[y + 1][x - 1].a * 0.05f; }
float laplaceB(int x, int y) { return grid.c[y][x].b * -1.0f + grid.c[y][x - 1].b * 0.2f + grid.c[y][x + 1].b * 0.2f + grid.c[y + 1][x].b * 0.2f + grid.c[y - 1][x].b * 0.2f + grid.c[y - 1][x - 1].b * 0.05f + grid.c[y + 1][x + 1].b * 0.05f + grid.c[y - 1][x + 1].b * 0.05f + grid.c[y + 1][x - 1].b * 0.05f; }

// -- FRAME
bool OnUserUpdate(float fElapsedTime)
{
    PGE_Clear(olc_BLACK);
    
    if (PGE_GetKey(olc_M).bPressed) nopausemenu = !nopausemenu;
    if (PGE_GetKey(olc_P).bPressed) paused = !paused;
    if (PGE_GetKey(olc_S).bPressed) presetMenu = !presetMenu;
    if (PGE_GetKey(olc_R).bPressed) {
        for (int x = 0; x < WWIDTH; x++) {
            for (int y = 0; y < WHEIGHT; y++) {
                grid.c[y][x].a = 1.0f; grid.c[y][x].b = 0.0f;
                next.c[y][x].a = 1.0f; next.c[y][x].b = 0.0f;
            }
        }
    }

    if (wscale > 1 && hscale > 1) PGE_DrawRect(wscale - 1, hscale - 1, WWIDTH - wscale * 2 + 1, WHEIGHT - hscale * 2 + 1, olc_MAGENTA);

    if (!paused || presetMenu) {
        int mX = PGE_GetMouseX(); int mY = PGE_GetMouseY();
        if (PGE_GetMouse(0).bHeld) {
            for (int i = mY - brushSize; i < mY + brushSize; i++) {
                for (int j = mX - brushSize; j < mX + brushSize; j++) {
                    if (i < 0 + hscale || i >= WHEIGHT - hscale || j < 0 + wscale || j >= WWIDTH - wscale) continue;
                    int distance = sqrt(pow(abs(i - mY), 2) + pow(abs(j - mX), 2));
                    if (distance > brushSize) continue;
                    grid.c[i][j].a = 0.0f; next.c[i][j].a = 0.0f;
                    grid.c[i][j].b = 1.0f; next.c[i][j].b = 1.0f;
                }
            }
        }
        if (PGE_GetMouse(1).bHeld) {
            for (int i = mY - brushSize; i < mY + brushSize; i++) {
                for (int j = mX - brushSize; j < mX + brushSize; j++) {
                    if (i < 0 + hscale || i >= WHEIGHT - hscale || j < 0 + wscale || j >= WWIDTH - wscale) continue;
                    int distance = sqrt(pow(abs(i - mY), 2) + pow(abs(j - mX), 2));
                    if (distance > brushSize) continue;
                    grid.c[i][j].b = 0.0f; next.c[i][j].b = 0.0f;
                    grid.c[i][j].a = 1.0f; next.c[i][j].a = 1.0f;
                }
            }
        }

        // reaction
        float a, b;
        for (int x = 1 + wscale; x < WWIDTH - 1 - wscale; x++) {
            for (int y = 1 + hscale; y < WHEIGHT - 1 - hscale; y++) {
                float la = laplaceA(x, y); float lb = laplaceB(x, y);
                a = grid.c[y][x].a;
                b = grid.c[y][x].b;
                if (la + lb == 0) {
                    continue;
                }

                float p = a * b * b;
                float newA = ((dA * la) - p + (feed * (1 - a)));
                float newB = ((dB * lb) + p - ((k + feed) * b));
                next.c[y][x].a = constrain(a + newA, 0, 1);
                next.c[y][x].b = constrain(b + newB, 0, 1);
                next.c[y][x].da = newA;
                next.c[y][x].db = newB;

                /*float c = floorf((next.c[y][x].a - next.c[y][x].b) * 255);
                if (c > 255) c = 255; if (c < 0) c = 0;
                PGE_Draw(x, y,olc_PixelRGBA((int)(c),(int)(c),(int)(c),(int)c));*/
                PGE_Draw(x, y, olc_PixelRGBA((next.c[y][x].da + next.c[y][x].db) * 127, next.c[y][x].a * 255, next.c[y][x].a * 255, next.c[y][x].b * 255));
            }
        }

        // swap
        swap();
    }

    if (presetMenu) {
        if (PGE_GetKey(olc_UP).bPressed) selection -= selection > 0 ? 1 : 0;
        if (PGE_GetKey(olc_DOWN).bPressed) selection += selection < 3 ? 1 : 0;
        if (PGE_GetKey(olc_ENTER).bPressed) {
            if (selection == 0) {
                dA = 1.0f; dB = 0.5f; feed = 0.055f; k = 0.062f;
            } else if (selection == 1) {
                dA = 1.0f; dB = 0.5f; feed = 0.0367f; k = 0.0649f; brushSize = 3;
            } else if (selection == 2) {
                dA = 1.0f; dB = 0.5f; feed = 0.0545f; k = 0.062f;
            } else if (selection == 3) {

            }
        }

        PGE_DrawString(16, WHEIGHT - 12 * 5, "Default", selection == 0 ? olc_YELLOW : olc_WHITE, 1);
        PGE_DrawString(16, WHEIGHT - 12 * 4, "Mitosis", selection == 1 ? olc_YELLOW : olc_WHITE, 1);
        PGE_DrawString(16, WHEIGHT - 12 * 3, "Coral Growth", selection == 2 ? olc_YELLOW : olc_WHITE, 1);
        PGE_DrawString(16, WHEIGHT - 12 * 2, "Custom", selection == 3 ? olc_YELLOW : olc_WHITE, 1);
    }

    if (paused || nopausemenu) {
        for (int x = 1 + wscale; x < WWIDTH - 1 - wscale; x++) {
            for (int y = 1 + hscale; y < WHEIGHT - 1 - hscale; y++) {
                /*float c = floorf((next.c[y][x].a - next.c[y][x].b) * 255);
                if (c > 255) c = 255; if (c < 0) c = 0;
                PGE_Draw(x, y, olc_PixelRGBA((int)(c), (int)(c), (int)(c), (int)c));*/
                PGE_Draw(x, y, olc_PixelRGBA((next.c[y][x].da + next.c[y][x].db) * 127, next.c[y][x].a * 255, next.c[y][x].a * 255, next.c[y][x].b * 255));
            }
        }
                

        // detection
        int mwDelta = PGE_GetMouseWheel();
        bool qKey = PGE_GetKey(olc_Q).bHeld;
        bool wKey = PGE_GetKey(olc_W).bHeld; bool sKey = PGE_GetKey(olc_S).bHeld;
        if (mwDelta > 0 && mouseInSquare(232, 50, 296, 58)) dA += qKey ? 0.01f : 0.0001f;
        if (mwDelta < 0 && mouseInSquare(232, 50, 296, 58)) { dA -= qKey ? 0.01f : 0.0001f; dA = fmax(0, dA);}
        if (wKey && mouseInSquare(232, 50, 296, 58)) dA += qKey ? 1.0f : 0.1f;
        if (sKey && mouseInSquare(232, 50, 296, 58)) { dA -= qKey ? 1.0f : 0.1f; dA = fmax(0, dA); }

        if (mwDelta > 0 && mouseInSquare(232, 63, 296, 71)) dB += qKey ? 0.01f : 0.0001f;
        if (mwDelta < 0 && mouseInSquare(232, 63, 296, 71)) { dB -= qKey ? 0.01f : 0.0001f; dB = fmax(0, dB);}
        if (wKey && mouseInSquare(232, 63, 296, 71)) dB += qKey ? 1.0f : 0.1f;
        if (sKey && mouseInSquare(232, 63, 296, 71)) { dB -= qKey ? 1.0f : 0.1f; dB = fmax(0, dB); }

        if (mwDelta > 0 && mouseInSquare(232, 76, 296, 84)) feed += qKey ? 0.01f : 0.0001f;
        if (mwDelta < 0 && mouseInSquare(232, 76, 296, 84)) { feed -= qKey ? 0.01f : 0.0001f; feed = fmax(0, feed);}
        if (wKey && mouseInSquare(232, 76, 296, 84)) feed += qKey ? 1.0f : 0.1f;
        if (sKey && mouseInSquare(232, 76, 296, 84)) { feed -= qKey ? 1.0f : 0.1f; feed = fmax(0, feed); }

        if (mwDelta > 0 && mouseInSquare(232, 89, 296, 97)) k += qKey ? 0.01f : 0.0001f;
        if (mwDelta < 0 && mouseInSquare(232, 89, 296, 97)) { k -= qKey ? 0.01f : 0.0001f; k = fmax(0, k);}
        if (wKey && mouseInSquare(232, 89, 296, 97)) k += qKey ? 1.0f : 0.1f;
        if (sKey && mouseInSquare(232, 89, 296, 97)) { k -= qKey ? 1.0f : 0.1f; k = fmax(0, k); }

        if (mwDelta > 0 && mouseInSquare(232, 102, 296, 110)) brushSize += qKey ? 4 : 1;
        if (mwDelta < 0 && mouseInSquare(232, 102, 296, 110)) { brushSize -= qKey ? 4 : 1; brushSize = fmax(1, brushSize); }

        if (mwDelta > 0 && mouseInSquare(232, 193, 296, 201)) {
            wscale += qKey ? 5 : 1; wscale = fmin(wscale, WWIDTH / 2 - 50);
        }
        if (mwDelta < 0 && mouseInSquare(232, 193, 296, 201)) {
            wscale -= qKey ? 5 : 1; wscale = fmax(0, wscale);
        }
        if (mwDelta > 0 && mouseInSquare(232, 206, 296, 214)) {
            hscale += qKey ? 5 : 1; hscale = fmin(hscale, WHEIGHT / 2 - 50);
        }
        if (mwDelta < 0 && mouseInSquare(232, 206, 296, 214)) {
            hscale -= qKey ? 5 : 1; hscale = fmax(0, hscale);
        }

        // colours
        olc_Pixel light_blue = olc_PixelRGB(150, 175, 255);
        olc_Pixel light_green = olc_PixelRGB(150, 255, 175);
        olc_Pixel light_red = olc_PixelRGB(255, 150, 175);
        olc_Pixel slime_green = olc_PixelRGB(15, 210, 60);

        // heading
        PGE_DrawString(16, 16, "Reaction-Diffusion using Gray-Scott Model", slime_green, 1);
        PGE_DrawString(16, 28, "Simulation Settings", olc_WHITE, 2);

        PGE_DrawString(16, 50, "Diffuse Speed A -", light_green, 1);
        char ta[8]; snprintf(ta, sizeof(ta), "%.5f", dA);
        PGE_DrawString(232, 50, ta, mouseInSquare(232, 50, 296, 58) ? olc_YELLOW : olc_WHITE, 1);

        PGE_DrawString(16, 63, "Diffuse Speed B -", light_green, 1);
        char tb[8]; snprintf(tb, sizeof(tb), "%.5f", dB);
        PGE_DrawString(232, 63, tb, mouseInSquare(232, 63, 296, 71) ? olc_YELLOW : olc_WHITE, 1);

        PGE_DrawString(16, 76, "Feed Rate -", light_red, 1);
        char tc[8]; snprintf(tc, sizeof(tc), "%.5f", feed);
        PGE_DrawString(232, 76, tc, mouseInSquare(232, 76, 296, 84) ? olc_YELLOW : olc_WHITE, 1);

        PGE_DrawString(16, 89, "Kill Rate -", light_blue, 1);
        char td[8]; snprintf(td, sizeof(td), "%.5f", k);
        PGE_DrawString(232, 89, td, mouseInSquare(232, 89, 296, 97) ? olc_YELLOW : olc_WHITE, 1);

        PGE_DrawString(16, 102, "Brush Size -", olc_WHITE, 1);
        char te[8]; snprintf(te, sizeof(te), "%i", brushSize);
        PGE_DrawString(232, 102, te, mouseInSquare(232, 102, 296, 110) ? olc_YELLOW : olc_WHITE, 1);

        PGE_DrawString(16, 193, "Width Scaling -", olc_WHITE, 1);
        char tj[8]; snprintf(tj, sizeof(tj), "%i", wscale);
        PGE_DrawString(232, 193, tj, mouseInSquare(232, 193, 296, 201) ? olc_YELLOW : olc_WHITE, 1);

        PGE_DrawString(16, 206, "Height Scaling -", olc_WHITE, 1);
        char tk[8]; snprintf(tk, sizeof(tk), "%i", hscale);
        PGE_DrawString(232, 206, tk, mouseInSquare(232, 206, 296, 214) ? olc_YELLOW : olc_WHITE, 1);
    }

    PGE_DrawString(WWIDTH - 8 * 21, WHEIGHT - 16, "[P] - Pause", paused ? olc_YELLOW : olc_WHITE, 1);
    PGE_DrawString(WWIDTH - 8 * 21, WHEIGHT - 28, "[R] - Clear", olc_WHITE, 1);
    PGE_DrawString(WWIDTH - 8 * 21, WHEIGHT - 40, "[S] - Select Presets", presetMenu ? olc_YELLOW : olc_WHITE, 1);
    PGE_DrawString(WWIDTH - 8 * 21, WHEIGHT - 52, "[M] - Menu", nopausemenu ? olc_YELLOW : olc_WHITE, 1);

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
    if (PGE_Construct(WWIDTH, WHEIGHT, PIXEL_SCALE, PIXEL_SCALE, false, false))
        PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);

    return 0;
}
