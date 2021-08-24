#define OLC_PGE_APPLICATION
#include "olcPixelGameEngineC.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define WHEIGHT 720
#define WWIDTH 1280
#define AGENT_COUNT 2000

typedef struct {
  float x; float y;
} float2;

typedef struct {
  float x; float y; float z; float w;
} float4;

typedef struct {
  int x; int y;
} int2;

typedef struct {
  float2 position;
  float angle;
} Agent;

typedef struct {
  float4 t[WHEIGHT][WWIDTH];
} Map;

Agent agents[AGENT_COUNT] = {0.0f};
Map TrailMap = {0.0f};
Map ProcessedTrailMap = {0.0f};

float moveSpeed = 20;
float evaporateSpeed = 0.05;
float diffuseSpeed = 0.1;
float sensorAngleSpacing = 52 * 180 / 3.14159;
float turnSpeed = 10;
float sensorOffsetDst = 15;
float sensorSize = 1;

void swapDishes(Map **a, Map **b){Map *tmp = *a;*a = *b;*b = tmp;}
void swap() {Map* pgrid = &ProcessedTrailMap;Map* pnext = &TrailMap;swapDishes(&pgrid, &pnext);ProcessedTrailMap = *pgrid;TrailMap = *pnext;}
int hash(int state) {state ^= 2747636419;state *= 2654435769;state ^= state >> 16;state *= 2654435769;state ^= state >> 16;state *= 2654435769;return state;}
float lerp(float a, float b, float f) {return (a * (1.0 - f)) + (b * f);}

// -- SENSE
float sense(Agent agent, float sensorAngleOffset) {
  float sensorAngle = agent.angle + sensorAngleOffset;
  float2 sensorDir = {cosf(sensorAngle), sinf(sensorAngle)};
  int2 sensorCentre = {agent.position.x + sensorDir.x * sensorOffsetDst, agent.position.y + sensorDir.y * sensorOffsetDst};
  float sum = 0.0f;

  for (int offsetX = -sensorSize; offsetX <= sensorSize; offsetX++) {
    for (int offsetY = -sensorSize; offsetY <= sensorSize; offsetY++) {
      int2 pos = {sensorCentre.x + offsetX, sensorCentre.y + offsetY};

      if (pos.x >= 0 && pos.x < WWIDTH && pos.y >= 0 && pos.y < WHEIGHT) sum += TrailMap.t[pos.y][pos.x].x;
    }
  }
  return sum;
}

// -- FRAME
bool OnUserUpdate(float fElapsedTime)
{
  for (int i = 0; i < AGENT_COUNT; i++) {
    Agent agent = agents[i];
    int random = hash(agent.position.y * WWIDTH + agent.position.x + hash(i));

    float2 direction = {cosf(agent.angle), sinf(agent.angle)};
    float2 newPos = {agent.position.x + direction.x * moveSpeed * fElapsedTime, agent.position.y + direction.y * moveSpeed * fElapsedTime};

    if (newPos.x < 0 || newPos.x >= WWIDTH || newPos.y < 0 || newPos.y >= WHEIGHT) {
      newPos.x = fmin(WWIDTH-0.01f, fmax(0.0f, newPos.x));
      newPos.y = fmin(WHEIGHT-0.01f, fmax(0.0f, newPos.y));
      agents[i].angle = ((double)rand() / (double)RAND_MAX) * 2.0f * 3.14159f;
    }

    agents[i].position = newPos;
    TrailMap.t[(int)newPos.y][(int)newPos.x].x = 1.0f;
    TrailMap.t[(int)newPos.y][(int)newPos.x].y = 1.0f;
    TrailMap.t[(int)newPos.y][(int)newPos.x].z = 1.0f;
    TrailMap.t[(int)newPos.y][(int)newPos.x].w = 1.0f;


    float weightForward = sense(agent, 0);
    float weightLeft = sense(agent, sensorAngleSpacing);
    float weightRight = sense(agent, -sensorAngleSpacing);

    float randomSteerStrength = (double)rand() / (double)RAND_MAX;

    if (weightForward > weightLeft && weightForward > weightRight) agents[i].angle += 0;
    else if (weightForward < weightLeft && weightForward < weightRight) agents[i].angle += (randomSteerStrength - 0.5) * 2 * turnSpeed * fElapsedTime;
    else if (weightRight > weightLeft) agents[i].angle -= randomSteerStrength * turnSpeed * fElapsedTime;
    else if (weightLeft > weightRight) agents[i].angle += randomSteerStrength * turnSpeed * fElapsedTime;
  }

  for (int j = 0; j < WHEIGHT; j++) {
    for (int k = 0; k < WWIDTH; k++) {
      float4 originalValue = TrailMap.t[j][k];
      float4 sum = { 0.0f };
      for (int offsetX = -1; offsetX <= 1; offsetX++) {
        for (int offsetY = -1; offsetY <= 1; offsetY++) {
          int sampleX = k + offsetX;
          int sampleY = j + offsetY;

          if (sampleX >= 0 && sampleX < WWIDTH && sampleY >= 0 && sampleY < WHEIGHT) {
            sum.x += TrailMap.t[sampleY][sampleX].x;
            sum.y += TrailMap.t[sampleY][sampleX].y;
            sum.z += TrailMap.t[sampleY][sampleX].z;
            sum.w += TrailMap.t[sampleY][sampleX].w;
          }
        }
      }
      int s = 9;
      float4 blurResult = {sum.x / s, sum.y / s, sum.z / s, sum.w / s};

      float4 diffusedValue = {
        lerp(originalValue.x, blurResult.x, diffuseSpeed * fElapsedTime),
        lerp(originalValue.y, blurResult.y, diffuseSpeed * fElapsedTime),
        lerp(originalValue.z, blurResult.z, diffuseSpeed * fElapsedTime),
        lerp(originalValue.w, blurResult.w, diffuseSpeed * fElapsedTime)
      };

      float4 diffusedAndEvaporatedValue = {
        fmax(0.0f, diffusedValue.x - evaporateSpeed * fElapsedTime),
        fmax(0.0f, diffusedValue.y - evaporateSpeed * fElapsedTime),
        fmax(0.0f, diffusedValue.z - evaporateSpeed * fElapsedTime),
        fmax(0.0f, diffusedValue.w - evaporateSpeed * fElapsedTime)
      };

      ProcessedTrailMap.t[j][k].x = fmax(0.0f, diffusedValue.x - evaporateSpeed * fElapsedTime);
      ProcessedTrailMap.t[j][k].y = fmax(0.0f, diffusedValue.y - evaporateSpeed * fElapsedTime);
      ProcessedTrailMap.t[j][k].z = fmax(0.0f, diffusedValue.z - evaporateSpeed * fElapsedTime);
      ProcessedTrailMap.t[j][k].w = fmax(0.0f, diffusedValue.w - evaporateSpeed * fElapsedTime);

      PGE_Draw(k, j, olc_PixelRGBA(TrailMap.t[j][k].x * 255, TrailMap.t[j][k].y * 255, TrailMap.t[j][k].z * 255, TrailMap.t[j][k].w * 255));

      // TrailMap = ProcessedTrailMap;
    }
  }

  TrailMap = ProcessedTrailMap;

  // for (int i = 0; i < AGENT_COUNT; i++) PGE_Draw(agents[i].position.x, agents[i].position.y, olc_PixelRGBA(255, 255, 0, 255));

  return true;
}

// -- DESTROY
bool OnUserDestroy() { return true; }

// -- SETUP
bool OnUserCreate() {
  for (int i = 0; i < AGENT_COUNT; i++) {
    agents[i].position.x = WWIDTH / 2 + ((double)rand() / (double)RAND_MAX) * 6 - 3;
    agents[i].position.y = WHEIGHT / 2 + ((double)rand() / (double)RAND_MAX) * 6 - 3;
    agents[i].angle = ((double)rand() / (double)RAND_MAX) * 2.0f * 3.14159f;
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
