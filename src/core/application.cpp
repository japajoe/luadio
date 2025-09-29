#include "application.hpp"
#include "../external/glad/glad.h"
#include "../external/imgui/imgui_manager.hpp"
#include "../../libs/glfw/include/GLFW/glfw3.h"
#include <iostream>
#include <cstdint>
#include <string>

namespace luadio
{
	struct configuration
	{
		GLFWwindow *pWindow;
		application *pApplication;
		uint32_t width;
		uint32_t height;
		double deltaTime;
		double lastTime;
		std::string title;
	};

	static configuration gConfig = {0};
	static imgui_manager gImGui;

	static GLFWwindow *create_window(uint32_t width, uint32_t height, const char *title, GLFWwindow *shared)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);

		return glfwCreateWindow(width, height, title, nullptr, shared);
	}

	application::application()
	{
		gConfig.pApplication = this;
		gConfig.title = "Luadio";
	}

	application::~application()
	{
		gConfig.pApplication = nullptr;
	}

	bool application::run()
	{
		if(gConfig.pWindow)
		{
			std::cerr << "Window is already created\n";
			return false;
		}

		gConfig.pWindow = nullptr;
		gConfig.width = 800;
		gConfig.height = 600;
		gConfig.deltaTime = 0.0;
		gConfig.lastTime = 0.0;

		if (!glfwInit()) 
		{
			std::cerr << "GLFW could not initialize\n";
			return false;
		}

		gConfig.pWindow = create_window(gConfig.width, gConfig.height, gConfig.title.c_str(), nullptr);
		
		if (!gConfig.pWindow) 
		{
			std::cerr << "Window could not be created\n";
			glfwDestroyWindow(gConfig.pWindow);
			glfwTerminate();
			return false;
		}

		glfwSetFramebufferSizeCallback(gConfig.pWindow, window_resize_callback);
		glfwSetKeyCallback(gConfig.pWindow, key_press_callback);
		glfwSetCharCallback(gConfig.pWindow, char_press_callback);
		glfwSetMouseButtonCallback(gConfig.pWindow, mouse_button_press_callback);
		glfwSetScrollCallback(gConfig.pWindow, scroll_callback);

		glfwMakeContextCurrent(gConfig.pWindow);

		if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			glfwTerminate();
			std::cerr << "Failed to initialize OpenGL\n";
			return false;
		}
		
		gImGui.initialize(gConfig.pWindow);
		gImGui.set_style();

		if(gConfig.pApplication)
		{
			gConfig.pApplication->on_load();
		}

		while(!glfwWindowShouldClose(gConfig.pWindow))
		{
			pump_events();
		}

		if(gConfig.pApplication)
		{
			gConfig.pApplication->on_destroy();
		}

		gImGui.destroy();

		glfwDestroyWindow(gConfig.pWindow);
		glfwTerminate();
		
		gConfig.pWindow = nullptr;
		gConfig.deltaTime = 0.0;
		gConfig.lastTime = 0.0;

		return true;
	}

	void application::close()
	{
		quit();
	}

	void application::quit()
	{
		if(glfwWindowShouldClose(gConfig.pWindow))
			return;
		
		glfwSetWindowShouldClose(gConfig.pWindow, GLFW_TRUE);
	}

	void application::set_title(const char *title)
	{
		gConfig.title = title;
		
		if(gConfig.pWindow)
			glfwSetWindowTitle(gConfig.pWindow, gConfig.title.c_str());
	}

	void application::pump_events()
	{
		double currentTime = glfwGetTime();
		gConfig.deltaTime = currentTime - gConfig.lastTime;
		gConfig.lastTime = currentTime;

		if(gConfig.pApplication)
		{
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			gImGui.begin_frame();
			gConfig.pApplication->on_update();
			gConfig.pApplication->on_late_update();
			gConfig.pApplication->on_gui();
			gImGui.end_frame();
		}
		
		glfwSwapBuffers(gConfig.pWindow);
		glfwPollEvents();
	}
	
	GLFWwindow *application::get_native_window()
	{
		return gConfig.pWindow;
	}

	void application::on_load() {}

	void application::on_destroy() {}

	void application::on_update() {}

	void application::on_late_update() {}

	void application::on_gui() {}

	void application::window_resize_callback(GLFWwindow *window, int32_t width, int32_t height)
	{
		gConfig.width = width;
		gConfig.height = height;
		glViewport(0, 0, width, height);
	}

	void application::key_press_callback(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
	{
	}

	void application::char_press_callback(GLFWwindow *window, uint32_t codepoint)
	{
	}

	void application::mouse_button_press_callback(GLFWwindow *window, int32_t button, int32_t action, int32_t mods)
	{
	}

	void application::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
	{
	}
}