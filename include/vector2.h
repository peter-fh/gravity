#include "GLFW/glfw3.h"

struct Vector2{
    GLfloat x, y;
    Vector2(GLfloat x, GLfloat y) : x(x), y(y) {}

};

Vector2 operator+(const Vector2& a, const Vector2& b);

Vector2 operator-(const Vector2& a, const Vector2& b);

Vector2 operator*(const float& k, const Vector2& v);

float operator*(const Vector2& a, const Vector2& b);

Vector2& operator+=(Vector2& a, const Vector2&b);

Vector2& operator-=(Vector2& a, const Vector2&b);
