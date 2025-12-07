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
 

	const int height = 784; // 784
	const int width = 1024; // 1024
#ifdef DEBUG_MODE
	const int grid_height = 49;
	const int grid_width = 64;
#else
	const int grid_height = height;
	const int grid_width = width;
#endif
	const double target_fps = 120;
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
	Circle* circle = new Circle(0.5, Vector2(0,0));
	sim.addShape(circle);

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

#ifdef DEBUG_MODE
			std::cout << "FT: " << std::round(average_frame_time * 1000) << "ms " << "(" << fps << "fps) \r";
#else
			std::cout << "FT: " << std::round(average_frame_time * 1000) << "ms " << "(" << fps << "fps) \r";
#endif
			std::cout << std::flush;

		}
		double frame_time_remaining = target_frame_time - current_frame_time;
		if (frame_time_remaining > 0) {
			std::this_thread::sleep_for(std::chrono::duration<double>(frame_time_remaining));
		}
		frames_passed++;

		if (frames_passed == 460) {
			double fps_450 = 450 / total_frame_time;
			double average_frame_time_450 = total_frame_time / 450.0;
			std::cout << std::flush;
			std::cout << "First 450 frame statistics:   \n";
			std::cout << "\t" << std::round(total_frame_time * 1000) << "ms total\n";
			std::cout << "\t" << std::round(average_frame_time_450 * 10000) / 10 << "ms average frame time\n";
			if (fps_450 > target_fps) {
				std::cout << "\t" << std::round(fps_450) << " average fps (locked at " << target_fps << ")\n";
			} else {
				std::cout <<  "\t" <<std::round(fps_450) << " average fps\n";
			}
		}
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
