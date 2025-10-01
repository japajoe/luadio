#ifndef LUADIO_LOGBOX_HPP
#define LUADIO_LOGBOX_HPP

#include "imgui.h"
#include "imgui_ex.h"
#include "../../system/ring_buffer.hpp"
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>

namespace luadio
{
    using ImGuiProcessCommandCallback = std::function<bool(const std::string &text)>;

    class imgui_logbox
    {
	public:
        ImGuiProcessCommandCallback processCommand;
        bool autoScroll = true;
        bool scrollToBottom = false;
        bool reclaim_focus = false;

        imgui_logbox()
        {
            ClearLog();

            historyPos = -1;
            commands.push_back("/help");
            commands.push_back("/history");
            commands.push_back("/clear");
            autoScroll = true;
            scrollToBottom = false;

            AddLog("Luadio {FFFF00}1.0.0");
        }

        bool HasFocus () const
        {
			return reclaim_focus == true && showInputField == true;
        }

        void Focus()
        {
            reclaim_focus = true;
            showInputField = true;
        }

        void Unfocus()
        {
            reclaim_focus = false;
            showInputField = false;
        }

        static int Stricmp(const std::string &str1, const std::string &str2)
        { 
			// Convert both strings to uppercase for case-insensitive comparison
			std::string upperStr1 = str1;
			std::string upperStr2 = str2;

			std::transform(upperStr1.begin(), upperStr1.end(), upperStr1.begin(), ::toupper);
			std::transform(upperStr2.begin(), upperStr2.end(), upperStr2.begin(), ::toupper);

			// Compare the uppercase strings
			if (upperStr1 == upperStr2)
				return 0; // Strings are equal

			return 1; // Strings are not equal
        }

        static int Strnicmp(const std::string &str1, const std::string &str2, int n) 
        { 
			int d = 0;
			int index = 0;

			// Compare characters up to n or until one string ends
			while (n > 0 && index < str1.size() && index < str2.size() && d == 0) 
			{
				d = std::toupper(str1[index]) - std::toupper(str2[index]);
				n--;
				index++;
			}

			// If we reached the end of one string, check if they are of different lengths
			if (d == 0 && index < str1.size() && index < str2.size()) 
			{
				return 0; // Both strings are equal up to n characters
			}

			return d; // Return the difference
        }

        static std::string Strtrim(const std::string &str)
        { 
			std::string trimmedStr = str;
			// Remove trailing whitespace
			trimmedStr.erase(std::find_if(trimmedStr.rbegin(), trimmedStr.rend(),
				[](unsigned char ch) { return !std::isspace(ch); }).base(), trimmedStr.end());
			return trimmedStr;
        }

        void ClearLog()
        {
            items.clear();
        }

        void AddLog(const std::string &text)
        {            
            items.add(text);
        }

        void Draw(const std::string &title)
        {
            BeginNoWindowFlags();
            
            if (!ImGui::Begin(title.c_str(), nullptr, noWindowFlags))
            {
                ImGui::End();
                return;
            }
			
			if(ImGuiEx::Button("Clear"))
			{
				ClearLog();
			}

            ImGui::SameLine();

			if(ImGuiEx::Button("Copy to Clipboard"))
			{
                if(items.count() > 0)
                {
                    std::string clipboardText;
                    for(size_t i = 0; i < items.count(); i++)
                    {
                        clipboardText += items.get_at(i) + "\n";
                    }

                    clipboardText.pop_back();
				    ImGui::SetClipboardText(clipboardText.c_str());
                }

			}

            bool copy_to_clipboard = false;
            
            ImVec4 bg = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
            ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);

            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
            
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
            if (copy_to_clipboard)
                ImGui::LogToClipboard();


			auto stringContains = [] (const std::string &haystack, const std::string &needle) -> bool {
				return haystack.find(needle) != std::string::npos;
			};

			auto stringStartsWith = [] (const std::string &str, const std::string &suffix) -> bool {
				if (str.length() >= suffix.length()) 
					return (0 == str.compare(0, suffix.length(), suffix));
				return false;
			};

            for (int i = 0; i < items.count(); i++)
            {
                std::string item = items.get_at(i);

                // Normally you would store more information in your item (e.g. make Items[] an array of structure, store color/type etc.)
                bool pop_color = false;
                
                if (stringContains(item, "[error]"))
                { 
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); 
                    pop_color = true;                
                }
                else if (stringStartsWith(item, "# "))
                { 
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.6f, 1.0f)); 
                    pop_color = true; 
                }
                
                ImGuiEx::TextWithColors(item);
                
                if (pop_color)
                    ImGui::PopStyleColor();
            }

            if (copy_to_clipboard)
                ImGui::LogFinish();

            if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);

            scrollToBottom = false;

            ImGui::PopStyleVar();
            ImGui::EndChild();

            ImGui::PopStyleColor(1);

            ImGui::End();

            EndNoWindowFlags();
        }

        void ExecCommand(const std::string &command_line)
        {
            //if(!BypassLog)
            {
                if(command_line[0] != '/')
                    AddLog(command_line);
            }

            // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
            historyPos = -1;
			int removeIndex = -1;
            for (int i = history.size()-1; i >= 0; i--)
            {
                if (Stricmp(history[i], command_line) == 0)
                {
					removeIndex = i;
                    break;
                }
            }

			if(removeIndex >= 0)
			{
				history.erase(history.begin() + removeIndex);
			}

            history.push_back(command_line);

            // Process command
            if (Stricmp(command_line, "/CLEAR") == 0)
            {
                ClearLog();
            }
            else if (Stricmp(command_line, "/HELP") == 0)
            {
                AddLog("Commands:");
                for (int i = 0; i < commands.size(); i++)
                    AddLog(commands[i]);
            }
            else if (Stricmp(command_line, "/HISTORY") == 0)
            {
                int first = history.size() - 10;
                for (int i = first > 0 ? first : 0; i < history.size(); i++)
                    AddLog(history[i]);
            }
            else
            {
                if(processCommand)
                {
                    if(!processCommand(command_line))
                    {
                        AddLog("Unknown command: " + command_line + "\n");
                    }
                }
            }

            // On commad input, we scroll to bottom even if AutoScroll==false
            scrollToBottom = true;
        }


	private:
        std::string inputBuf;
        const int inputBufMaxLength = 256;
        ring_buffer<std::string> items;
        std::vector<std::string> commands;
        std::vector<std::string> history;
        int historyPos;    // -1: new line, 0..History.Size-1 browsing history.       
        bool showInputField = false;
        ImGuiInputTextCallback textEditCallback;

        ImGuiWindowFlags noWindowFlags = 0;

        void BeginNoWindowFlags()
        {
            noWindowFlags |= ImGuiWindowFlags_NoTitleBar;
            noWindowFlags |= ImGuiWindowFlags_NoResize;
            noWindowFlags |= ImGuiWindowFlags_NoMove;
            noWindowFlags |= ImGuiWindowFlags_NoScrollbar;	
            noWindowFlags |= ImGuiWindowFlags_NoBackground;	
            noWindowFlags |= ImGuiWindowFlags_NoScrollWithMouse;
        }

        void EndNoWindowFlags()
        {
            noWindowFlags = 0;
        }

        int TextEditCallback(ImGuiInputTextCallbackData *data)
        {
            switch (data->EventFlag)
            {
                case ImGuiInputTextFlags_CallbackHistory:
                {
                    int prev_history_pos = historyPos;

                    if (data->EventKey == ImGuiKey_UpArrow)
                    {
                        if (historyPos == -1)
                            historyPos = history.size() - 1;
                        else if (historyPos > 0)
                            historyPos--;
                    }
                    else if (data->EventKey == ImGuiKey_DownArrow)
                    {
                        if (historyPos != -1)
                            if (++historyPos >= history.size())
                                historyPos = -1;
                    }

                    // A better implementation would preserve the data on the current input line along with cursor position.
                    if (prev_history_pos != historyPos)
                    {
                        std::string history_str = (historyPos >= 0) ? history[historyPos] : "";
                        data->DeleteChars(0, data->BufTextLen);
                        data->InsertChars(0, history_str.c_str());
                    }

                    break;
                }
                default:
                    break;
            }

            return 0;
        }
    };
}

#endif