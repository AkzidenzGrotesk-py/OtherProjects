#define OLC_PGE_APPLICATION
#include "olcPixelGameEngineC.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// settings area WWIDTH, WHEIGHT and max_iter

#define WWIDTH 720
#define WHEIGHT 720
#define PXLSCALE 1
#define fmap(x,a,b,c,d) (((x)-(a))/((b)-(a))*((d)-(c))+(c))

int max_iter = 150;
float zoom = 3;
float camera_pos[2] = { 0, 0 };

bool OnUserUpdate(float fElapsedTime)
{
    int mDelta = PGE_GetMouseWheel();
    float speed = 3.0f * (PGE_GetKey(olc_CTRL).bHeld ? 2 : 1);
    if (mDelta > 0) {
        zoom -= (speed * zoom) * fElapsedTime; zoom = fmax(zoom, 0.0f);
    }
    if (mDelta < 0) {
        zoom += (speed * zoom) * fElapsedTime;
    }

    if (PGE_GetKey(olc_UP).bHeld || PGE_GetKey(olc_W).bHeld) {
        camera_pos[1] -= speed * zoom * fElapsedTime;
    }

    if (PGE_GetKey(olc_DOWN).bHeld || PGE_GetKey(olc_S).bHeld) {
        camera_pos[1] += speed * zoom * fElapsedTime;
    }

    if (PGE_GetKey(olc_LEFT).bHeld || PGE_GetKey(olc_A).bHeld) {
        camera_pos[0] -= speed * zoom * fElapsedTime;
    }

    if (PGE_GetKey(olc_RIGHT).bHeld || PGE_GetKey(olc_D).bHeld) {
        camera_pos[0] += speed * zoom * fElapsedTime;
    }
    


    float cxmn = -zoom + camera_pos[0];
    float cxmx = zoom + camera_pos[0];
    float cymn = -zoom + camera_pos[1];
    float cymx = zoom + camera_pos[1];

    for (int x = 1; x < WWIDTH-1; x++) {
        for (int y = 1; y < WHEIGHT-1; y++) {
            float a = fmap(x, 0.0f, WWIDTH, cxmn, cxmx);
            float b = fmap(y, 0.0f, WHEIGHT, cymn, cymx);
            float ca = a;
            float cb = b;
            
            int z = 0;
            for (int n = 0; n < max_iter; n++) {
                float aa = a * a - b * b;
                float bb = 2 * a * b;
                a = aa + ca; b = bb + cb;

                if (a + b > 16) break;
                z = n;
            }
            
            int brightness = fmap((float)z, 0.0f, (float)max_iter, 0.0f, 255.0f);
            if (z == (max_iter-1)) brightness = 0;
            PGE_Draw(x, y, olc_PixelRGBA(brightness, 0, 0, 255));
        }
    }

    // system("pause");
    return true;
}


bool OnUserCreate() { return true; }
bool OnUserDestroy() {return true;}

int main(int argc, char* argv[])
{
    PGE_SetAppName("Mandelbrot Set");
    if (PGE_Construct(WWIDTH, WHEIGHT, PXLSCALE, PXLSCALE, false, false))
        PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);

    return 0;
}
