#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "Counter.h"

Counter* Counter_new(int init_num) {
  Counter* newc = (Counter*) malloc(sizeof(Counter));
  for (int i = 0; i < MAX_COUNTER_VALS; i++) {
    if (i < init_num)
      newc->counts[i] = 0;
    else
      newc->counts[i] = -1;
    newc->values[i] = 0;
  }
  return newc;
}

void Counter_print(Counter* self) {
  printf("{");
  for (int i = 0; i < MAX_COUNTER_VALS; i++) {
    if (self->counts[i] <= -1)
      break;
    else
      printf("%i: %i, ", self->values[i], self->counts[i]);
  }
  printf("}\n");
}

int Counter_change(Counter* self, int value, int count) {
  for (int i = 0; i < MAX_COUNTER_VALS; i++) {
    if (self->values[i] == value) {
      self->counts[i] = count;
      return count;
    }
    else if (self->counts[i] <= -1) {
      self->values[i] = value;
      self->counts[i] = count;
      return count;
    }
  }
  return -1;
}

void Counter_fix(Counter* self) {
  Counter* new = Counter_new(0);
  for (int i = 0; i < MAX_COUNTER_VALS; i++) {
    bool is_duplicate = false;
    if (self->counts[i] <= -1)
      break;
    for (int j = i + 1; j < MAX_COUNTER_VALS; j++) {
      if (self->counts[j] <= -1)
        break;
      if (self->values[j] == self->values[i]) {
        Counter_change(new, self->values[j], self->counts[j] + self->counts[i] + Counter_get(new, self->values[j]));
        self->counts[j] = 0;
        is_duplicate = true;
      }
    }
    if (!is_duplicate)
      Counter_change(new, self->values[i], self->counts[i] + Counter_get(new, self->values[i]));
  }
  *self = *new;
}

int Counter_get(Counter* self, int value) {
  int total = 0;
  for (int i = 0; i < MAX_COUNTER_VALS; i++) {
    if (self->counts[i] <= -1)
      break;
    if (self->values[i] == value) {
      total = self->counts[i];
      break;
    }
  }
  return total;
}

int Counter_sumget(Counter* self, int value) {
  int total = 0;
  for (int i = 0; i < MAX_COUNTER_VALS; i++) {
    if (self->counts[i] <= -1)
      break;
    if (self->values[i] == value)
      total += self->counts[i];
  }
  return total;
}

int Counter_total(Counter* self) {
  int total = 0;
  for (int i = 0; i < MAX_COUNTER_VALS; i++) {
    if (self->counts[i] <= -1)
      break;
    total += self->counts[i];
  }
  return total;
}

void Counter_delete(Counter* self, int value) {
  Counter* new = Counter_new(0);
  for (int i = 0; i < MAX_COUNTER_VALS; i++) {
    if (self->counts[i] <= -1)
      break;
    if (self->values[i] == value)
      continue;
    Counter_change(new, self->values[i], self->counts[i]);
  }
  *self = *new;
}

int* Counter_elements(Counter* self) {
  static int elems[MAX_ELEMENTS];
  int e = 1;
  for (int i = 0; i < MAX_COUNTER_VALS; i++) {
    if (self->counts[i] <= -1)
      break;
    for (int j = 0; j < self->counts[i]; j++)
      elems[e++] = self->values[i];
  }
  elems[0] = e - 1;
  return elems;
}
