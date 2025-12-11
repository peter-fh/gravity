#include "GLFW/glfw3.h"
#include <vector>
#include "vector2.h"


struct RGBA {
    GLubyte r,g,b,a;
    RGBA(GLubyte r, GLubyte g, GLubyte b, GLubyte a) 
    : r(r), g(g), b(b), a(a) {}
};

struct Vertex {
    Vector2 pos;
    RGBA color;
    Vertex(Vector2 position, RGBA color) : pos(position), color(color) {}
    Vertex() : pos(Vector2(0,0)), color(RGBA(0,0,0,0)) {}
};


class Circle {
public:
    Circle(GLfloat radius, Vector2 position, int grain=1000);

    Circle(GLfloat radius, Vector2 position, Vector2 velocity, int grain=1000);

    std::vector<Vertex> draw(RGBA color);

    GLfloat r;
    Vector2 pos;
    Vector2 velocity;
    float mass;
    int grain;
};

class Simulation {
public:
    Simulation();
    void step();
    void addCircle(Circle circle);
private:
    double lastFrameTime;
    std::vector<Circle> m_shapes;
    std::vector<Vertex> m_vertices;
    GLuint m_VAO;
    GLuint m_VBO;
};

