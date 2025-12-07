#include <cmath>
#include "GL/glew.h"
#include "simulation.h"
#include <iostream>

const float PI = 3.14159f;
const float G = 0.1f;

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
	lastFrameTime = glfwGetTime();
}

void Simulation::addCircle(Circle circle) {
	m_shapes.push_back(circle);
}

float distance(Circle a, Circle b) {
	return std::sqrt(pow(a.pos.x - b.pos.x, 2) + pow(a.pos.y - b.pos.y, 2));
}

void Simulation::step() {
	double currentTime = glfwGetTime();
	double timeDelta = currentTime - lastFrameTime;
	lastFrameTime = currentTime;

	int vertex_count = 0;
	for (Circle& shape : m_shapes) {
		vertex_count += shape.grain;
	}

	std::vector<Vertex> vertices;
	vertices.reserve(vertex_count);
	for (Circle& shape : m_shapes) {
		std::vector<Vertex> shape_vertices = shape.draw(RGBA(255,255,255,255));
		vertices.insert(vertices.begin(), shape_vertices.begin(), shape_vertices.end());
		float dx = shape.velocity.x * timeDelta;
		float dy = shape.velocity.y * timeDelta;
		shape.pos.x += dx;
		shape.pos.y += dy;
		if (shape.pos.x + shape.r > 1 || shape.pos.x - shape.r < -1) {
			shape.velocity.x *= -1;
			if (dx > 0) {
				shape.pos.x = 1-shape.r;
			} else {
				shape.pos.x = -1+shape.r;
			}
		}
		if (shape.pos.y + shape.r > 1 || shape.pos.y - shape.r < -1) {
			shape.velocity.y *= -1;
			if (dy > 0) {
				shape.pos.y = 1-shape.r;
			} else {
				shape.pos.y = -1+shape.r;
			}
		}
		for (Circle& other_shape : m_shapes)  {
			if (&shape != &other_shape) {
				float x_dist = other_shape.pos.x - shape.pos.x;
				float y_dist = other_shape.pos.y - shape.pos.y;
				float dist = sqrt(pow(x_dist, 2) + pow(y_dist, 2));
				if (dist < shape.r + other_shape.r) {
					shape.pos.x -= dx;
					shape.pos.y -= dy;
					shape.velocity.x *= -1;
					shape.velocity.y *= -1;
					other_shape.velocity.x *= -1;
					other_shape.velocity.y *= -1;
				}

				float gravity_force = G / pow(dist, 2.0);
				float gravity_force_x = gravity_force * x_dist / dist;
				float gravity_force_y = gravity_force * y_dist / dist;
				shape.velocity.x += gravity_force_x;
				shape.velocity.y += gravity_force_y;
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_POINTS, 0, vertex_count);
}

