
typedef struct {float x; float y;} float2;
typedef struct {float x; float y; float z;} float3;
typedef struct {float x; float y; float z; float w;} float4;
typedef struct {int x; int y;} int2;
typedef struct {int x; int y; int z;} int3;
typedef struct {int x; int y; int z; int w;} int4;

float2 float2_Add(float2 a, float2 b) {float2 o = {a.x + b.x, a.y + b.y};return o;}
float2 float2_Sub(float2 a, float2 b) {float2 o = {a.x - b.x, a.y - b.y};return o;}
float2 float2_fDiv(float2 a, float b) {float2 o = {a.x / b, a.y / b};return o;}
float2 float2_fMul(float2 a, float b) {float2 o = {a.x * b, a.y * b};return o;}
float2 float2_Div(float2 a, float2 b) {float2 o = {a.x / b.x, a.y / b.y};return o;}
float2 float2_Mul(float2 a, float2 b) {float2 o = {a.x * b.x, a.y * b.y};return o;}
void float2_Set(float2* a, float x, float y) {a->x = x;a->y = y;}
void float2_f2Set(float2* a, float2* b) {a->x = b->x;a->y = b->y;}
void float2_f3Set(float2* a, float3* b) {a->x = b->x;a->y = b->y;}
void float2_f4Set(float2* a, float4* b) {a->x = b->x;a->y = b->y;}

float3 float3_Add(float3 a, float3 b) {float3 o = {a.x + b.x, a.y + b.y, a.z + b.z};return o;}
float3 float3_Sub(float3 a, float3 b) {float3 o = {a.x - b.x, a.y - b.y, a.z - b.z};return o;}
float3 float3_fDiv(float3 a, float b) {float3 o = {a.x / b, a.y / b, a.z / b};return o;}
float3 float3_fMul(float3 a, float b) {float3 o = {a.x * b, a.y * b, a.z * b};return o;}
float3 float3_Div(float3 a, float3 b) {float3 o = {a.x / b.x, a.y / b.y, a.z / b.z};return o;}
float3 float3_Mul(float3 a, float3 b) {float3 o = {a.x * b.x, a.y * b.y, a.z * b.z};return o;}
void float3_Set(float3* a, float x, float y, float z) {a->x = x;a->y = y;a->z = z;}
void float3_f2Set(float3* a, float2* b) {a->x = b->x;a->y = b->y;a->z = 0.0f;}
void float3_f3Set(float3* a, float3* b) {a->x = b->x;a->y = b->y;a->z = b->z;}
void float3_f4Set(float3* a, float4* b) {a->x = b->x;a->y = b->y;a->z = b->z;}

float4 float4_Add(float4 a, float4 b) {float4 o = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};return o;}
float4 float4_Sub(float4 a, float4 b) {float4 o = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};return o;}
float4 float4_fDiv(float4 a, float b) {float4 o = {a.x / b, a.y / b, a.z / b, a.w / b};return o;}
float4 float4_fMul(float4 a, float b) {float4 o = {a.x * b, a.y * b, a.z * b, a.w * b};return o;}
float4 float4_Div(float4 a, float4 b) {float4 o = {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};return o;}
float4 float4_Mul(float4 a, float4 b) {float4 o = {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};return o;}
void float4_Set(float4* a, float x, float y, float z, float w) {a->x = x;a->y = y;a->z = z;a->w = w;}
void float4_f2Set(float4* a, float2* b) {a->x = b->x;a->y = b->y;a->z = 0.0f;a->w = 0.0f;}
void float4_f3Set(float4* a, float3* b) {a->x = b->x;a->y = b->y;a->z = b->z;a->w = 0.0f;}
void float4_f4Set(float4* a, float4* b) {a->x = b->x;a->y = b->y;a->z = b->z;a->w = b->w;}
void float4_i4Set(float4* a, int4* b) {a->x = b->x;a->y = b->y;a->z = b->z;a->w = b->w;}

int2 int2_Add(int2 a, int2 b) {int2 o = {a.x + b.x, a.y + b.y};return o;}
int2 int2_Sub(int2 a, int2 b) {int2 o = {a.x - b.x, a.y - b.y};return o;}
int2 int2_fDiv(int2 a, float b) {int2 o = {a.x / b, a.y / b};return o;}
int2 int2_fMul(int2 a, float b) {int2 o = {a.x * b, a.y * b};return o;}
int2 int2_Div(int2 a, int2 b) {int2 o = {a.x / b.x, a.y / b.y};return o;}
int2 int2_Mul(int2 a, int2 b) {int2 o = {a.x * b.x, a.y * b.y};return o;}
void int2_Set(int2* a, int x, int y) {a->x = x;a->y = y;}
void int2_i2Set(int2* a, int2* b) {a->x = b->x;a->y = b->y;}
void int2_i3Set(int2* a, int3* b) {a->x = b->x;a->y = b->y;}
void int2_i4Set(int2* a, int4* b) {a->x = b->x;a->y = b->y;}

int3 int3_Add(int3 a, int3 b) {int3 o = {a.x + b.x, a.y + b.y, a.z + b.z};return o;}
int3 int3_Sub(int3 a, int3 b) {int3 o = {a.x - b.x, a.y - b.y, a.z - b.z};return o;}
int3 int3_fDiv(int3 a, float b) {int3 o = {a.x / b, a.y / b, a.z / b};return o;}
int3 int3_fMul(int3 a, float b) {int3 o = {a.x * b, a.y * b, a.z * b};return o;}
int3 int3_Div(int3 a, int3 b) {int3 o = {a.x / b.x, a.y / b.y, a.z / b.z};return o;}
int3 int3_Mul(int3 a, int3 b) {int3 o = {a.x * b.x, a.y * b.y, a.z * b.z};return o;}
void int3_Set(int3* a, int x, int y, int z) {a->x = x;a->y = y;a->z = z;}
void int3_i2Set(int3* a, int2* b) {a->x = b->x;a->y = b->y;a->z = 0.0f;}
void int3_i3Set(int3* a, int3* b) {a->x = b->x;a->y = b->y;a->z = b->z;}
void int3_i4Set(int3* a, int4* b) {a->x = b->x;a->y = b->y;a->z = b->z;}

int4 int4_Add(int4 a, int4 b) {int4 o = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};return o;}
int4 int4_Sub(int4 a, int4 b) {int4 o = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};return o;}
int4 int4_fDiv(int4 a, float b) {int4 o = {a.x / b, a.y / b, a.z / b, a.w / b};return o;}
int4 int4_fMul(int4 a, float b) {int4 o = {a.x * b, a.y * b, a.z * b, a.w * b};return o;}
int4 int4_Div(int4 a, int4 b) {int4 o = {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};return o;}
int4 int4_Mul(int4 a, int4 b) {int4 o = {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};return o;}
void int4_Set(int4* a, int x, int y, int z, int w) {a->x = x;a->y = y;a->z = z;a->w = w;}
void int4_i2Set(int4* a, int2* b) {a->x = b->x;a->y = b->y;a->z = 0.0f;a->w = 0.0f;}
void int4_i3Set(int4* a, int3* b) {a->x = b->x;a->y = b->y;a->z = b->z;a->w = 0.0f;}
void int4_i4Set(int4* a, int4* b) {a->x = b->x;a->y = b->y;a->z = b->z;a->w = b->w;}

int2 f2toi2(float2 a) {int2 o = {a.x, a.y};return o;}
int2 f3toi2(float3 a) {int2 o = {a.x, a.y};return o;}
int2 f4toi2(float4 a) {int2 o = {a.x, a.y};return o;}
float4 i4tof4(int4 a) {float4 o = {a.x, a.y, a.z, a.w};return o;}
