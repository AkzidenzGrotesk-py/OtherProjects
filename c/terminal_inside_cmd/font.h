#ifndef BMPCHAR_H
#define BMPCHAR_H

char *binchar(char *c, unsigned char n);

/* Bitmap character structure
 * Each character contains 8 bits -> 00000000
 * There are 8 characters creating an 8x8 grid
 * These will be converted to be rendered.
 */
struct bmpchar { unsigned char l[8]; };
struct bmpchar *bmpchar_make();
void bmpchar_render(struct bmpchar *self, int column, int row, char on, char off);

/* Bitmap character sheet
 * Contains enough slots for 256 characters, 0-255
 * Matches the ASCII table.
 */
struct bmpsheet { struct bmpchar *s[256]; };
void bmpsheet_empty(struct bmpsheet *self);
void bmpsheet_crender(struct bmpsheet *self, char c, int column, int row, char on, char off);
void bmpsheet_srender(struct bmpsheet *self, char *str, int column, int row, char on, char off);
int bmpsheet_tofile(struct bmpsheet *self, char *name);
int bmpsheet_fromfile(struct bmpsheet *buf, char *name);
void bmpsheet_free(struct bmpsheet *self);

#endif
