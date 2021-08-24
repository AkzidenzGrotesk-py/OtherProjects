#define OLC_PGE_APPLICATION
#include "olcPixelGameEngineC.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "uvtypes.h"

// -- CONSTANTS
#define WHEIGHT 720
#define WWIDTH 1280
#define AGENT_MAX 5000

// -- STRUCTURES
typedef struct {
  float2 position;
  float angle;
  int4 speciesMask;
} Agent;

typedef struct {
  float4 t[WHEIGHT][WWIDTH];
} Map;

// -- GLOBALS
Agent agents[AGENT_MAX] = {0.0f};
Map TrailMap = {0.0f};
Map ProcessedTrailMap = {0.0f};

float moveSpeed = 20;
float evaporateSpeed = 0.05;
float diffuseSpeed = 0.1;
float sensorAngleSpacing = 3.14159;
float turnSpeed = 30;
float sensorOffsetDst = 5;
float sensorSize = 10;

bool mode = true;
bool nopausemenu = false;
bool drawHeads = false;

int AGENT_COUNT = 2000;
int SPECIES_COUNT = 3;

int wscale = 2;
int hscale = 2;

// -- UTILITY
int hash(int state) {state ^= 2747636419;state *= 2654435769;state ^= state >> 16;state *= 2654435769;state ^= state >> 16;state *= 2654435769;return state;}
float lerp(float a, float b, float f) {return (a * (1.0 - f)) + (b * f);}
float rand_01(int seed) {return fmin(0.0f, fmax(1.0f, hash(seed) / 4294967295.0));}

bool mouseInSquare(int ax, int ay, int bx, int by) {
  if (PGE_GetMouseX() > ax && PGE_GetMouseX() < bx)
    if (PGE_GetMouseY() > ay && PGE_GetMouseY() < by)
      return true;
  return false;
}

// -- SENSE
float sense(Agent agent, float sensorAngleOffset) {
  float sensorAngle = agent.angle + sensorAngleOffset;
  float2 sensorDir = {cosf(sensorAngle), sinf(sensorAngle)};
  int2 sensorCentre = int2_Add(f2toi2(agent.position), f2toi2(float2_fMul(sensorDir, sensorOffsetDst)));

  float sum = 0.0f;
  for (int offsetX = -sensorSize; offsetX <= sensorSize; offsetX++) {
    for (int offsetY = -sensorSize; offsetY <= sensorSize; offsetY++) {
      int2 pos = {sensorCentre.x + offsetX, sensorCentre.y + offsetY};

      if (pos.x >= 0 + wscale && pos.x < WWIDTH - wscale && pos.y >= 0 + hscale && pos.y < WHEIGHT - hscale)  {
        sum += TrailMap.t[pos.y][pos.x].x * (agent.speciesMask.x * 2 - 1) + TrailMap.t[pos.y][pos.x].y * (agent.speciesMask.y * 2 - 1) + TrailMap.t[pos.y][pos.x].z * (agent.speciesMask.z * 2 - 1) + TrailMap.t[pos.y][pos.x].w * (agent.speciesMask.w * 2 - 1); //TrailMap.t[pos.y][pos.x].x;
      }
    }
  }

  return sum;
}

// -- FRAME
bool OnUserUpdate(float fElapsedTime)
{
  // -- GLOBAL CONTROLS
  if (PGE_GetKey(olc_P).bPressed) mode = !mode;
  if (PGE_GetKey(olc_M).bPressed) nopausemenu = !nopausemenu;
  if (PGE_GetKey(olc_R).bPressed) {
    PGE_DrawString(WWIDTH/2-8*12, WHEIGHT/2-4, "Resetting...", olc_WHITE, 3);
    for (int j = 0 + hscale; j < WHEIGHT - hscale; j++) {
      for (int k = 0 + wscale; k < WWIDTH - wscale; k++) {
        float4_Set(&TrailMap.t[j][k], 0.0f, 0.0f, 0.0f, 0.0f);
        float4_Set(&ProcessedTrailMap.t[j][k], 0.0f, 0.0f, 0.0f, 0.0f);
      }
    }
    for (int i = 0; i < AGENT_MAX; i++) {
      agents[i].position.x = WWIDTH / 2 + ((double)rand() / (double)RAND_MAX) * 100 - 50;
      agents[i].position.y = WHEIGHT / 2 + ((double)rand() / (double)RAND_MAX) * 100 - 50;
      agents[i].angle = ((double)rand() / (double)RAND_MAX) * 2.0f * 3.14159f;

      int r = rand() % SPECIES_COUNT;
      if (SPECIES_COUNT > 1) {
        if (r==0) int4_Set(&agents[i].speciesMask, 0, 0, 1, 0);
        else if (r==1) int4_Set(&agents[i].speciesMask, 0, 1, 0, 0);
        else if (r==2) int4_Set(&agents[i].speciesMask, 1, 0, 0, 0);
      } else {
        int4_Set(&agents[i].speciesMask, 1, 1, 1, 0);
      }
    }
  }
  if (PGE_GetKey(olc_D).bPressed) drawHeads = !drawHeads;

  // -- BORDER
  if (wscale > 1 && hscale > 1) {
    PGE_Clear(olc_BLACK);
    PGE_DrawRect(wscale-1, hscale-1, WWIDTH-wscale*2+1, WHEIGHT-hscale*2+1, olc_MAGENTA);
  }


  // -- SIMULATION
  if (mode) {
    // -- AGENTS
    for (int i = 0; i < AGENT_COUNT; i++) {
      // random
      Agent agent = agents[i];
      int random = hash(agent.position.y * (WWIDTH - wscale) + agent.position.x + hash(i));

      // move
      float2 direction = {cosf(agent.angle), sinf(agent.angle)};
      float2 newPos = {agent.position.x + direction.x * moveSpeed * fElapsedTime, agent.position.y + direction.y * moveSpeed * fElapsedTime};

      // edge bounce
      if (newPos.x < 0 + wscale || newPos.x >= WWIDTH-1-wscale || newPos.y < 0 + hscale || newPos.y >= WHEIGHT-1-hscale) {
        newPos.x = fmin(WWIDTH-0.01f- wscale, fmax(0.0f+ wscale, newPos.x));
        newPos.y = fmin(WHEIGHT-0.01f- hscale, fmax(0.0f+ hscale, newPos.y));
        agents[i].angle = rand_01(random++) * 2.0f * 3.14159f;
      }

      // update position
      agents[i].position = newPos;
      float4_Set(&TrailMap.t[(int)newPos.y][(int)newPos.x], (float)agent.speciesMask.x, (float)agent.speciesMask.y, (float)agent.speciesMask.z, 1.0f);

      // sense
      float weightForward = sense(agent, 0);
      float weightLeft = sense(agent, sensorAngleSpacing);
      float weightRight = sense(agent, -sensorAngleSpacing);

      // random
      float randomSteerStrength = rand_01(random++);

      // turn according to senses
      if (weightForward > weightLeft && weightForward > weightRight) agents[i].angle += 0;
      else if (weightForward < weightLeft && weightForward < weightRight) agents[i].angle += (randomSteerStrength - 0.5) * 2 * turnSpeed * fElapsedTime;
      else if (weightRight > weightLeft) agents[i].angle -= randomSteerStrength * turnSpeed * fElapsedTime;
      else if (weightLeft > weightRight) agents[i].angle += randomSteerStrength * turnSpeed * fElapsedTime;
    }

    // -- TRAILS
    for (int j = 0 + hscale; j < WHEIGHT - hscale; j++) {
      for (int k = 0 + wscale; k < WWIDTH - wscale; k++) {
        // collect values and average
        float4 originalValue = TrailMap.t[j][k];
        float4 sum = { 0.0f };
        for (int offsetX = -1; offsetX <= 1; offsetX++) {
          for (int offsetY = -1; offsetY <= 1; offsetY++) {
            int sampleX = k + offsetX;
            int sampleY = j + offsetY;

            if (sampleX >= 0 + wscale && sampleX < WWIDTH - wscale && sampleY >= 0 + hscale && sampleY < WHEIGHT - hscale) {
              sum = float4_Add(sum, TrailMap.t[sampleY][sampleX]);
            }
          }
        }
        float4 blurResult = float4_fDiv(sum, 9.0f);

        // diffuse
        float4 diffusedValue = {
          lerp(originalValue.x, blurResult.x, diffuseSpeed * fElapsedTime),
          lerp(originalValue.y, blurResult.y, diffuseSpeed * fElapsedTime),
          lerp(originalValue.z, blurResult.z, diffuseSpeed * fElapsedTime),
          lerp(originalValue.w, blurResult.w, diffuseSpeed * fElapsedTime)
        };

        // evaporate
        float4 diffusedAndEvaporatedValue = {
          fmax(0.0f, diffusedValue.x - evaporateSpeed * fElapsedTime),
          fmax(0.0f, diffusedValue.y - evaporateSpeed * fElapsedTime),
          fmax(0.0f, diffusedValue.z - evaporateSpeed * fElapsedTime),
          fmax(0.0f, diffusedValue.w - evaporateSpeed * fElapsedTime)
        };

        // update processed
        float4_f4Set(&ProcessedTrailMap.t[j][k], &diffusedAndEvaporatedValue);

        // draw
        PGE_Draw(k, j, olc_PixelRGBA(TrailMap.t[j][k].x * 255, TrailMap.t[j][k].y * 255, TrailMap.t[j][k].z * 255, TrailMap.t[j][k].w * 255));
      }
    }

    // flip
    TrailMap = ProcessedTrailMap;
  }

  // -- HEADS
  if (drawHeads) for (int i = 0; i < AGENT_COUNT; i++) PGE_Draw(agents[i].position.x, agents[i].position.y, olc_PixelRGBA(255, 255, 255, 255));

  // -- GUI
  if (!mode || nopausemenu) {
    // controls
    int mwDelta = PGE_GetMouseWheel();
    bool qKey = PGE_GetKey(olc_Q).bHeld;
    if (mwDelta > 0 && mouseInSquare(232, 50, 296, 58)) moveSpeed += qKey ? 0.5f : 0.05f;
    if (mwDelta < 0 && mouseInSquare(232, 50, 296, 58)) moveSpeed -= qKey ? 0.5f : 0.05f;
    if (mwDelta > 0 && mouseInSquare(232, 63, 296, 71)) turnSpeed += qKey ? 0.5f : 0.05f;
    if (mwDelta < 0 && mouseInSquare(232, 63, 296, 71)) turnSpeed -= qKey ? 0.5f : 0.05f;
    if (mwDelta > 0 && mouseInSquare(232, 76, 296, 84)) evaporateSpeed += qKey ? 0.5f : 0.05f;
    if (mwDelta < 0 && mouseInSquare(232, 76, 296, 84)) evaporateSpeed -= qKey ? 0.5f : 0.05f;
    if (mwDelta > 0 && mouseInSquare(232, 89, 296, 97)) diffuseSpeed += qKey ? 0.5f : 0.05f;
    if (mwDelta < 0 && mouseInSquare(232, 89, 296, 97)) diffuseSpeed -= qKey ? 0.5f : 0.05f;
    if (mwDelta > 0 && mouseInSquare(232, 102, 296, 110)) sensorAngleSpacing += qKey ? 0.5f : 0.05f;
    if (mwDelta < 0 && mouseInSquare(232, 102, 296, 110)) sensorAngleSpacing -= qKey ? 0.5f : 0.05f;
    if (mwDelta > 0 && mouseInSquare(232, 115, 296, 123)) sensorOffsetDst += qKey ? 2.0f : 0.5f;
    if (mwDelta < 0 && mouseInSquare(232, 115, 296, 123)) sensorOffsetDst -= qKey ? 2.0f : 0.5f;
    if (mwDelta > 0 && mouseInSquare(232, 128, 296, 132)) sensorSize += qKey ? 2.0f : 0.5f;
    if (mwDelta < 0 && mouseInSquare(232, 128, 296, 132)) sensorSize -= qKey ? 2.0f : 0.5f;

    if (mwDelta > 0 && mouseInSquare(232, 154, 316, 164)) {
      AGENT_COUNT += qKey ? 100 : 10; AGENT_COUNT = fmin(AGENT_MAX, AGENT_COUNT);
    }
    if (mwDelta < 0 && mouseInSquare(232, 154, 316, 164)) {
      AGENT_COUNT -= qKey ? 100 : 10; AGENT_COUNT = fmax(10, AGENT_COUNT);
    }
    if (mwDelta > 0 && mouseInSquare(232, 167, 296, 175)) SPECIES_COUNT += SPECIES_COUNT < 3 ? 1 : 0;
    if (mwDelta < 0 && mouseInSquare(232, 167, 296, 175)) SPECIES_COUNT -= SPECIES_COUNT > 1 ? 1 : 0;

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

    // draw bg
    for (int j = 0 + hscale; j < WHEIGHT - hscale; j++)
      for (int k = 0 + wscale; k < WWIDTH - wscale; k++)
        PGE_Draw(k, j, olc_PixelRGBA(TrailMap.t[j][k].x * 255, TrailMap.t[j][k].y * 255, TrailMap.t[j][k].z * 255, TrailMap.t[j][k].w * 255));

    // colours
    olc_Pixel light_blue = olc_PixelRGB(150, 175, 255);
    olc_Pixel light_green = olc_PixelRGB(150, 255, 175);
    olc_Pixel light_red = olc_PixelRGB(255, 150, 175);
    olc_Pixel slime_green = olc_PixelRGB(15, 210, 60);

    // heading
    PGE_DrawString(16, 16, "Slime Mould", slime_green, 1);
    PGE_DrawString(16, 28, "Simulation Settings", olc_WHITE, 2);

    // settings
    PGE_DrawString(16, 50, "Particle Move Speed -", light_green, 1);
      char ta[8];snprintf(ta, sizeof(ta), "%.3f", moveSpeed);
      PGE_DrawString(232, 50, ta, mouseInSquare(232, 50, 296, 58) ? olc_YELLOW : olc_WHITE, 1);

    PGE_DrawString(16, 63, "Particle Turn Speed -", light_green, 1);
      char tb[8];snprintf(tb, sizeof(tb), "%.3f", turnSpeed);
      PGE_DrawString(232, 63, tb, mouseInSquare(232, 63, 296, 71) ? olc_YELLOW : olc_WHITE, 1);

    PGE_DrawString(16, 76, "Trail Diffusion Speed -", light_red, 1);
      char tc[8];snprintf(tc, sizeof(tc), "%.3f", evaporateSpeed);
      PGE_DrawString(232, 76, tc, mouseInSquare(232, 76, 296, 84) ? olc_YELLOW : olc_WHITE, 1);

    PGE_DrawString(16, 89, "Trail Evaporation Speed -", light_red, 1);
      char td[8];snprintf(td, sizeof(td), "%.3f", diffuseSpeed);
      PGE_DrawString(232, 89, td, mouseInSquare(232, 89, 296, 97) ? olc_YELLOW : olc_WHITE, 1);

    PGE_DrawString(16, 102, "Sensor Angle Spacing -", light_blue, 1);
      char te[8];snprintf(te, sizeof(te), "%.3f", sensorAngleSpacing);
      PGE_DrawString(232, 102, te, mouseInSquare(232, 102, 296, 110) ? olc_YELLOW : olc_WHITE, 1);

    PGE_DrawString(16, 115, "Sensor Offset Distance -", light_blue, 1);
      char tf[8];snprintf(tf, sizeof(tf), "%.3f", sensorOffsetDst);
      PGE_DrawString(232, 115, tf, mouseInSquare(232, 115, 296, 123) ? olc_YELLOW : olc_WHITE, 1);

    PGE_DrawString(16, 128, "Sensor Size -", light_blue, 1);
      char tg[8];snprintf(tg, sizeof(tg), "%.3f", sensorSize);
      PGE_DrawString(232, 128, tg, mouseInSquare(232, 128, 296, 132) ? olc_YELLOW : olc_WHITE, 1);

    PGE_DrawString(16, 154, "# of Particles -", olc_WHITE, 1);
      char th[32];snprintf(th, sizeof(th), "%i/%i", AGENT_COUNT, AGENT_MAX);
      PGE_DrawString(232, 154, th, mouseInSquare(232, 154, 316, 164) ? olc_YELLOW : olc_WHITE, 1);
    PGE_DrawString(16, 167, "# of Species -", olc_WHITE, 1);
      char ti[8];snprintf(ti, sizeof(ti), "%i/3", SPECIES_COUNT);
      PGE_DrawString(232, 167, ti, mouseInSquare(232, 167, 296, 175) ? olc_YELLOW : olc_WHITE, 1);

    PGE_DrawString(16, 193, "Width Scaling -", olc_WHITE, 1);
      char tj[8];snprintf(tj, sizeof(tj), "%i", wscale);
      PGE_DrawString(232, 193, tj, mouseInSquare(232, 193, 296, 201) ? olc_YELLOW : olc_WHITE, 1);
    PGE_DrawString(16, 206, "Height Scaling -", olc_WHITE, 1);
      char tk[8];snprintf(tk, sizeof(tk), "%i", hscale);
      PGE_DrawString(232, 206, tk, mouseInSquare(232, 206, 296, 214) ? olc_YELLOW : olc_WHITE, 1);
  }

  // -- GLOBAL GUI
  PGE_DrawString(WWIDTH-8*19, WHEIGHT-16, "[P] - Pause", mode ? olc_WHITE : olc_YELLOW, 1);
  PGE_DrawString(WWIDTH-8*19, WHEIGHT-28, "[R] - Reset Frame", olc_WHITE, 1);
  PGE_DrawString(WWIDTH-8*19, WHEIGHT-40, "[D] - Draw Heads", drawHeads ? olc_YELLOW : olc_WHITE, 1);
  PGE_DrawString(WWIDTH-8*19, WHEIGHT-52, "[M] - Menu", nopausemenu ? olc_YELLOW : olc_WHITE, 1);

  return true;
}

// -- DESTROY
bool OnUserDestroy() { return true; }

// -- SETUP
bool OnUserCreate() {
  for (int i = 0; i < AGENT_MAX; i++) {
    agents[i].position.x = WWIDTH / 2 + ((double)rand() / (double)RAND_MAX) * 100 - 50;
    agents[i].position.y = WHEIGHT / 2 + ((double)rand() / (double)RAND_MAX) * 100 - 50;
    agents[i].angle = ((double)rand() / (double)RAND_MAX) * 2.0f * 3.14159f;

    int r = rand() % SPECIES_COUNT;
    if (SPECIES_COUNT > 1) {
      if (r==0) int4_Set(&agents[i].speciesMask, 0, 0, 1, 0);
      else if (r==1) int4_Set(&agents[i].speciesMask, 0, 1, 0, 0);
      else if (r==2) int4_Set(&agents[i].speciesMask, 1, 0, 0, 0);
    } else {
      int4_Set(&agents[i].speciesMask, 1, 1, 1, 0);
    }
  }

  return true;
}

int main(int argc, char* argv[])
{
  PGE_SetAppName("Slime Mould Simulation");
  if(PGE_Construct(WWIDTH, WHEIGHT, 1, 1, false, false))
    PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);

  return 0;
}
