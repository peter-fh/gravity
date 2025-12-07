#include <cmath>
#include "GL/glew.h"
#include "simulation.h"
#include <iostream>

const float PI = 3.14159f;

Circle::Circle(GLfloat radius, Vector2 position, int grain) : 
	r(radius), 
	pos(position),
	grain(grain) {}

std::vector<Vertex> Circle::draw(RGBA color){
	std::vector<Vertex> vertices(this->grain);
	float angle = 2 * PI / (float) this->grain;
	for (int i=0; i < this->grain; i++) {
		float theta = angle * i;
		float x = pos.x + (r * cos(theta));
		float y = pos.y + (r * sin(theta));
		Vector2 position(x, y);
		vertices[i] = Vertex(position, color);
	}
	return vertices;
}

Simulation::Simulation(){

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	GLsizei stride = sizeof(Vertex);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);
}

void Simulation::addCircle(Circle circle) {
	m_shapes.push_back(circle);
}

void Simulation::step() {
	int vertex_count = 0;
	for (Circle& shape : m_shapes) {
		vertex_count += shape.grain;
	}
	std::vector<Vertex> vertices;
	vertices.reserve(vertex_count);
	for (Circle& shape : m_shapes) {
		std::vector<Vertex> shape_vertices = shape.draw(RGBA(255,255,255,255));
		vertices.insert(vertices.begin(), shape_vertices.begin(), shape_vertices.end());
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_POINTS, 0, vertex_count);
}

