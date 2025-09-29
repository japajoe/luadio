#ifndef LUADIO_IMGUIMANAGER_HPP
#define LUADIO_IMGUIMANAGER_HPP

#include <string>

struct GLFWwindow;

namespace luadio
{
    class imgui_manager
    {
    public:
        imgui_manager();
        void initialize(GLFWwindow *window);
        void destroy();
        void begin_frame();
        void end_frame();
        void set_style();
        std::string get_settings();
    private:
        GLFWwindow *window;
    };
}
#endif