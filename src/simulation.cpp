#include <cmath>
#include "GL/glew.h"
#include "simulation.h"
#include <iostream>

const float PI = 3.14159f;
const float G = 0.1f;
const float COLLISION_DAMPING = 0.9f;
const float COLLISION_OFFSET = 0.01f;

Circle::Circle(GLfloat radius, Vector2 position, int grain) :
	r(radius), 
	pos(position),
	velocity(Vector2(0,0)),
	grain(grain) {
	mass = radius * radius * PI;
};

Circle::Circle(GLfloat radius, Vector2 position, Vector2 velocity, int grain) : 
	r(radius),
	pos(position),
	velocity(velocity),
	grain(grain) {
	mass = radius * radius * PI;
};

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

float magnitude(Vector2 v) {
	return std::sqrt(pow(v.x, 2) + pow(v.y, 2));
}

void Simulation::step() {
	double currentTime = glfwGetTime();
	double timeDelta = currentTime - lastFrameTime;
	lastFrameTime = currentTime;

	// Count vertices
	int vertex_count = 0;
	for (Circle& shape : m_shapes) {
		vertex_count += shape.grain;
	}

	// Compute forces
	for (Circle& shape : m_shapes) {
		for (Circle& other_shape : m_shapes) {
			if (&shape != &other_shape) {
				float x_dist = other_shape.pos.x - shape.pos.x;
				float y_dist = other_shape.pos.y - shape.pos.y;
				float dist = sqrt(pow(x_dist, 2) + pow(y_dist, 2));
				float offset = dist - COLLISION_OFFSET - (shape.r + other_shape.r);
				float gravity_force = shape.mass * other_shape.mass * G / pow(dist, 2.0);
				float gravity_force_x = gravity_force * x_dist / dist;
				float gravity_force_y = gravity_force * y_dist / dist;
				shape.velocity.x += gravity_force_x/shape.mass;
				shape.velocity.y += gravity_force_y/shape.mass;
			}
		}
	}

	// Integrate positions
	for (Circle& shape : m_shapes) {
		float dx = shape.velocity.x * timeDelta;
		float dy = shape.velocity.y * timeDelta;
		shape.pos.x += dx;
		shape.pos.y += dy;
	}

	// Handle collisions
	for (Circle& shape : m_shapes) {
		if (shape.pos.x + shape.r > 1) {
			shape.pos.x = 1 - shape.r;
			shape.velocity.x *= -1;
		}
		if (shape.pos.x - shape.r < -1) {
			shape.pos.x = -1 + shape.r;
			shape.velocity.x *= -1;
		}
		if (shape.pos.y + shape.r > 1) {
			shape.pos.y = 1 - shape.r;
			shape.velocity.y *= -1;
		}
		if (shape.pos.y - shape.r < -1) {
			shape.pos.y = -1 + shape.r;
			shape.velocity.y *= -1;
		}
		for (Circle& other_shape : m_shapes) {
			if (&shape != &other_shape) {
				float x_dist = other_shape.pos.x - shape.pos.x;
				float y_dist = other_shape.pos.y - shape.pos.y;
				float dist = sqrt(pow(x_dist, 2) + pow(y_dist, 2));
				if (dist < shape.r + other_shape.r) {
					// Position
					Vector2 u(other_shape.pos.x - shape.pos.x, other_shape.pos.y - shape.pos.y);
					float k = (shape.r + other_shape.r) / magnitude(u);
					Vector2 v(k*u.x, k*u.y);
					Vector2 displacement(v.x-u.x, v.y-u.y);
					float displacement_multiplier = shape.mass / (shape.mass + other_shape.mass);
					other_shape.pos.x += displacement_multiplier * displacement.x * COLLISION_DAMPING;
					other_shape.pos.y += displacement_multiplier * displacement.y * COLLISION_DAMPING;


					float C = 1;
					float friction = 0.5;

					// https://en.wikipedia.org/wiki/Inelastic_collision
					Vector2 n(x_dist/dist, y_dist/dist);
					k = ((shape.mass * other_shape.mass) / (shape.mass + other_shape.mass)) * (1 + C);
					float J = k*(other_shape.velocity - shape.velocity) * n;
					shape.velocity += J/shape.mass * n;
				}
			}
		}
	}

	// Generate vertices
	std::vector<Vertex> vertices;
	vertices.reserve(vertex_count);
	for (Circle& shape : m_shapes) {
		std::vector<Vertex> shape_vertices = shape.draw(RGBA(255,255,255,255));
		vertices.insert(vertices.begin(), shape_vertices.begin(), shape_vertices.end());
		/*
		float dx = shape.velocity.x * timeDelta;
		float dy = shape.velocity.y * timeDelta;
		float x = shape.pos.x + dx;
		float y = shape.pos.y + dy;
		bool x_collision = false;
		bool y_collision = false;

		if (x + shape.r > 1 || x - shape.r < -1) {
			shape.velocity.x *= -1;
			if (dx > 0) {
				shape.pos.x = 1-shape.r;
			} else {
				shape.pos.x = -1+shape.r;
			}
			x_collision = true;
		}

		if (y + shape.r > 1 || y - shape.r < -1) {
			shape.velocity.y *= -1;
			if (dy > 0) {
				shape.pos.y = 1-shape.r;
			} else {
				shape.pos.y = -1+shape.r;
			}
			y_collision = true;
		}

		for (Circle& other_shape : m_shapes)  {
			if (&shape != &other_shape) {
				float x_dist = other_shape.pos.x - x;
				float y_dist = other_shape.pos.y - y;
				float dist = sqrt(pow(x_dist, 2) + pow(y_dist, 2));
				float offset = dist - COLLISION_OFFSET - (shape.r + other_shape.r);


				float x_start_dist = other_shape.pos.x - shape.pos.x;
				float y_start_dist = other_shape.pos.y - shape.pos.y;
				float start_dist = sqrt(pow(x_start_dist, 2.0) + pow(y_start_dist, 2.0));
				float start_offset = start_dist -COLLISION_OFFSET - (shape.r + other_shape.r);
				if (start_offset < 0 && shape.mass < other_shape.mass) {
					x_collision = true;
					y_collision = true;
					Vector2 n(x_start_dist/start_dist, y_start_dist/start_dist);
					shape.pos.x += n.x * start_offset;
					shape.pos.y += n.y * start_offset;
				}

				if (offset < 0) {
					x_collision = true;
					y_collision = true;
					float frame_velocity = sqrt(pow(dx, 2.0) + pow(dy, 2.0));
					float clip_percentage = abs(offset / frame_velocity);
					float non_clip_percentage = 1.0-clip_percentage;
					Vector2 n(x_dist/dist, y_dist/dist);
					Vector2 v = shape.velocity;
					float dot_product = COLLISION_DAMPING * 2 * (n.x * v.x + n.y * v.y);
					//shape.pos.x += dx * non_clip_percentage;
					//shape.pos.y += dy * non_clip_percentage;
					shape.velocity.x -= dot_product*n.x;
					shape.velocity.y -= dot_product*n.y;
					dx = shape.velocity.x * timeDelta;
					dy = shape.velocity.y * timeDelta;
					shape.pos.x += dx * clip_percentage;
					shape.pos.y += dy * clip_percentage;
				}

			}

		}
		if (!x_collision) {
			shape.pos.x += dx;
		}
		if (!y_collision) {
			shape.pos.y += dy;
		}
		*/
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_POINTS, 0, vertex_count);
}

