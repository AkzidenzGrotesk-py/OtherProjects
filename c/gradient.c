#include <stdio.h>
#include <stdlib.h>

int lerp(int x0, int x1, float t) {return (int)(x0+(x1-x0)*t);}

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} RGBColor;

char* RGBColor_char(RGBColor* self) {
  static char colo_block[32];
  snprintf(colo_block, sizeof(colo_block), "\x1b[38;2;%d;%d;%dm\xDB\x1b[0m", self->r, self->g, self->b);
  return colo_block;
}

RGBColor* RGBColor_construct(unsigned char r, unsigned char g, unsigned char b) {
  RGBColor* color = (RGBColor*) malloc(sizeof(RGBColor));
  color->r = r; color->g = g; color->b = b;
  return color;
}

struct Gradient {
  RGBColor* color;
  struct Gradient* next;
};

struct Gradient* Gradient_construct(RGBColor* color) {
  struct Gradient* grad = (struct Gradient*) malloc(sizeof(struct Gradient));
  grad->color = color;
  grad->next = NULL;
  return grad;
}

struct Gradient* Gradient_generate(RGBColor* start, RGBColor* end, int steps) {
  float stepsize = 1.0 / steps;
  struct Gradient* head = Gradient_construct(start);
  struct Gradient* nextitem = head;
  for (float t = stepsize; t <= 1.00001; t += stepsize) {
    int red = lerp(start->r, end->r, t);
    int green = lerp(start->g, end->g, t);
    int blue = lerp(start->b, end->b, t);
    struct Gradient* next = Gradient_construct(RGBColor_construct(red, green, blue));
    nextitem->next = next;
    nextitem = next;
  }
  return head;
}

void Gradient_vrender(struct Gradient* n, int pad) {
  for (int i = 0; i < pad; i++)
    printf("%s\n", RGBColor_char(n->color));
  while (1 == 1) {
    printf("%s\n", RGBColor_char(n->color));
    if (n->next == NULL) break;
    n = n->next;
  }
  for (int i = 0; i < pad; i++)
    printf("%s\n", RGBColor_char(n->color));
}

void Gradient_vrrender(struct Gradient* n, int pad, int width) {
  for (int i = 0; i < pad; i++) {
    for (int j = 0; j < width; j++)
      printf("%s", RGBColor_char(n->color));
    printf("\n");
  }
  while (1 == 1) {
    for (int j = 0; j < width; j++)
      printf("%s", RGBColor_char(n->color));
    printf("\n");
    if (n->next == NULL) break;
    n = n->next;
  }
  for (int i = 0; i < pad; i++) {
    for (int j = 0; j < width; j++)
      printf("%s", RGBColor_char(n->color));
    printf("\n");
  }
}

void Gradient_hrender(struct Gradient* n, int pad) {
  for (int i = 0; i < pad; i++)
    printf("%s", RGBColor_char(n->color));
  while (1 == 1) {
    printf("%s", RGBColor_char(n->color));
    if (n->next == NULL) break;
    n = n->next;
  }
  for (int i = 0; i < pad; i++)
    printf("%s", RGBColor_char(n->color));
}

void Gradient_hrrender(struct Gradient* n, int pad, int height) {
  for (int i = 0; i < height; i++) {
    Gradient_hrender(n, pad);
    printf("\n");
  }
}

int main(void) {
  Gradient_hrrender(Gradient_generate(RGBColor_construct(185, 43, 39), RGBColor_construct(21, 101, 192), 100),5,30);
  Gradient_hrrender(Gradient_generate(RGBColor_construct(241, 39, 17), RGBColor_construct(245, 175, 25), 100),5,30);
  Gradient_hrrender(Gradient_generate(RGBColor_construct(195, 20, 50), RGBColor_construct(36, 11, 56), 100),5,30);
  Gradient_vrrender(Gradient_generate(RGBColor_construct(195, 20, 50), RGBColor_construct(36, 11, 56), 100),5,30);
  return 0;
}
