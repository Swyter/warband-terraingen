#include "Utils.h"
#include <cstdlib>
#include <cmath>

unsigned int g_randSeed;

float Min(float a, float b)
{
	return a <= b ? a : b;
}

int Min(int a, int b)
{
	return a <= b ? a : b;
}

int Min(unsigned int a, unsigned int b)
{
	return a <= b ? a : b;
}

float Max(float a, float b)
{
	return a >= b ? a : b;
}

int Max(int a, int b)
{
	return a >= b ? a : b;
}

unsigned int Max(unsigned int a, unsigned int b)
{
	return a >= b ? a : b;
}

float Clamp(float value, float min, float max)
{
	if (value < min)
		return min;

	if (value > max)
		return max;

	return value;
}

int ClampExclusive(int value, int min, int max)
{
	if (value < min)
		return min;

	if (value >= max)
		return max - 1;

	return value;
}

int ClampInclusive(int value, int min, int max)
{
	if (value < min)
		return min;

	if (value > max)
		return max;

	return value;
}

unsigned int ClampExclusive(unsigned int value, unsigned int min, unsigned int max)
{
	if (value < min)
		return min;

	if (value >= max)
		return max - 1;

	return value;
}

unsigned int ClampInclusive(unsigned int value, unsigned int min, unsigned int max)
{
	if (value < min)
		return min;

	if (value > max)
		return max;

	return value;
}

float Randf()
{
	return rand() % 15817 / 15817.0f;
}

float Randf(float max)
{
	return Randf() * max;
}

float Randf(float min, float max)
{
	return Randf() * (max - min) + min;
}

int Rand(int mod)
{
	return mod > 1 ? (rand() % mod) : 0;
}

float DegToRad(float degrees)
{
	return degrees * 0.0174532925f;
}

float RadToDeg(float radians)
{
	return radians * 57.2957795f;
}

int Round(float value)
{
	float f = floor(value);

	return (value - f > 0.5f) ? (int)f + 1 : (int)f;
}

int Round1(float value)
{
	if (value <= 0.0f)
		return (int)ceil(value - 0.00000001f);
	else
		return (int)ceil(value + 0.00000001f);
}

int Round2(float value)
{
	if (value <= 0.0f)
		return (int)(value - 0.9999999f);
	else
		return (int)value;
}

__int64 Round64(float value)
{
	float f = floor(value);

	return (value - f > 0.5f) ? (__int64)f + 1 : (__int64)f;
}

bool Between(int value, int min, int max)
{
	return value >= min && value < max;
}

bool Between(float value, float min, float max)
{
	return value >= min && value < max;
}

Vector4 MakeNormalFromPlane(const Vector4 &p0, const Vector4 &p1, const Vector4 &p2)
{
	Vector4 normal = (p1 - p0).cross(p2 - p0);

	normal.normalize();
	return normal;
}
