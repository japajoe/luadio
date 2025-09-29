#ifndef IMGUI_EX_H
#define IMGUI_EX_H

#include "imgui.h"
#include <complex>

struct ImKnobInfo
{
    ImTextureID textureId;
    int numberOfSprites;
    int rows;
    int columns;
};

namespace ImGuiEx
{
	bool Knob(const char *label, ImKnobInfo knobInfo, const ImVec2 &size, float *value, float min, float max, int snapSteps);
    bool DrawWaveform(const float *samples, int frameCount, int channels, const ImVec2 &size, const ImVec4 &foregroundColor, const ImVec4 &backgroundColor);
    bool DrawHistogram(const std::complex<double> *samples, int count, const ImVec2 &size, const ImVec4 &foregroundColor, const ImVec4 &backgroundColor);
}

#endif