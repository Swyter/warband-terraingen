#pragma once

#if !defined _MSC_VER // need exact MSVC implementation of rand
extern unsigned int g_randSeed;

void srand(unsigned int seed)
{
	g_randSeed = seed;
}

int rand()
{
	g_randSeed = 214013 * g_randSeed + 2531011;
	return (g_randSeed >> 16) & 0x7FFF;
}
#endif

#include "Vector4.h"

#define PI 3.14159265f
#define LOG2_E 1.44269504f
#define BITSHIFT64(x) (1ULL << (x))
#define BITSHIFT(x) (1 << (x))

float Min(float a, float b);
int Min(int a, int b);
int Min(unsigned int a, unsigned int b);
float Max(float a, float b);
int Max(int a, int b);
unsigned int Max(unsigned int a, unsigned int b);
float Clamp(float value, float min, float max);
int ClampExclusive(int value, int min, int max);
int ClampInclusive(int value, int min, int max);
unsigned int ClampExclusive(int value, unsigned int min, unsigned int max);
unsigned int ClampInclusive(int value, unsigned int min, unsigned int max);
float Randf();
float Randf(float max);
float Randf(float min, float max);
int Rand(int mod);
float DegToRad(float degrees);
float RadToDeg(float radians);
int Round(float value);
int Round1(float value);
int Round2(float value);
__int64 Round64(float value);
bool Between(int value, int min, int max);
bool Between(float value, float min, float max);
Vector4 MakeNormalFromPlane(const Vector4 &p0, const Vector4 &p1, const Vector4 &p2);
