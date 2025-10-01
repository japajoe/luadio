#include "imgui_ex.h"
#include "../../system/image_utility.hpp"
#include "../../core/texture_2d.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>

namespace ImGuiEx
{
	static luadio::texture_2d dotMatrixTexture;
	
	static void create_dot_matrix_texture()
	{
		if(dotMatrixTexture.get_id() > 0)
			return;

		auto pDotMatrixImage = luadio::image_utility::create_dot_matrix(128, 64, 1, 2, 255, 255, 255, 64);

		dotMatrixTexture.generate(pDotMatrixImage);

		delete pDotMatrixImage;
	}

	bool Knob(const char *label, ImKnobInfo knobInfo, const ImVec2 &size, float *value, float min, float max, int snapSteps)
	{
		ImVec2 cursorPosition = ImGui::GetCursorScreenPos();
		ImVec2 center(cursorPosition.x + (size.x * 0.5f), cursorPosition.y + (size.y * 0.5f));
		float lineHeight = ImGui::GetTextLineHeight();

		auto &style = ImGui::GetStyle();
		ImGui::InvisibleButton(label, ImVec2(size.x, size.y));
		bool valueChanged = false;
		bool isActive = ImGui::IsItemActive();
		bool isHovered = ImGui::IsItemHovered();
		bool isDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

		float t = (*value - min) / (max - min);
		float gamma = (float)M_PI / 4.0f;
		float alpha = ((float)M_PI - gamma) * t * 2.0f + gamma;

		if(isActive && isDragging)
		{
			auto &io = ImGui::GetIO();
			ImVec2 mousePosition = io.MousePos;
			alpha = (float)std::atan2(mousePosition.x - center.x, center.y - mousePosition.y) + (float)M_PI;
			alpha = std::max(gamma, std::min(2.0f * (float)M_PI - gamma, alpha));
			float val = 0.5f * (alpha - gamma) / ((float)M_PI - gamma);

			if(snapSteps > 0)
			{
				if(snapSteps > knobInfo.numberOfSprites)
					snapSteps = knobInfo.numberOfSprites;

				float stepSize = (max - min) / snapSteps;
				float snappedValue = min + (float)std::round(val * snapSteps) * stepSize;
				*value = std::clamp(snappedValue, min, max);
			}
			else
			{
				*value = val * (max - min) + min;
			}

			valueChanged = true;
		}

		auto getUVCoordinates = [] (float percentage, int numSprites, int rows, int columns, ImVec2 &uv0, ImVec2 &uv1) {
			uv0 = ImVec2(0, 0);
			uv1 = ImVec2(0, 0);
			
			int index = (int)std::floor(percentage * (numSprites-1));

			float spriteWidth = 1.0f / columns;
			float spriteHeight = 1.0f / rows;

			int colIndex = index % columns;
			int rowIndex = index / columns;

			uv0.x = colIndex * spriteWidth;
			uv0.y = rowIndex * spriteHeight;

			uv1.x = uv0[0] + spriteWidth;
			uv1.y = uv0[1] + spriteHeight;
		};

		ImVec2 uv0, uv1;
		getUVCoordinates(t, knobInfo.numberOfSprites -1, knobInfo.rows, knobInfo.columns, uv0, uv1);

		ImVec2 pMin = cursorPosition;
		ImVec2 pMax = ImVec2(pMin.x + size.x, pMin.y + size.y);
		auto drawList = ImGui::GetWindowDrawList();
		drawList->AddImage(knobInfo.textureId, pMin, pMax, uv0, uv1);

		if (isActive || isHovered)
		{
			ImGui::SetNextWindowPos(ImVec2(cursorPosition.x - style.WindowPadding.x, cursorPosition.y - lineHeight - style.ItemInnerSpacing.y - style.WindowPadding.y));
			ImGui::BeginTooltip();
			ImGui::Text("%f", *value);
			ImGui::EndTooltip();
		}

		return valueChanged;
	}

	bool DrawWaveform(const float *samples, int frameCount, int channels, const ImVec2 &size, const ImVec4 &foregroundColor, const ImVec4 &backgroundColor)
	{
		if (samples == nullptr || frameCount <= 0 || channels <= 0 || size.x <= 0 || size.y <= 0)
			return false;

		// Calculate the height for each channel
		float channelHeight = size.y / channels;

		// Get the current window's draw list
		auto drawList = ImGui::GetWindowDrawList();
		ImVec2 windowPos = ImGui::GetCursorScreenPos(); // Get the current cursor position in the window

		ImVec2 topLeft = windowPos;
		ImVec2 bottomRight = ImVec2(windowPos.x + size.x, windowPos.y + size.y);
		//drawList->AddRectFilled(topLeft, bottomRight, ImGui::GetColorU32(backgroundColor));

		create_dot_matrix_texture();

		const ImVec2 uv_min(0, 0);
		const ImVec2 uv_max(1, 1);
		const ImU32 col = ImGui::GetColorU32(backgroundColor);

		drawList->AddImage(ImTextureID(dotMatrixTexture.get_id()), topLeft, bottomRight, uv_min, uv_max, col);

		// Loop through each channel
		for (int channel = 0; channel < channels; channel++)
		{
			// Calculate the scaling factor for the waveform
			float xScale = size.x / frameCount; // Scale for the x-axis
			float yScale = channelHeight / 2; // Scale for the y-axis

			// Determine the number of samples to draw based on the width of the drawing area
			int samplesToDraw = (int)(size.x / xScale);
			int step = frameCount / samplesToDraw; // Step size to sample the waveform

			// Draw the waveform points
			for (int i = 0; i < samplesToDraw - 1; i++)
			{
				// Get the sample value for the current channel
				int sampleIndex = i * step * channels + channel; // Interleaved access
				float sampleValue = samples[sampleIndex];
				float nextSampleValue = samples[sampleIndex + step * channels];

				// Calculate the positions for the current and next sample
				ImVec2 start(windowPos.x + i * xScale, (1 - sampleValue) * yScale + (channel * channelHeight + channelHeight / 2));
				ImVec2 end(windowPos.x + (i + 1) * xScale, (1 - nextSampleValue) * yScale + (channel * channelHeight + channelHeight / 2));

				start.y += windowPos.y * 0.33f;
				end.y += windowPos.y * 0.33f;

				// Draw the line between the two sample points
				drawList->AddLine(start, end, ImGui::GetColorU32(foregroundColor));
			}
		}

		// Mouse hit test
		ImVec2 mouse = ImGui::GetIO().MousePos;
		bool hovered = mouse.x >= topLeft.x && mouse.x < bottomRight.x &&
					mouse.y >= topLeft.y && mouse.y < bottomRight.y;
		bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		return clicked;
	}

	bool DrawHistogram(const std::complex<double> *samples, int count, const ImVec2 &size, const ImVec4 &foregroundColor, const ImVec4 &backgroundColor)
	{
		if (samples == nullptr || count <= 0 || size.x <= 0 || size.y <= 0)
			return false;

		// Calculate the height for each channel
		float channelHeight = size.y;

		// Get the current window's draw list
		auto drawList = ImGui::GetWindowDrawList();
		ImVec2 windowPos = ImGui::GetCursorScreenPos(); // Get the current cursor position in the window

		ImVec2 topLeft = windowPos;
		ImVec2 bottomRight = ImVec2(windowPos.x + size.x, windowPos.y + size.y);
		//drawList->AddRectFilled(topLeft, bottomRight, ImGui::GetColorU32(backgroundColor));

		create_dot_matrix_texture();

		const ImVec2 uv_min(0, 0);
		const ImVec2 uv_max(1, 1);
		const ImU32 col = ImGui::GetColorU32(backgroundColor);

		drawList->AddImage(ImTextureID(dotMatrixTexture.get_id()), topLeft, bottomRight, uv_min, uv_max, col);

		int numBins = count / 2; // Display only the first half

		// Find maximum magnitude to normalize the bars
		double maxMag = 1e-10;
		for (int i = 0; i < numBins; ++i) 
		{
			double mag = std::abs(samples[i]);
			if (mag > maxMag) 
				maxMag = mag;
		}

		// Draw histogram bars (first half only)
		float binWidth = size.x / float(numBins);
		for (int i = 0; i < numBins; ++i) 
		{
			double mag = std::abs(samples[i]);
			float normHeight = static_cast<float>(mag / maxMag); // 0..1

			float x0 = windowPos.x + i * binWidth;
			float x1 = x0 + binWidth;
			float y1 = windowPos.y + size.y;
			float y0 = y1 - normHeight * channelHeight;

			// Draw the bar
			drawList->AddRectFilled(
				ImVec2(x0, y0),
				ImVec2(x1, y1),
				ImGui::GetColorU32(foregroundColor)
			);
		}

		// Mouse hit test
		ImVec2 mouse = ImGui::GetIO().MousePos;
		bool hovered = mouse.x >= topLeft.x && mouse.x < bottomRight.x &&
					mouse.y >= topLeft.y && mouse.y < bottomRight.y;
		bool clicked = hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);
		return clicked;
	}

	static const char ColorMarkerStart = '{';
	static const char ColorMarkerEnd = '}';

	static bool ProcessInlineHexColor(const std::string &text, ImVec4 &color)
	{
		int hexCount = text.size();
		if (hexCount == 6 || hexCount == 8)
		{
			std::string hex = text;

			uint32_t hexColor = 0;
            std::stringstream ss;
            ss << std::hex << text;
            ss >> hexColor;

			if (!ss.fail())
			{
				color.x = (float)((hexColor & 0x00FF0000) >> 16) / 255.0f;
				color.y = (float)((hexColor & 0x0000FF00) >> 8) / 255.0f;
				color.z = (float)(hexColor & 0x000000FF) / 255.0f;
				color.w = 1.0f;

				if (hexCount == 8)
				{
					color.w = (float)((hexColor & 0xFF000000) >> 24) / 255.0f;
				}

				return true;
			}
		}

		return false;
	}

	void TextWithColors(const std::string &str)
	{
		std::string tempStr = str;

		bool pushedColorStyle = false;
		int textStart = 0;
		int textCur = 0;

		while (textCur < tempStr.size())
		{
			if (tempStr[textCur] == ColorMarkerStart)
			{
				// Print accumulated text
				if (textCur != textStart)
				{
					int length = textCur - textStart;
					ImGui::TextUnformatted(tempStr.c_str() + textStart, tempStr.c_str() + textStart + length);
					ImGui::SameLine(0.0f, 0.0f);
				}

				// Process color code
				int colorStart = textCur + 1;
				do
				{
					++textCur;
				}
				while (textCur < tempStr.size() && tempStr[textCur] != ColorMarkerEnd);

				// Change color
				if (pushedColorStyle)
				{
					ImGui::PopStyleColor();
					pushedColorStyle = false;
				}

				ImVec4 textColor;
				if (ProcessInlineHexColor(tempStr.substr(colorStart, textCur - colorStart), textColor))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, textColor);
					pushedColorStyle = true;
				}

				textStart = textCur + 1;
			}
			else if (tempStr[textCur] == '\n')
			{
				// Print accumulated text and go to the next line
				int length = textCur - textStart;
				ImGui::TextUnformatted(tempStr.c_str() + textStart, tempStr.c_str() + textStart + length);
				textStart = textCur + 1;
			}

			++textCur;
		}

		if (textCur != textStart)
		{
			int length = textCur - textStart;
			ImGui::TextUnformatted(tempStr.c_str() + textStart, tempStr.c_str() + textStart + length);
		}
		else
		{
			ImGui::NewLine();
		}

		if (pushedColorStyle)
		{
			ImGui::PopStyleColor();
		}
	}

	bool Button(const char *text, const ImVec2 &size)
	{
		bool result = false;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));

		if(ImGui::Button(text, size))
		{
			result = true;
		}

		ImGui::PopStyleColor(2);

		return result;
	}
}