#pragma once

#include "Vector2.h"

class Vector4
{
public:
	static const Vector4 Zero;
	static const Vector4 One;

public:
	float x;
	float y;
	float z;
	float w;

public:
	Vector4();
	Vector4(float f);
	Vector4(float fx, float fy, float fz);
	Vector4(float fx, float fy, float fz, float fw);
	Vector4 cross(const Vector4 &v) const;
	float dot(const Vector4 &v) const;
	float length() const;
	float lengthSquared() const;
	float normalize();
	Vector4 normalized();
	Vector2 vec2() const;
	Vector4 operator +(float f) const;
	Vector4 operator +(const Vector4 &v) const;
	Vector4 operator -() const;
	Vector4 operator -(float f) const;
	Vector4 operator -(const Vector4 &v) const;
	Vector4 operator *(float f) const;
	Vector4 operator *(const Vector4 &v) const;
	Vector4 operator /(float f) const;
	Vector4 operator /(const Vector4 &v) const;
	void operator +=(float f);
	void operator +=(const Vector4 &v);
	void operator -=(float f);
	void operator -=(const Vector4 &v);
	void operator *=(float f);
	void operator *=(const Vector4 &v);
	void operator /=(float f);
	void operator /=(const Vector4 &v);
	bool operator ==(float f) const;
	bool operator ==(const Vector4 &v) const;
	bool operator !=(float f) const;
	bool operator !=(const Vector4 &v) const;
	bool operator >=(const Vector4 &v) const;
	bool operator <=(const Vector4 &v) const;
	bool operator >(const Vector4 &v) const;
	bool operator <(const Vector4 &v) const;
	float &operator [](int index);
	const float &operator [](int index) const;
	operator Vector2() const;
};
