#ifndef LUADIO_APPLICATION_HPP
#define LUADIO_APPLICATION_HPP

#include <cstdint>

struct GLFWwindow;

namespace luadio
{
	class application
	{
	public:
		application();
		virtual ~application();
		virtual void on_load();
		virtual void on_destroy();
		virtual void on_update();
		virtual void on_late_update();
		virtual void on_gui();
		bool run();
		void close();
		void set_title(const char *title);
		static void quit();
		static GLFWwindow *get_native_window();
	private:		
		static void pump_events();
		static void window_resize_callback(GLFWwindow *window, int32_t width, int32_t height);
		static void key_press_callback(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
		static void char_press_callback(GLFWwindow *window, uint32_t codepoint);
		static void mouse_button_press_callback(GLFWwindow *window, int32_t button, int32_t action, int32_t mods);
		static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
	};
}

#endif