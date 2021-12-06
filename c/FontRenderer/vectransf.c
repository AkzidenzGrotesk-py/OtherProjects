#define OLC_PGE_APPLICATION
#include "D:\Users\Kazuto\.k\Projects\Programming\C\Visual Studio Projects\clib\olcPixelGameEngineC.h"
#include "D:\Users\Kazuto\.k\Projects\Programming\C\Visual Studio Projects\clib\uvtypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define MAX_POINTS_PER_TRANSFORM 64
#define FLERP(v0, v1, t) ((1 - (t)) * (v0) + (t) * (v1))

// POINT TYPES
// 0 -> regular
// 1 -> quadratic transform
// 2 -> cubic transform

// ----- QUADRATIC & CUBIC CURVES
float2 quadratic(float2 p0, float2 p1, float2 p2, float t) {
	float2 c = {
		p1.x + powf(1 - t, 2) *
		(p0.x - p1.x) + powf(t, 2) *
		(p2.x - p1.x),
		p1.y + powf(1 - t, 2) *
		(p0.y - p1.y) + powf(t, 2) *
		(p2.y - p1.y),
	};
	return c;
}

float2 cubic(float2 p0, float2 p1, float2 p2, float2 p3, float t) {
	float2 c = {
		powf(1 - t, 3.0f) * p0.x +
		3.0f * powf(1 - t, 2.0f) * t * p1.x +
		(double)3.0f * (1.0f -t)  * powf(t, 2.0f) * p2.x +
		powf(t, 3.0f) * (double)p3.x,
		powf(1 - t, 3.0f)* p0.y +
		3.0f * powf(1 - t, 2.0f) * t * p1.y +
		(double)3.0f * (1.0f - t) * powf(t, 2.0f) * p2.y +
		powf(t, 3.0f) * (double)p3.y
	};
	return c;
}

void XPGE_DrawQuadraticBezier(float2 p1, float2 p2, float2 p3, float delta, olc_Pixel col) {
	float2 lp = p1;
	for (float t = 0; t <= 1.00001; t += delta) {
		float2 c = quadratic(p1, p2, p3, t);
		PGE_DrawLine(lp.x, lp.y, c.x, c.y, col, -1);
		lp = c;
	}
}

void XPGE_DrawCubicBezier(float2 p1, float2 p2, float2 p3, float2 p4, float delta, olc_Pixel col) {
	float2 lp = p1;
	for (float t = 0; t <= 1.00001; t += delta) {
		float2 c = cubic(p1, p2, p3, p4, t);
		PGE_DrawLine(lp.x, lp.y, c.x, c.y, col, -1);
		lp = c;
	}
}

// ----- FLOAT2 FUNCS
float float2_dist2(float2 p1, float2 p2) {
  return sqrt(powf(p1.x - p2.x, 2) + powf(p1.y - p2.y, 2));
}

float2* float2p_new(float x, float y) {
  float2* new_float2 = (float2*) malloc(sizeof(float2));
  new_float2->x = x; new_float2->y = y;
  return new_float2;
}

// ----- POINTTRANSFORM STRUCT
typedef struct {
  float2 point1;
  float2 point2;
} PointTransform;

PointTransform* PointTransform_new(float p1x, float p1y, float p2x, float p2y) {
  PointTransform* new_pt = (PointTransform*) malloc(sizeof(PointTransform));
  new_pt->point1.x = p1x; new_pt->point1.y = p1y;
  new_pt->point2.x = p2x; new_pt->point2.y = p2y;
  return new_pt;
}

void PointTransform_destroy(PointTransform* pt) {free(pt);return;}

// ----- MIDTRANSFORMATION STRUCT
typedef struct {
  float2* pts[MAX_POINTS_PER_TRANSFORM];
  int ptstype[MAX_POINTS_PER_TRANSFORM];
} MidTransformation;

MidTransformation* MidTransformation_new() {
  MidTransformation* new_transf = (MidTransformation*) malloc(sizeof(MidTransformation));
  for (int i = 0; i < MAX_POINTS_PER_TRANSFORM; i++) {
    new_transf->pts[i] = NULL;
    new_transf->ptstype[i] = 0;
  }
  return new_transf;
}

void MidTransformation_destroy(MidTransformation* transf) {
  for (int i = 0; i < MAX_POINTS_PER_TRANSFORM; i++) {
    if (transf->pts[i] != NULL)
      free(transf->pts[i]);
    else break;
  }
  return;
}

// ----- TRANSFORMATION STRUCT
typedef struct {
  PointTransform* pts[MAX_POINTS_PER_TRANSFORM];
  int ptstype[MAX_POINTS_PER_TRANSFORM];
  int n;
} Transformation;

Transformation* Transformation_new() {
  Transformation* new_transf = (Transformation*) malloc(sizeof(Transformation));
  for (int i = 0; i < MAX_POINTS_PER_TRANSFORM; i++) {
    new_transf->pts[i] = NULL;
    new_transf->ptstype[i] = 0;
  }
  new_transf->n = 0;
  return new_transf;
}

void Transformation_destroy(Transformation* transf) {
  for (int i = 0; i < transf->n; i++)
    free(transf->pts[i]);
  return;
}

void Transformation_sdestroy(Transformation* transf) {
  for (int i = 0; i < MAX_POINTS_PER_TRANSFORM; i++) {
    if (transf->pts[i] != NULL)
      free(transf->pts[i]);
    else break;
  }
  return;
}

MidTransformation* Transformation_lerp(Transformation* transf, float t) {
  MidTransformation* f_transf = MidTransformation_new();
  for (int i = 0; i < transf->n; i++) {
    float2* new_pos = (float2*) malloc(sizeof(float2));
    new_pos->x = FLERP(transf->pts[i]->point1.x, transf->pts[i]->point2.x, t);
    new_pos->y = FLERP(transf->pts[i]->point1.y, transf->pts[i]->point2.y, t);
    f_transf->pts[i] = new_pos;
  }
  return f_transf;
}

// ----- ANIMATION FUNCTIONS
void drawFrame(Transformation* animTrans, int drawMode, olc_Pixel pointCol, olc_Pixel lineCol, float* animationTime, int* loopMode, float fElapsedTime) {
  if (*animationTime > 1.0f) {
    if (*loopMode == 0) {
      *animationTime = 0.0f;
    }
    if (*loopMode == 2) {
      *animationTime = 1.0f;
      *loopMode = 3;
    }
  }
  else if (*animationTime < 0.0f) {
    if (*loopMode == 1) {
      *animationTime = 1.0f;
    }
    if (*loopMode == 3) {
      *animationTime = 0.0f;
      *loopMode = 2;
    }
  }
  else {
    switch(*loopMode) {
      case 0:
      case 2:
        *animationTime += fElapsedTime;
        break;
      case 1:
      case 3:
        *animationTime -= fElapsedTime;
        break;
      default:
        printf("%i is not a valid loop mode for animation", *loopMode);
        exit(1);
    }
  }

  MidTransformation* mid_transform = Transformation_lerp(animTrans, *animationTime);
  float2 carry1 = { -1.0f, -1.0f };
  float2 carry2 = { -1.0f, -1.0f };
  int bp = -1;
  for (int i = 0; i < animTrans->n; i++) {
    if (animTrans->ptstype[i] == 0) {
      if (carry1.x != -1.0f && carry2.x == -1.0f) {
        if (drawMode == 0 || drawMode == 2) {
          PGE_FillCircle(mid_transform->pts[i]->x, mid_transform->pts[i]->y, 2, pointCol);
          PGE_FillCircle(carry1.x, carry1.y, 2, pointCol);
        }
        if (drawMode == 1 || drawMode == 2) {
          float2 pointStart = { mid_transform->pts[bp]->x, mid_transform->pts[bp]->y };
          float2 pointEnd = { mid_transform->pts[i]->x, mid_transform->pts[i]->y };
          XPGE_DrawQuadraticBezier(pointStart, carry1, pointEnd, 0.1f, lineCol);
        }

        carry1.x = -1.0f;
        carry1.y = -1.0f;
      } else if (carry1.x != -1.0f && carry2.x != -1.0f) {
        if (drawMode == 0 || drawMode == 2) {
          PGE_FillCircle(mid_transform->pts[i]->x, mid_transform->pts[i]->y, 2, pointCol);
          PGE_FillCircle(carry1.x, carry1.y, 2, pointCol);
          PGE_FillCircle(carry2.x, carry2.y, 2, pointCol);
        }
        if (drawMode == 1 || drawMode == 2) {
          float2 pointStart = { mid_transform->pts[bp]->x, mid_transform->pts[bp]->y };
          float2 pointEnd = { mid_transform->pts[i]->x, mid_transform->pts[i]->y };
          XPGE_DrawCubicBezier(pointStart, carry1, carry2, pointEnd, 0.1f, lineCol);
        }

        carry1.x = -1.0f;
        carry1.y = -1.0f;
        carry2.x = -1.0f;
        carry2.y = -1.0f;
      } else {
        bp = i - 1 == -1 ? (animTrans->n - 1) : (i - 1);
        if (drawMode == 0 || drawMode == 2) PGE_FillCircle(mid_transform->pts[i]->x, mid_transform->pts[i]->y, 2, pointCol);
        if (drawMode == 1 || drawMode == 2) PGE_DrawLine(mid_transform->pts[i]->x, mid_transform->pts[i]->y, mid_transform->pts[bp]->x, mid_transform->pts[bp]->y, lineCol, -1);
      }
    } else if (animTrans->ptstype[i] == 1) {
      bp += 1;
      carry1.x = mid_transform->pts[i]->x;
      carry1.y = mid_transform->pts[i]->y;
    } else if (animTrans->ptstype[i] == 2) {
      if (carry1.x == -1.0f) {
        bp += 1;
        carry1.x = mid_transform->pts[i]->x;
        carry1.y = mid_transform->pts[i]->y;
      } else {
        carry2.x = mid_transform->pts[i]->x;
        carry2.y = mid_transform->pts[i]->y;
      }
    } else {
      printf("unrecognized point type %i", animTrans->ptstype[i]);
      exit(1);
    }
  }
  MidTransformation_destroy(mid_transform);

}

Transformation* transform_2mt_to_t(MidTransformation* mt1, int mt1_len, MidTransformation* mt2, int mt2_len) {
  Transformation* transf = Transformation_new();
  bool claimed_indexes[MAX_POINTS_PER_TRANSFORM];
  for (int i = 0; i < mt2_len; i++)
    claimed_indexes[i] = false;

  if (mt1_len > mt2_len || mt1_len == mt2_len) {
    for (int i = 0; i < mt1_len; i++) {
      float shortest = INFINITY;
      int point_index = -1;
      for (int j = 0; j < mt2_len; j++) {
        float dist = float2_dist2(*mt1->pts[i], *mt2->pts[j]);
        if (dist < shortest && !claimed_indexes[j]) { // && mt1->ptstype[i] == mt2->ptstype[i]) {
          shortest = dist;
          point_index = j;
        }
      }
      claimed_indexes[point_index] = true;
      int total = 0;
      for (int j = 0; j < mt2_len; j++)
        total += claimed_indexes[j] ? 1 : 0;
      printf("total: %i\n", total);
      if (total == mt2_len)
        for (int j = 0; j < mt2_len; j++)
          claimed_indexes[j] = false;


      transf->pts[i] = PointTransform_new(mt1->pts[i]->x, mt1->pts[i]->y, mt2->pts[point_index]->x, mt2->pts[point_index]->y);
      transf->ptstype[i] = mt1->ptstype[point_index]; 
    }
    transf->n = mt1_len;
  } else {
    return transform_2mt_to_t(mt2, mt2_len, mt1, mt1_len);
  }

  return transf;
}

// ----- PROGRAM
Transformation* line_transf;
float animTime = 0.0f;
int animMode = 2;

bool OnUserUpdate(float fElapsedTime) {
  PGE_Clear(olc_BLACK);
  drawFrame(line_transf, 2, olc_PixelRGB(0, 255, 0), olc_PixelRGB(0, 255, 0), &animTime, &animMode, fElapsedTime);
  return !PGE_GetKey(olc_ESCAPE).bPressed;
}

bool OnUserCreate() {
  /*MidTransformation* square_pts = MidTransformation_new();
  square_pts->pts[0] = float2p_new(200, 600);
  square_pts->pts[1] = float2p_new(600, 600);
  square_pts->pts[2] = float2p_new(600, 200);
  square_pts->pts[3] = float2p_new(200, 200);*/

  MidTransformation* circ_pts = MidTransformation_new();
  circ_pts->pts[0] = float2p_new(400, 700);
  circ_pts->pts[1] = float2p_new(700, 700);circ_pts->ptstype[1] = 1;
  circ_pts->pts[2] = float2p_new(700, 400);
  circ_pts->pts[3] = float2p_new(700, 100);circ_pts->ptstype[3] = 1;
  circ_pts->pts[4] = float2p_new(400, 100);
  circ_pts->pts[5] = float2p_new(100, 100);circ_pts->ptstype[5] = 1;
  circ_pts->pts[6] = float2p_new(100, 400);
  circ_pts->pts[7] = float2p_new(100, 700);circ_pts->ptstype[7] = 1;
  circ_pts->pts[8] = float2p_new(400, 700);

  MidTransformation* octa_pts = MidTransformation_new();
  octa_pts->pts[0] = float2p_new(300, 700);
  octa_pts->pts[1] = float2p_new(500, 700);octa_pts->ptstype[1] = 1;
  octa_pts->pts[2] = float2p_new(700, 500);
  octa_pts->pts[3] = float2p_new(700, 300);octa_pts->ptstype[3] = 1;
  octa_pts->pts[4] = float2p_new(500, 100);
  octa_pts->pts[5] = float2p_new(300, 100);octa_pts->ptstype[5] = 1;
  octa_pts->pts[6] = float2p_new(100, 300);
  octa_pts->pts[7] = float2p_new(100, 500);octa_pts->ptstype[7] = 1;
  octa_pts->pts[9] = float2p_new(300, 700);

  line_transf = transform_2mt_to_t(octa_pts, 9, circ_pts, 9);
  MidTransformation_destroy(circ_pts);
  MidTransformation_destroy(octa_pts);

  printf("GENERATED POINTS:\n----------\n");
  for (int i = 0; i < line_transf->n; i++) {
    printf("point %i: (%.1f, %.1f) -> (%.1f, %.1f), type: %i\n", i + 1, line_transf->pts[i]->point1.x, line_transf->pts[i]->point1.y, line_transf->pts[i]->point2.x, line_transf->pts[i]->point2.y, line_transf->ptstype[i]);
  }

  return true;
}
bool OnUserDestroy() {
  Transformation_destroy(line_transf);

  return true;
}

int main(int argc, char* argv[]) {
    PGE_SetAppName("vectranf Tester");
    if (PGE_Construct(800, 800, 1, 1, false, false))
        PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);

    return 0;
}
