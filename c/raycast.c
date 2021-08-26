#define OLC_PGE_APPLICATION
#include "...\source\clib\olcPixelGameEngineC.h"
#include "...\source\clib\uvtypes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// --
#define WWIDTH 800
#define WHEIGHT 800
#define PXLSCALE 1
#define PRAYS 360
#define NWALLS 12

// -- BOUNDARY
typedef struct { float2 a; float2 b; } Boundary;
void drawBoundary(Boundary b) { PGE_DrawLine(b.a.x, b.a.y, b.b.x, b.b.y, olc_WHITE, -1); }

// -- RAY
typedef struct { float2 pos; bool isIntersecting; } Intersect;
typedef struct { float2 pos; float2 dir; } Ray;
void drawRay(Ray r) { PGE_DrawLine(r.pos.x, r.pos.y, r.pos.x + r.dir.x * 10, r.pos.y + r.dir.y * 10, olc_WHITE, -1); }
void rayLookAt(Ray* r, float x, float y) { r->dir.x = x - r->pos.x; r->dir.y = y - r->pos.y; float2_normalize(&r->dir); }
Intersect castRay(Ray r, Boundary b) {
    Intersect i = { 0.0f, 0.0f, false };
    const float x1 = b.a.x; const float y1 = b.a.y;
    const float x2 = b.b.x; const float y2 = b.b.y;
    const float x3 = r.pos.x; const float y3 = r.pos.y;
    const float x4 = r.pos.x + r.dir.x; 
    const float y4 = r.pos.y + r.dir.y;

    const float den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (den == 0) return i;

    const float t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
    const float u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;

    if (t > 0 && t < 1 && u > 0) {
        i.isIntersecting = true;
        i.pos.x = x1 + t * (x2 - x1);
        i.pos.y = y1 + t * (y2 - y1);
    }

    return i;
}

// -- PARTICLE
typedef struct { float2 pos; Ray rays[PRAYS]; } Particle;

void moveParticle(Particle* p, float nx, float ny) {
    p->pos.x = nx; p->pos.y = ny;
    for (int i = 0; i < PRAYS; i++) { p->rays[i].pos.x = nx; p->rays[i].pos.y = ny; }
}
void drawParticle(Particle p) {
    PGE_FillCircle(p.pos.x, p.pos.y, 2, olc_WHITE);
    for (int i = 0; i < PRAYS; i++) drawRay(p.rays[i]);
}
void particleLook(Particle p, Boundary* b) {
    for (int i = 0; i < PRAYS; i++) {
        Intersect closest = {0.0f, 0.0f, false};
        float record = INFINITY;
        for (int w = 0; w < NWALLS; w++) {
            Intersect j = castRay(p.rays[i], b[w]);
            if (j.isIntersecting) { 
                const float yh = (j.pos.y - p.pos.y);
                const float xh = (j.pos.x - p.pos.x);
                const float d = sqrtf(yh * yh + xh * xh);
                if (d < record) {
                    record = d;
                    float2_f2Set(&closest.pos, &j);
                    closest.isIntersecting = true;
                }

                // PGE_DrawLine(j.pos.x, j.pos.y, p.pos.x, p.pos.y, olc_YELLOW, -1); 
            }
        }
        // printf("%.3f\n", record);
        if (closest.isIntersecting) {
            PGE_DrawLine(closest.pos.x, closest.pos.y, p.pos.x, p.pos.y, olc_WHITE, -1);
        }
    }
}

// -- GLOBALS
Boundary walls[NWALLS];
Ray ray = { 100.0f, 200.0f, 1.0f, 0.0f };
Particle particle = { WWIDTH / 2, WHEIGHT / 2 };

// -- FRAME
bool OnUserUpdate(float fElapsedTime)
{
    PGE_Clear(olc_BLACK);

    // draw boundaries
    for (int j = 0; j < NWALLS; j++) {
        drawBoundary(walls[j]);
    }

    // draw and update particle
    particleLook(particle, &walls);
    drawParticle(particle);
    moveParticle(&particle, PGE_GetMouseX(), PGE_GetMouseY());

	return true;
}

// -- CREATE
bool OnUserCreate() {

    float degtoradval = (360.0f / 3.14159f / 2.0f);
    float incr = 360.0f / (float)PRAYS;
    for (int i = 0; i < PRAYS; i++) {
        // move particle's ray to its position
        particle.rays[i].pos.x = particle.pos.x;
        particle.rays[i].pos.y = particle.pos.y;

        // create vecors to point outwards
        particle.rays[i].dir.x = cosf(((float)i * incr) / degtoradval);
        particle.rays[i].dir.y = sinf(((float)i * incr) / degtoradval);

        // normalize, just in case.
        float2_normalize(&particle.rays[i].dir);
    }

    for (int j = 0; j < NWALLS; j++) {
        // random positions
        int x1 = rand() % WWIDTH;
        int x2 = rand() % WWIDTH;
        int y1 = rand() % WHEIGHT;
        int y2 = rand() % WHEIGHT;

        // set
        float2_Set(&walls[j].a, x1, y1);
        float2_Set(&walls[j].b, x2, y2);
    }

    float2_Set(&walls[0].a, 0, 0);
    float2_Set(&walls[0].b, WWIDTH, 0);
    float2_Set(&walls[1].a, WWIDTH, 0);
    float2_Set(&walls[1].b, WWIDTH, WHEIGHT);
    float2_Set(&walls[2].a, WWIDTH, WHEIGHT);
    float2_Set(&walls[2].b, 0, WHEIGHT);
    float2_Set(&walls[3].a, 0, WHEIGHT);
    float2_Set(&walls[3].b, 0, 0);

    return true; 
}

// -- DESTROY
bool OnUserDestroy() { return true; }

// --
int main(int argc, char* argv[])
{
    PGE_SetAppName("Raycast");
    if (PGE_Construct(WWIDTH, WHEIGHT, PXLSCALE, PXLSCALE, false, false))
        PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);

    return 0;
}
