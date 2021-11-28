#define OLC_PGE_APPLICATION
#include "..\clib\olcPixelGameEngineC.h"
#include "..\clib\uvtypes.h"
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 500
#define HEIGHT 500

typedef struct {
    bool t[WIDTH][HEIGHT];
} table;

table front, back;
int lmx, lmy;

void swap(int* xp, int* yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void drawBresenham(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    bool is_steep = fabs(dy) > fabs(dx);
    if (is_steep) {
        swap(&x1, &y1); swap(&x2, &y2);
    }


    bool swapped = false;
    if (x1 > x2) {
        swap(&x1, &x2); swap(&y1, &y2);
        swapped = true;
    }

    dx = x2 - x1;
    dy = y2 - y1;

    int error = dx / 2.0f;
    int ystep = y1 < y2 ? 1 : -1;

    int y = y1;
    for (int x = x1; x < x2 + 1; x++) {
        if (is_steep)
            back.t[x][y] = true;
        else
            back.t[y][x] = true;

        error -= fabs(dy);
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
    
    return;
}

bool perPixel(bool currentState, int surroundedBy) {
    if (currentState) {
        if (surroundedBy == 2 || surroundedBy == 3) return true;
        else return false;
    } else {
        if (surroundedBy == 3) return true;
    }
}

int countSurrounded(int x, int y) {
    return back.t[y-1][x-1] + back.t[y-1][x] + back.t[y-1][x+1] +
           back.t[y][x-1]   + back.t[y][x+1] +
           back.t[y+1][x-1] + back.t[y+1][x] + back.t[y+1][x+1];
}

bool OnUserUpdate(float fElapsedTime) {
    if (!PGE_GetKey(olc_P).bHeld) {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                front.t[y][x] = perPixel(back.t[y][x], countSurrounded(x, y));
                PGE_Draw(x, y, front.t[y][x] ? olc_PixelRGB(0, 0, 0) : olc_PixelRGB(255, 255, 255));
            }
        }
        back = front;
    }
    else {
        if (PGE_GetMouse(0).bPressed) {
            back.t[PGE_GetMouseY()][PGE_GetMouseX()] = true;
            lmx = PGE_GetMouseX(); lmy = PGE_GetMouseY();
        }
        else if (PGE_GetMouse(0).bHeld) { 
            //back.t[PGE_GetMouseY()][PGE_GetMouseX()] = true;
            drawBresenham(PGE_GetMouseX(), PGE_GetMouseY(), lmx, lmy);
            lmx = PGE_GetMouseX(); lmy = PGE_GetMouseY();
        }

        for (int y = 0; y < HEIGHT; y++)
            for (int x = 0; x < WIDTH; x++)
                PGE_Draw(x, y, back.t[y][x] ? olc_PixelRGB(0, 0, 0) : olc_PixelRGB(255, 255, 255));
    }

    return !PGE_GetKey(olc_ESCAPE).bPressed;
}

bool OnUserCreate() { 
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            front.t[y][x] = false;
            back.t[y][x] = false;
        }
    }

    return true; 
}
bool OnUserDestroy() { return true; }

int main(int argc, char* argv[]) {
    PGE_SetAppName("Cellular Automata");
    if (PGE_Construct(WIDTH, HEIGHT, 2, 2, false, false))
        PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);

    return 0;
}
