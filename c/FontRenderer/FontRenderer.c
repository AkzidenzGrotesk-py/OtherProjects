#define OLC_PGE_APPLICATION
#include <C:\Users\Kazuto (^^)\source\clib\olcPixelGameEngineC.h>
#include <C:\Users\Kazuto (^^)\source\clib\uvtypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAXVERTS 32
#define MAXBREAKS 4
#define MAXCHARS 256
#define MAXLINELENGTH 256

typedef struct {
	float3 v[MAXVERTS];
	int b[MAXBREAKS];
	int d;
	int n;
	char c;
} Glyph;

typedef struct {
	Glyph f[MAXCHARS];
} Face;

Face myFace;

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

void ProduceFontface(Face* f) {
	for (int i = 0; i < MAXCHARS; i++) {
		f->f[i].c = ' ';
		f->f[i].n = 0;
		for (int j = 0; j < MAXVERTS; j++) {
			f->f[i].v[j].x = 0.0f;
			f->f[i].v[j].y = 0.0f;
		}

		for (int j = 0; j < MAXBREAKS; j++)
			f->f[i].b[j] = -1;
	}
}

bool LoadFaceFromFile(char* sFileName, Face* font, bool strict) {
	FILE* f = fopen(sFileName, "r");
	char line[MAXLINELENGTH];
	char working_char = NULL;
	int vcount = 0;
	int bcount = 0;

	while (fgets(line, sizeof(line), f)) {
		char delim[] = " ";

		// Choose GLYPHSET to write to; syntax: g [CHAR]
		if (line[0] == 'g') {
			char* ptr = strtok(line, delim);

			while (ptr != NULL) {
				working_char = ptr[0];
				ptr = strtok(NULL, delim);
			}

			printf("Writing to glyph \033[35m[%c]\033[0m...\n", (char)working_char);

			vcount = 0;
			bcount = 0;
		}

		// 
		// Add new POINT to current GLYPHSET; syntax: 
		//		p [x] [y]			-> add regular point
		//		p [x] [y] [type]	-> add point of specific type
		//								0: REGULAR
		//								1: QUAD BEZIER CURVE POINT
		//								2: CUBIC BEZIER CURVE POINT
		//								3: INVERSE
		//		p # [n]				-> add point copy (macro) of point at position n
		//
		else if (line[0] == 'p') {
			if ((int)working_char != NULL) {
				char* ptr = strtok(line, delim);
				int r = -1;
				bool macro_mode = false;
				float3 pr = { 0.0f, 0.0f, 0.0f };

				while (ptr != NULL) {
					if (r == 0) { 
						if (ptr[0] == '#') macro_mode = true;
						else pr.x = atof(ptr); 
					}
					else if (r == 1) { 
						if (macro_mode) {
							float3_f3Set(&pr, &font->f[(int)working_char].v[atoi(ptr)]);
						}
						else pr.y = atof(ptr); 
					}
					else if (r == 2 && !macro_mode) pr.z = atof(ptr);

					r++;
					ptr = strtok(NULL, delim);
				}

				font->f[(int)working_char].c = working_char;
				font->f[(int)working_char].n = vcount + 1;
				font->f[(int)working_char].v[vcount].x = pr.x;
				font->f[(int)working_char].v[vcount].y = pr.y;
				font->f[(int)working_char].v[vcount].z = pr.z;

				if (!macro_mode) printf("Written point \033[35m[%f, %f, %f]\033[0m to glyph \033[35m[%c]\033[0m, totalling \033[35m[%i]\033[0m points.\n", pr.x, pr.y, pr.z, working_char, vcount + 1);
				else printf("Written MACRO point \033[35m[%f, %f, %f]\033[0m to glyph \033[35m[%c]\033[0m, totalling \033[35m[%i]\033[0m points.\n", pr.x, pr.y, pr.z, working_char, vcount + 1);

				vcount++;

				if (vcount >= MAXVERTS) {
					printf("\033[33mwarning:\033[0m written maximum points, resetting to avoid overflow.\n");
					vcount = 0;
					if (strict) return false;
				}
			}
			else {
				printf("\033[33mwarning:\033[0m failing to write point; no glyphset.\n");
				if (strict) return false;
			}
		}

		// 
		// Add new BREAKPOINT to current GLYPHSET; syntax:
		//		b [n]		-> add breakpoint at position N
		//		b *			-> add breakpoint (auto) here
		//
		else if (line[0] == 'b') {
			if ((int)working_char != NULL) {
				char* ptr = strtok(line, delim);
				int r = -1;
				int breakpoint = -1;
				bool autob = false;

				while (ptr != NULL) {
					if (r == 0) { 
						if (ptr[0] == '*') {
							breakpoint = vcount; autob = true;
						}
						else breakpoint = atoi(ptr);
					}

					r++;
					ptr = strtok(NULL, delim);
				}

				font->f[(int)working_char].b[bcount] = breakpoint;
				font->f[(int)working_char].d = bcount + 1;

				if (!autob) printf("Written breakpoint \033[35m[%i]\033[0m to glyph \033[35m[%c]\033[0m, totalling \033[35m[%i]\033[0m breakpoints.\n", breakpoint, working_char, bcount + 1);
				else printf("Written AUTO breakpoint \033[35m[%i]\033[0m to glyph \033[35m[%c]\033[0m, totalling \033[35m[%i]\033[0m breakpoints.\n", breakpoint, working_char, bcount + 1);


				bcount++;

				if (bcount >= MAXBREAKS) {
					printf("\033[33mwarning:\033[0m written maximum breakpoints, resetting to avoid overflow.\n");
					bcount = 0;
					if (strict) return false;
				}
			}
			else {
				printf("\033[33mwarning:\033[0m failing to write break; no glyphset.\n");
				if (strict) return false;
			}
		}
	}

	fclose(f);
	return true;
}

void Draw_FaceText(char* sText, int fsize, int lspace, int kspace, olc_Pixel col, Face* font) {
	int sx = 0; int sy = 0;
	float letter_space = fsize * kspace;
	float line_space = fsize * lspace;

	for (int i = 0; i < strlen(sText); i++) {
		char c = sText[i];
		if (c == '\n') { sx = 0; sy += line_space; }
		else {
			int rb = 0;
			bool breakl = false;
			float2 sp[3] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
			int curve_type = 0;

			// loop through verticies
			for (int j = 0; j < font->f[(int)c].n; j++) {
				breakl = false;

				// check if breakline
				for (int k = rb; k < font->f[(int)c].d; k++) {
					if (j == font->f[(int)c].b[k]) {
						breakl = true; rb++;
					}
				}

				float2 point = { font->f[(int)c].v[j].x, font->f[(int)c].v[j].y };
				point = float2_fMul(point, fsize);
				int point_type = font->f[(int)c].v[j].z;

				if (point_type == 0) {
					float2 spec_point = { sx, sy };
					if (curve_type == 0) {
						if (j > 0 && !breakl)
							PGE_DrawLine(point.x + sx, point.y + sy, sp[0].x + sx, sp[0].y + sy, col, -1);
					}
					else if (curve_type == 1) {
						XPGE_DrawQuadraticBezier(float2_Add(point, spec_point), 
							float2_Add(sp[1], spec_point),
							float2_Add(sp[0], spec_point), 0.1f, col);
						curve_type = 0;
					}
					else if (curve_type == 2) {
						XPGE_DrawCubicBezier(float2_Add(point, spec_point),
							float2_Add(sp[2], spec_point),
							float2_Add(sp[1], spec_point),
							float2_Add(sp[0], spec_point), 0.1f, col);
						curve_type = 0;
					}

					sp[0] = point;
				}
				else if (point_type == 1) {
					sp[1] = point;
					curve_type = 1;
				}
				else if (point_type == 2) {
					if (curve_type == 0) {
						sp[1] = point;
						curve_type = 1;
					}
					else {
						sp[2] = point;
						curve_type = 2;
					}
				}
			}

			sx += letter_space;
		}
	}
}

bool OnUserUpdate(float deltatime) { 
	Draw_FaceText("ABO", 32, 1, 1, olc_WHITE, &myFace);


	return true; 
}

bool OnUserCreate(float deltatime) { 
	ProduceFontface(&myFace);
	LoadFaceFromFile("font.kf", &myFace, true);

	return true; 
}
bool OnUserDestroy(float deltatime) { return true;  }

int main() {
	PGE_SetAppName("Font Renderer");
	if (PGE_Construct(800, 800, 1, 1, false, false))
		PGE_Start(&OnUserCreate, &OnUserUpdate, &OnUserDestroy);


	return 0;
}
