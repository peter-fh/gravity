#include <thread>
#include "window.h"
#include "GLFW/glfw3.h"
#include "shader.h"
#include "config.h"
#include "simulation.h"

#define BACKGROUND_COLOR 0.0f, 0.0f, 0.0f, 0.0f

const int MAX_SPECIES = 16;
bool key_pressed = false;


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		key_pressed = true;
	}
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
		key_pressed = true;
		glfwSetWindowShouldClose(window, true);
	}


}



int main(int argc, char* argv[]) {
 

	const int height = 800; // 784
	const int width = 800; // 1024
	const int grid_height = height;
	const int grid_width = width;
	const double target_fps = 60;
	const float point_size = float(width) / float(grid_width) * 2;



	GLFWwindow* window = init_window(width, height, "Game of Life");
	Shader shader("vertex.glsl", "fragment.glsl");

#ifdef __APPLE__
	glViewport(0, 0, width * 2, height * 2);
#else
	glViewport(0, 0, WIDTH, HEIGHT);
#endif

	glfwSwapInterval(1);

	glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);
	glPointSize(point_size);

	glfwSetKeyCallback(window, keyCallback);

	const double target_frame_time = 1.0 / target_fps;
	double average_frame_time = 0;
	const double alpha = 0.99;
	int frames_passed = 0;
	double total_frame_time = 0;

	Simulation sim;

	// Horizontal collision
	/*
	sim.addCircle(Circle(0.1, Vector2(-0.5,0), Vector2(1, 0)));
	sim.addCircle(Circle(0.1, Vector2(0.5,0), Vector2(-1, 0)));
	*/

	// One planet multiple small objects
	sim.addCircle(Circle(0.1, Vector2(0,0)));
	//sim.addCircle(Circle(0.1, Vector2(0.5,0), Vector2(-1, 0)));
	sim.addCircle(Circle(0.001, Vector2(-0.5,-0.5), Vector2(0.5,0.5)));
	sim.addCircle(Circle(0.001, Vector2(0.5,0.5), Vector2(0.5,0.5)));
	sim.addCircle(Circle(0.001, Vector2(-0.5,0.5), Vector2(0.5,0.5)));
	sim.addCircle(Circle(0.001, Vector2(0.5,-0.5), Vector2(0.5,0.5)));
	sim.addCircle(Circle(0.001, Vector2(0.55,-0.5), Vector2(0.5,0.5)));
	sim.addCircle(Circle(0.001, Vector2(0.6,-0.5), Vector2(0.5,0.5)));
	sim.addCircle(Circle(0.001, Vector2(0.65,-0.5), Vector2(0.5,0.5)));
	sim.addCircle(Circle(0.001, Vector2(0.7,-0.5), Vector2(0.5,0.5)));

	while (!glfwWindowShouldClose(window)) {

		double last_frame_time = glfwGetTime();

		glClearColor(BACKGROUND_COLOR);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();

		sim.step();

		glfwSwapBuffers(window);

		glfwPollEvents();
		double current_frame_time = glfwGetTime() - last_frame_time;

		if (frames_passed == 0) {
			average_frame_time = 0.01;
		} else { 
			total_frame_time += current_frame_time;
			average_frame_time = alpha * average_frame_time + (1 - alpha) * current_frame_time;
			int fps = average_frame_time < target_frame_time ? target_fps : (1.0 / average_frame_time);

			std::cout << " FT: " << std::round(average_frame_time * 1000) << "ms " << "(" << fps << "fps) \r";
			std::cout << std::flush;

		}
		double frame_time_remaining = target_frame_time - current_frame_time;
		if (frame_time_remaining > 0) {
			std::this_thread::sleep_for(std::chrono::duration<double>(frame_time_remaining));
		}
		frames_passed++;

#ifdef DEBUG_MODE
		while (!key_pressed) {
			glfwWaitEvents();
		}
		key_pressed = false;
#endif
	}

	std::cout << "\n";
	glfwDestroyWindow(window);
	glfwTerminate();

	return 1;
}
