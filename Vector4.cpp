#include "Vector4.h"
#include "Utils.h"
#include <cmath>

const Vector4 Vector4::Zero = Vector4(0.0f);
const Vector4 Vector4::One = Vector4(1.0f);

Vector4::Vector4()
{
}

Vector4::Vector4(float f)
{
	x = f;
	y = f;
	z = f;
}

Vector4::Vector4(float fx, float fy, float fz)
{
	x = fx;
	y = fy;
	z = fz;
}

Vector4::Vector4(float fx, float fy, float fz, float fw)
{
	x = fx;
	y = fy;
	z = fz;
	w = fw;
}

Vector4 Vector4::cross(const Vector4 &v) const
{
	return Vector4(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

float Vector4::dot(const Vector4 &v) const
{
	return x * v.x + y * v.y + z * v.z;
}

float Vector4::length() const
{
	return std::sqrt(x * x + y * y + z * z);
}

float Vector4::lengthSquared() const
{
	return x * x + y * y + z * z;
}

float Vector4::normalize()
{
	float len = length();

	if (len <= 0.0000000001f)
	{
		x = 0.0f;
		y = 1.0f;
		z = 0.0f;
	}
	else
	{
		x /= len;
		y /= len;
		z /= len;
	}

	return len;
}

Vector4 Vector4::normalized()
{
	Vector4 vec = *this;
	
	vec.normalize();
	return vec;
}

Vector2 Vector4::vec2() const
{
	return Vector2(x, y);
}

Vector4 Vector4::operator +(float f) const
{
	return Vector4(x + f, y + f, z + f);
}

Vector4 Vector4::operator +(const Vector4 &v) const
{
	return Vector4(x + v.x, y + v.y, z + v.z);
}

Vector4 Vector4::operator -() const
{
	return Vector4(-x, -y, -z);
}

Vector4 Vector4::operator -(float f) const
{
	return Vector4(x - f, y - f, z - f);
}

Vector4 Vector4::operator -(const Vector4 &v) const
{
	return Vector4(x - v.x, y - v.y, z - v.z);
}

Vector4 Vector4::operator *(float f) const
{
	return Vector4(x * f, y * f, z * f);
}

Vector4 Vector4::operator *(const Vector4 &v) const
{
	return Vector4(x * v.x, y * v.y, z * v.z);
}

Vector4 Vector4::operator /(float f) const
{
	return Vector4(x / f, y / f, z / f);
}

Vector4 Vector4::operator /(const Vector4 &v) const
{
	return Vector4(x / v.x, y / v.y, z / v.z);
}

void Vector4::operator +=(float f)
{
	x += f;
	y += f;
	z += f;
}

void Vector4::operator +=(const Vector4 &v)
{
	x += v.x;
	y += v.y;
	z += v.z;
}

void Vector4::operator -=(float f)
{
	x -= f;
	y -= f;
	z -= f;
}

void Vector4::operator -=(const Vector4 &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

void Vector4::operator *=(float f)
{
	x *= f;
	y *= f;
	z *= f;
}

void Vector4::operator *=(const Vector4 &v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

void Vector4::operator /=(float f)
{
	x /= f;
	y /= f;
	z /= f;
}

void Vector4::operator /=(const Vector4 &v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
}

bool Vector4::operator ==(float f) const
{
	return x == f && y == f && z == f;
}

bool Vector4::operator ==(const Vector4 &v) const
{
	return x == v.x && y == v.y && z == v.z;
}

bool Vector4::operator !=(float f) const
{
	return x != f || y != f || z != f;
}

bool Vector4::operator !=(const Vector4 &v) const
{
	return x != v.x || y != v.y || z != v.z;
}

bool Vector4::operator >=(const Vector4 &v) const
{
	return x >= v.x && y >= v.y && z >= v.z;
}

bool Vector4::operator <=(const Vector4 &v) const
{
	return x <= v.x && y <= v.y && z <= v.z;
}

bool Vector4::operator >(const Vector4 &v) const
{
	return x > v.x && y > v.y && z > v.z;
}

bool Vector4::operator <(const Vector4 &v) const
{
	return x < v.x && y < v.y && z < v.z;
}

float &Vector4::operator [](int index)
{
	return ((float *)this)[index];
}

const float &Vector4::operator [](int index) const
{
	return ((float *)this)[index];
}

Vector4::operator Vector2() const
{
	return vec2();
}
