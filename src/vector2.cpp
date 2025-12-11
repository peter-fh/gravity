#include "vector2.h"

Vector2 operator+(const Vector2& a, const Vector2& b) {
	return Vector2(a.x+b.x, a.y+b.y);
}

Vector2 operator-(const Vector2& a, const Vector2& b) {
	return Vector2(a.x-b.x, a.y-b.y);
}

Vector2 operator*(const float& k, const Vector2& v) {
	return Vector2(k*v.x, k*v.y);
}

float operator*(const Vector2& a, const Vector2& b) {
	return a.x * b.x + a.y * b.y;
}

Vector2& operator+=(Vector2& a, const Vector2&b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}

Vector2& operator-=(Vector2& a, const Vector2&b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

