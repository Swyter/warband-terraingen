#pragma once

#include "Vector4.h"

extern unsigned char g_perlinPermutationTable[256];
extern float g_perlinGradientTable[256 * 3];

#define PERLIN_CLAMP(x) ((unsigned char)(x))
#define PERLIN_PERMUTATION(x) g_perlinPermutationTable[PERLIN_CLAMP(x)]
#define PERLIN_GRADIENT(x) &g_perlinGradientTable[3 * PERLIN_PERMUTATION(x)]

Vector4 PerlinOctave(float frequency, float gain, int numOctaves, const Vector4 &in);
Vector4 Perlin(float factor, const Vector4 &in);
