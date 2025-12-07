#include "GLFW/glfw3.h"
#include <vector>


struct Vector2{
    GLfloat x, y;
    Vector2(GLfloat x, GLfloat y) : x(x), y(y) {}
};

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

class IShape {
public:
    virtual std::vector<Vertex> draw(RGBA color)=0;
    virtual int vertexCount()=0;
};

class Circle : public IShape {
public:
    Circle(GLfloat radius, Vector2 position, int grain=1000);
    std::vector<Vertex> draw(RGBA color) override;
    int vertexCount() override;

private:
    GLfloat r;
    Vector2 pos;
    int grain;
};

class Simulation {
public:
    Simulation();
    void step();
    void addShape(IShape* shape);
private:
    std::vector<IShape*> m_shapes;
    std::vector<Vertex> m_vertices;
    GLuint m_VAO;
    GLuint m_VBO;
};
