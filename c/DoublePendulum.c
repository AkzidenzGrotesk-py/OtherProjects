#define OLC_PGE_APPLICATION
#include "..\source\clib\olcPixelGameEngineC.h"
#include "..\source\clib\uvtypes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// --
#define WWIDTH 900
#define WHEIGHT 600
#define PXLSCALE 1
#define PI 3.14159

float r1 = 200.0f;
float r2 = 200.0f;
float m1 = 20.0f;
float m2 = 20.0f;
float a1 = PI / 2.0f;
float a2 = PI / 2.0f;
float a1_v = 0;
float a2_v = 0;
float a1_a = 0.01;
float a2_a = -0.001;
float g = 1;


float m1v, m2v;

int2 origin = { WWIDTH / 2, 100 };
int4 canvas[WHEIGHT][WWIDTH] = { 255, 255, 255, 255 };

// -- UPDATE
bool OnUserUpdate(float fElapsedTime) {

	float num1 = -g * (2 * m1 + m2) * sinf(a1);
	float num2 = -m2 * g * sinf(a1 - 2 * a2);
	float num3 = -2 * sinf(a1 - a2) * m2;
	float num4 = a2_v * a2_v * r2 + a1_v * a1_v * r1 * cosf(a1 - a2);
	float den = r1 * (2 * m1 + m2 - m2 * cosf(2 * a1 - 2 * a2));
	a1_a = (num1 + num2 + num3 * num4) / den;

	num1 = 2 * sinf(a1 - a2);
	num2 = (a1_v * a1_v * r1 * (m1 + m2));
	num3 = g * (m1 + m2) * cosf(a1);
	num4 = a2_v * a2_v * r2 * m2 * cosf(a1 - a2);
	den = r2 * (2 * m1 + m2 - m2 * cosf(2 * a1 - 2 * a2));
	a2_a = (num1 * (num2 + num3 + num4)) / den;

	// PGE_Clear(olc_WHITE);
	for (int y = 0; y < WHEIGHT; y++)
		for (int x = 0; x < WWIDTH; x++)
			PGE_Draw(x, y, olc_PixelRGBA(canvas[y][x].x, canvas[y][x].y, canvas[y][x].z, canvas[y][x].w));
	float x1 = r1 * sinf(a1);
	float y1 = r1 * cosf(a1);

	float x2 = x1 + r2 * sinf(a2);
	float y2 = y1 + r2 * cosf(a2);

	PGE_DrawLine(origin.x, origin.y, origin.x + x1, origin.y + y1, olc_BLACK, -1);
	PGE_FillCircle(origin.x + x1, origin.y + y1, m1v, olc_BLACK);

	PGE_DrawLine(origin.x + x1, origin.y + y1, origin.x + x2, origin.y + y2, olc_BLACK, -1);
	PGE_FillCircle(origin.x + x2, origin.y + y2, m2v, olc_BLACK);

	a1_v += a1_a;
	a2_v += a2_a;
	a1 += a1_v;
	a2 += a2_v;

	float mposy = origin.y + y2;
	float mposx = origin.x + x2;

	if (mposy > 0 && mposy < WHEIGHT-1 && mposx > 0 && mposx < WWIDTH-1) int4_Set(&canvas[(int)mposy][(int)mposx], 255, 0, 0, 200);

	return true;
}

// -- CREATE
bool OnUserCreate(float fElapsedTime) { 
	m1v = m1 * 0.5f; m2v = m2 * 0.5f;

	for (int y = 0; y < WHEIGHT; y++)
		for (int x = 0; x < WWIDTH; x++)
			int4_Set(&canvas[y][x], 255, 255, 255, 255);

	return true; 
}

// -- DESTROY
bool OnUserDestroy(float fElapsedTime) { return true; }

// --
int main() {
	PGE_SetAppName("Double Pendulum Simulation");
	if (PGE_Construct(WWIDTH, WHEIGHT, PXLSCALE, PXLSCALE, false, false))
		PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserCreate);

	return 0;
}
