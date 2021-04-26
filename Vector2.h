#pragma once

class Vector2
{
public:
	static const Vector2 Zero;
	static const Vector2 One;

public:
	float x;
	float y;

public:
	Vector2();
	Vector2(float f);
	Vector2(float fx, float fy);
	float dot(const Vector2 &v) const;
	float length() const;
	float lengthSquared() const;
	float normalize();
	Vector2 operator +(float f) const;
	Vector2 operator +(const Vector2 &v) const;
	Vector2 operator -() const;
	Vector2 operator -(float f) const;
	Vector2 operator -(const Vector2 &v) const;
	Vector2 operator *(float f) const;
	Vector2 operator *(const Vector2 &v) const;
	Vector2 operator /(float f) const;
	Vector2 operator /(const Vector2 &v) const;
	void operator +=(float f);
	void operator +=(const Vector2 &v);
	void operator -=(float f);
	void operator -=(const Vector2 &v);
	void operator *=(float f);
	void operator *=(const Vector2 &v);
	void operator /=(float f);
	void operator /=(const Vector2 &v);
	bool operator ==(float f) const;
	bool operator ==(const Vector2 &v) const;
	bool operator !=(float f) const;
	bool operator !=(const Vector2 &v) const;
	bool operator >=(const Vector2 &v) const;
	bool operator <=(const Vector2 &v) const;
	bool operator >(const Vector2 &v) const;
	bool operator <(const Vector2 &v) const;
	operator float *();
	operator float *() const;
};
