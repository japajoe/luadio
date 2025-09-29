#include "imgui_manager.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../../libs/glfw/include/GLFW/glfw3.h"

namespace luadio
{
    imgui_manager::imgui_manager()
    {
        this->window = nullptr;
    }

    void imgui_manager::initialize(GLFWwindow *window)
    {
        this->window = window;

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
    #ifdef __EMSCRIPTEN__
        ImGui_ImplOpenGL3_Init("#version 300 es");
    #else
        ImGui_ImplOpenGL3_Init("#version 150");
    #endif
        
        io.Fonts->AddFontDefault();        
    }

    void imgui_manager::destroy()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void imgui_manager::begin_frame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void imgui_manager::end_frame()
    {
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void imgui_manager::set_style()
    {
        auto &colors = ImGui::GetStyle().Colors;
        auto bg = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
        auto bgHovered = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);
        auto bgActive = ImVec4(0.23f, 0.26f, 0.29f, 1.00f);
        auto frameBg = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
        auto menuBg = ImVec4(0.10f, 0.11f, 0.11f, 1.00f);
        auto text = ImVec4(0.86f, 0.87f, 0.88f, 1.00f);
        auto grab = ImVec4(0.17f, 0.18f, 0.19f, 1.00f);
        auto checkMark = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

        colors[ImGuiCol_Text] = text;
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_WindowBg] = bg;
        colors[ImGuiCol_ChildBg] = bg;
        colors[ImGuiCol_PopupBg] = frameBg;
        colors[ImGuiCol_Border] = bg;
        colors[ImGuiCol_BorderShadow] = ImVec4(0.14f, 0.16f, 0.18f, 1.00f);
        colors[ImGuiCol_FrameBg] = frameBg;
        colors[ImGuiCol_FrameBgHovered] = bgHovered;
        colors[ImGuiCol_FrameBgActive] = bgActive;
        colors[ImGuiCol_TitleBg] = bg;
        colors[ImGuiCol_TitleBgActive] = bg;
        colors[ImGuiCol_TitleBgCollapsed] = bg;
        colors[ImGuiCol_MenuBarBg] = menuBg;
        colors[ImGuiCol_ScrollbarBg] = bg;
        colors[ImGuiCol_ScrollbarGrab] = grab;
        colors[ImGuiCol_ScrollbarGrabHovered] = bgHovered;
        colors[ImGuiCol_ScrollbarGrabActive] = bgActive;
        colors[ImGuiCol_CheckMark] = checkMark;
        colors[ImGuiCol_SliderGrab] = bgHovered;
        colors[ImGuiCol_SliderGrabActive] = checkMark;
        colors[ImGuiCol_Button] = bg;
        colors[ImGuiCol_ButtonHovered] = bgHovered;
        colors[ImGuiCol_ButtonActive] = bg;
        colors[ImGuiCol_Header] = grab;
        colors[ImGuiCol_HeaderHovered] = bgHovered;
        colors[ImGuiCol_HeaderActive] = bgActive;
        colors[ImGuiCol_Separator] = bg;
        colors[ImGuiCol_SeparatorHovered] = bgHovered;
        colors[ImGuiCol_SeparatorActive] = bgActive;
        colors[ImGuiCol_ResizeGrip] = grab;
        colors[ImGuiCol_ResizeGripHovered] = bgHovered;
        colors[ImGuiCol_ResizeGripActive] = bgActive;
        colors[ImGuiCol_TabHovered] = menuBg;
        colors[ImGuiCol_Tab] = menuBg;
        colors[ImGuiCol_TabSelected] = menuBg;
        colors[ImGuiCol_TabSelectedOverline] = menuBg;
        colors[ImGuiCol_TabDimmed] = menuBg;
        colors[ImGuiCol_TabDimmedSelected] = bg;
        colors[ImGuiCol_TabDimmedSelectedOverline] = menuBg;
        colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines] = text;
        colors[ImGuiCol_PlotLinesHovered] = bgActive;
        colors[ImGuiCol_PlotHistogram] = text;
        colors[ImGuiCol_PlotHistogramHovered] = bgActive;
        colors[ImGuiCol_TableHeaderBg] = menuBg;
        colors[ImGuiCol_TableBorderStrong] = menuBg;
        colors[ImGuiCol_TableBorderLight] = menuBg;
        colors[ImGuiCol_TableRowBg] = bg;
        colors[ImGuiCol_TableRowBgAlt] = menuBg;
        colors[ImGuiCol_TextLink] = checkMark;
        colors[ImGuiCol_TextSelectedBg] = bgActive;
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = checkMark;
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.10f, 0.11f, 0.50f);

        auto &style = ImGui::GetStyle();
        style.FrameBorderSize = 0.0f;
        style.FrameRounding = 2.0f;
        style.WindowBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.ScrollbarSize = 12.0f;
        style.ScrollbarRounding = 2.0f;
        style.GrabMinSize = 7.0f;
        style.GrabRounding = 2.0f;
        style.TabRounding = 2.0f;

        style.WindowPadding = ImVec2(5.0f, 5.0f);
        style.FramePadding = ImVec2(4.0f, 3.0f);
        style.ItemSpacing = ImVec2(6.0f, 4.0f);
        style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
        style.TabBarBorderSize = 0;
        style.WindowBorderSize = 0;
    }

    std::string imgui_manager::get_settings()
    {
        static const std::string settings = R"([Window][WindowOverViewport_11111111]
Pos=0,0
Size=906,600
Collapsed=0

[Window][Debug##Default]
ViewportPos=11,117
ViewportId=0x16723995
Size=400,400
Collapsed=0

[Window][Test]
Pos=16,12
Size=308,218
Collapsed=0

[Window][Viewport]
Pos=319,49
Size=314,455
Collapsed=0
DockId=0x00000004,0

[Window][Code]
Pos=0,49
Size=317,455
Collapsed=0
DockId=0x00000003,0

[Window][Log]
Pos=0,506
Size=633,94
Collapsed=0
DockId=0x00000006,0

[Window][Inspector]
Pos=635,49
Size=271,551
Collapsed=0
DockId=0x00000002,0

[Window][Panel]
Pos=0,0
Size=906,47
Collapsed=0
DockId=0x00000007,0

[Docking][Data]
DockSpace         ID=0x08BD597D Window=0x1BBC0F80 Pos=29,57 Size=906,600 Split=Y
  DockNode        ID=0x00000007 Parent=0x08BD597D SizeRef=1366,47 HiddenTabBar=1 Selected=0x323278BB
  DockNode        ID=0x00000008 Parent=0x08BD597D SizeRef=1366,652 Split=X
    DockNode      ID=0x00000001 Parent=0x00000008 SizeRef=878,600 Split=Y
      DockNode    ID=0x00000005 Parent=0x00000001 SizeRef=878,455 Split=X Selected=0x440BB4FB
        DockNode  ID=0x00000003 Parent=0x00000005 SizeRef=264,517 HiddenTabBar=1 Selected=0x440BB4FB
        DockNode  ID=0x00000004 Parent=0x00000005 SizeRef=261,517 HiddenTabBar=1 Selected=0xC450F867
      DockNode    ID=0x00000006 Parent=0x00000001 SizeRef=878,94 CentralNode=1 HiddenTabBar=1 Selected=0x139FDA3F
    DockNode      ID=0x00000002 Parent=0x00000008 SizeRef=271,600 HiddenTabBar=1 Selected=0x36DC96AB

)";
        return settings;
    }
}