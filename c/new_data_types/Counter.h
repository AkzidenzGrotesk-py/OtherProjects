#ifndef COUNTER_H
#define COUNTER_H

#define MAX_COUNTER_VALS 256
#define MAX_ELEMENTS 256*8

typedef struct {
  int values[MAX_COUNTER_VALS];
  int counts[MAX_COUNTER_VALS];
} Counter;

// Generate new Counter object
Counter* Counter_new(int init_num);

// Print out Counter values up to the first value with a count of -1 or lower
void Counter_print(Counter* self);

// Change the count at a value, or add it if it does not exist
int Counter_change(Counter* self, int value, int count);

// Merge all duplicate values up to the first value with a count of -1 or lower
void Counter_fix(Counter* self);

// Get count at first occurance of value, returns 0 if it does not exist
int Counter_get(Counter* self, int value);

// Get sum count at all occurances of value, returns 0 if it does not exist
int Counter_sumget(Counter* self, int value);

// Get total of all values up to first value with a count of -1 or lower
int Counter_total(Counter* self);

// Delete a value out of the Counter
void Counter_delete(Counter* self, int value);

// Get an array of elements, first element is length of array
// Get elements with *(elements + index)
int* Counter_elements(Counter* self);

#endif
