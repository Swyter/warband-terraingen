#include "Vector2.h"
#include "Utils.h"
#include <cmath>

const Vector2 Vector2::Zero = Vector2(0.0f);
const Vector2 Vector2::One = Vector2(1.0f);

Vector2::Vector2()
{
}

Vector2::Vector2(float f)
{
	x = f;
	y = f;
}

Vector2::Vector2(float fx, float fy)
{
	x = fx;
	y = fy;
}

float Vector2::dot(const Vector2 &v) const
{
	return x * v.x + y * v.y;
}

float Vector2::length() const
{
	return sqrt(x * x + y * y);
}

float Vector2::lengthSquared() const
{
	return x * x + y * y;
}

float Vector2::normalize()
{
	float len = length();

	if (len <= 0.0f)
	{
		x = 0.0f;
		y = 1.0f;
	}
	else
	{
		x /= len;
		y /= len;
	}

	return len;
}

Vector2 Vector2::operator +(float f) const
{
	return Vector2(x + f, y + f);
}

Vector2 Vector2::operator +(const Vector2 &v) const
{
	return Vector2(x + v.x, y + v.y);
}

Vector2 Vector2::operator -() const
{
	return Vector2(-x, -y);
}

Vector2 Vector2::operator -(float f) const
{
	return Vector2(x - f, y - f);
}

Vector2 Vector2::operator -(const Vector2 &v) const
{
	return Vector2(x - v.x, y - v.y);
}

Vector2 Vector2::operator *(float f) const
{
	return Vector2(x * f, y * f);
}

Vector2 Vector2::operator *(const Vector2 &v) const
{
	return Vector2(x * v.x, y * v.y);
}

Vector2 Vector2::operator /(float f) const
{
	return Vector2(x / f, y / f);
}

Vector2 Vector2::operator /(const Vector2 &v) const
{
	return Vector2(x / v.x, y / v.y);
}

void Vector2::operator +=(float f)
{
	x += f;
	y += f;
}

void Vector2::operator +=(const Vector2 &v)
{
	x += v.x;
	y += v.y;
}

void Vector2::operator -=(float f)
{
	x -= f;
	y -= f;
}

void Vector2::operator -=(const Vector2 &v)
{
	x -= v.x;
	y -= v.y;
}

void Vector2::operator *=(float f)
{
	x *= f;
	y *= f;
}

void Vector2::operator *=(const Vector2 &v)
{
	x *= v.x;
	y *= v.y;
}

void Vector2::operator /=(float f)
{
	x /= f;
	y /= f;
}

void Vector2::operator /=(const Vector2 &v)
{
	x /= v.x;
	y /= v.y;
}

bool Vector2::operator ==(float f) const
{
	return x == f && y == f;
}

bool Vector2::operator ==(const Vector2 &v) const
{
	return x == v.x && y == v.y;
}

bool Vector2::operator !=(float f) const
{
	return x != f || y != f;
}

bool Vector2::operator !=(const Vector2 &v) const
{
	return x != v.x || y != v.y;
}

bool Vector2::operator >=(const Vector2 &v) const
{
	return x >= v.x && y >= v.y;
}

bool Vector2::operator <=(const Vector2 &v) const
{
	return x <= v.x && y <= v.y;
}

bool Vector2::operator >(const Vector2 &v) const
{
	return x > v.x && y > v.y;
}

bool Vector2::operator <(const Vector2 &v) const
{
	return x < v.x && y < v.y;
}

Vector2::operator float *()
{
	return (float *)this;
}

Vector2::operator float *() const
{
	return (float *)this;
}
