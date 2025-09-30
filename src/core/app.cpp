#include "app.hpp"
#include "../external/imgui/imgui_stdlib.h"
#include "../external/imgui/imgui_ex.h"
#include "../modules/luadio_module.hpp"
#include "../modules/oscillator_module.hpp"
#include "../modules/wavetable_module.hpp"
#include "../modules/script_template.hpp"
#include "../embedded/knobs.hpp"
#include "image.hpp"
#include <algorithm> //std::clamp

namespace luadio
{
	void app::on_load() 
	{
		ma_ex_context_config contextConfig = ma_ex_context_config_init(44100, 2, 1024, NULL);
		pContext = ma_ex_context_init(&contextConfig);
		pSource = ma_ex_audio_source_init(pContext);

		std::memset(&soundGroup, 0, sizeof(ma_sound_group));
		std::memset(&effectNode, 0, sizeof(ma_effect_node));

		ma_sound_group_init(&pContext->engine, 0, nullptr, &soundGroup);
		ma_ex_audio_source_set_group(pSource, &soundGroup);

		ma_effect_node_config effectNodeConfig = ma_effect_node_config_init(2, 44100, on_audio_effect, this);

		if (ma_effect_node_init(ma_engine_get_node_graph(&pContext->engine), &effectNodeConfig, nullptr, &effectNode) == MA_SUCCESS)
		{
			ma_node_attach_output_bus(&effectNode, 0, ma_engine_get_endpoint(&pContext->engine), 0);
			ma_node_attach_output_bus(&soundGroup, 0, &effectNode, 0);
		}

		editor.SetShowHorizontalScrollbar(false);
		auto palette = editor.GetDarkPalette();
		palette[12] = ImColor(ImVec4(0.15f, 0.16f, 0.17f, 1.00f)); //background
		editor.SetPalette(palette);

		editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
		editor.SetShowWhitespaces(false);
		editor.SetText(script_template::get_source());

		if(compiler::initialize())
		{
			luadio_module luadioModule;
			oscillator_module oscillatorModule;
			wavetable_module wavetableModule;

			luadioModule.load(compiler::get_lua_state());
			oscillatorModule.load(compiler::get_lua_state());
			wavetableModule.load(compiler::get_lua_state());

			luadio_module::onLog = [this] (const std::string &message) {
				on_log_message(message);
			};

			luadio_module::onQueueAudio = [this] (const std::string &filePath) {
				on_queue_audio(filePath);
			};
		}

		image img(knobs::get_data(), knobs::get_size());

		if(img.is_loaded())
		{
			knobTexture.generate(&img);
		}

		outputData.resize(1024);
		std::memset(outputData.data(), 0, outputData.size() * sizeof(float));

		constexpr auto getColorFromRGBA = [] (int r, int g, int b, int a) -> ImVec4 {
			return ImVec4((float)r / 255, (float)g / 255, (float)b / 255, (float)a / 255);
		};

		waveformSettings.plotMode = plot_mode_waveform;
		waveformSettings.foregroundColor = ImVec4(0.621f, 1.000f, 0.284f, 1.000f);
		waveformSettings.backgroundColor = ImVec4(1, 1, 1, 1);
		waveformSettings.selectedMode = 0;
		menuState = menu_state_none;
	}

	void app::on_destroy() 
	{
		compiler::destroy();

		clear_fields();

		ma_ex_audio_source_stop(pSource);

		ma_ex_audio_source_uninit(pSource);
		pSource = nullptr;

		ma_sound_group_uninit(&soundGroup);
		ma_effect_node_uninit(&effectNode, nullptr);

		ma_ex_context_uninit(pContext);
		pContext = nullptr;
	}
	
	void app::on_update() 
	{
		updateTimer.update();
		on_script_update();
	}

	void app::on_late_update() 
	{
		if(eventQueue.size() > 0)
		{
			while(eventQueue.size() > 0)
			{
				queue_item item;
				
				if(eventQueue.try_dequeue(item))
				{
					if(item.type == item_type_log)
					{
						logText += item.message + "\n";
					}
					else if(item.type == item_type_audio)
					{
						if(item.message.size() > 0)
						{
							ma_ex_audio_source_play_from_file(pSource, item.message.c_str(), MA_TRUE);
						}
						else
						{
							ma_ex_audio_source_play_from_callback(pSource, on_audio_read, this);
						}
					}
				}

			}
		}
	}

	void app::on_gui()
	{
		show_menu();
		show_panel();
		show_editor();
		show_log();
		show_inspector();
	}

    void app::show_menu()
	{
		if (ImGui::BeginMainMenuBar()) 
		{
			if (ImGui::BeginMenu("File")) 
			{
				if (ImGui::MenuItem("New")) 
				{
					// Handle New action
				}
				if (ImGui::MenuItem("Open")) 
				{

				}
				if (ImGui::MenuItem("Save")) 
				{
				}

				ImGui::Separator(); // Optional: adds a separator line
				if (ImGui::MenuItem("Exit")) 
				{
					close();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Settings")) 
			{
				if (ImGui::MenuItem("Waveform")) 
				{
					menuState = menu_state_settings_visuals;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		if(menuState == menu_state_settings_visuals)
		{
			bool show = (menuState == menu_state_settings_visuals);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1);
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.200f, 0.220f, 0.240f, 1.000f));

			if(ImGui::Begin("Waveform settings", &show))
			{
				ImGui::ColorEdit4("Foreground", &waveformSettings.foregroundColor.x);
				ImGui::ColorEdit4("Background", &waveformSettings.backgroundColor.x);
				
				const char* items[] = { "Wave", "FFT" };

				if (ImGui::BeginCombo("Mode", items[waveformSettings.selectedMode])) 
				{
					for (int i = 0; i < IM_ARRAYSIZE(items); i++) {
						bool isSelected = (waveformSettings.selectedMode == i);
						if (ImGui::Selectable(items[i], isSelected)) 
						{
							waveformSettings.selectedMode = i;
							waveformSettings.plotMode = (plot_mode)i;
						}
						if (isSelected) 
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}



			}
			ImGui::End();

			ImGui::PopStyleVar(1);
			ImGui::PopStyleColor(1);

			if(!show)
			{
				menuState = menu_state_none;
			}
		}
	}

    void app::show_panel()
	{
		ImGui::Begin("Panel");

		size_t bufferSize = concurrentBuffer.read(outputData);

		if(bufferSize > 0)
		{
			auto isPowerOfTwo = [] (size_t n) -> bool {
				return n > 0 && (n & (n - 1)) == 0;
			};

			auto nextPowerOfTwo = [] (size_t n) -> size_t {
				size_t p = 1;
				while (p < n) 
					p <<= 1;
				return p;
			};

			if(waveformSettings.plotMode == plot_mode_fft)
			{
				if(!isPowerOfTwo(outputData.size()))
				{
					size_t n = nextPowerOfTwo(outputData.size()) / 2;
					fftBuffer.resize(n);
				}
				else
				{
					size_t n = outputData.size() / 2;
					if(fftBuffer.size() != n)
						fftBuffer.resize(n);
				}

				size_t numFrames = outputData.size() / 2;

				for (size_t i = 0; i < numFrames; ++i) 
				{
					float left  = outputData[2 * i];
					float right = outputData[2 * i + 1];
					float mono = 0.5f * (left + right); // Average the channels
					fftBuffer[i] = std::complex<double>(static_cast<double>(mono), 0.0);
				}

				fft::perform(fftBuffer, fftBuffer.size());

				ImGuiEx::DrawHistogram(fftBuffer.data(), fftBuffer.size(), ImVec2(128, 64), waveformSettings.foregroundColor, waveformSettings.backgroundColor);
			}
			else
			{
				ImGuiEx::DrawWaveform(outputData.data(), bufferSize / 2, 2, ImVec2(128, 64), waveformSettings.foregroundColor, waveformSettings.backgroundColor);
			}
		}
		else
		{
			ImGuiEx::DrawWaveform(outputData.data(), outputData.size() / 2, 2, ImVec2(128, 64), waveformSettings.foregroundColor, waveformSettings.backgroundColor);
		}
		
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 132);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));

		if(ImGui::Button("Play/Stop"))
		{
			if(ma_ex_audio_source_get_is_playing(pSource) != MA_TRUE)
			{
				auto code = editor.GetText();
				auto tokens = codeTokenizer.tokenize(code);
				code = compiler::parse_code(tokens, code);

				clear_fields();

				fields = compiler::get_fields(tokens);

				lua_State *L = compiler::get_lua_state();

				if (luaL_dostring(L, code.c_str()) == LUA_OK) 
				{
					on_log_message("Compile ok");
					on_script_start();
					ma_ex_audio_source_play_from_callback(pSource, on_audio_read, this);
				}
				else
				{
					const char *pMessage = lua_tostring(L, -1);
					on_log_message(pMessage);
					lua_pop(L, 1);
				}
			}
			else
			{
				on_script_stop();
				ma_ex_audio_source_stop(pSource);
			}

			//audioSource.Play();
		}

		ImGui::PopStyleColor(2);

		ImGui::End();
	}

    void app::show_editor()
	{
		ImGui::Begin("Code");
		editor.Render("Editor");
		ImGui::End();
	}

    void app::show_log()
	{
		ImGui::Begin("Log");

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));

		if(ImGui::Button("Clear Log"))
		{
			logText.clear();
		}

		ImGui::PopStyleColor(2);

		ImGui::InputTextMultiline("##ErrorLog", &logText, ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_ReadOnly);
		ImGui::End();
	}

    void app::show_inspector()
	{
		if(ImGui::Begin("Inspector"))
		{
			for(size_t i = 0; i < fields.size(); i++)
			{
				switch(fields[i]->type)
				{
					case lua_field_type_slider_float:
					{
						lua_field_float *field = static_cast<lua_field_float*>(fields[i]);
						if(ImGui::SliderFloat(fields[i]->name.c_str(), &field->value, field->min, field->max))
						{
							push_field_to_queue(field);
						}
						break;
					}
					case lua_field_type_slider_int:
					{
						lua_field_int *field = static_cast<lua_field_int*>(fields[i]);
						if(ImGui::SliderInt(fields[i]->name.c_str(), &field->value, field->min, field->max))
						{
							push_field_to_queue(field);
						}
						break;
					}
					case lua_field_type_input_float:
					{
						lua_field_float *field = static_cast<lua_field_float*>(fields[i]);
						if(ImGui::InputFloat(field->name.c_str(), &field->value))
						{
							field->value = std::clamp(field->value, field->min, field->max);
							push_field_to_queue(field);
						}
						break;
					}
					case lua_field_type_input_int:
					{
						lua_field_int *field = static_cast<lua_field_int*>(fields[i]);
						if(ImGui::InputInt(field->name.c_str(), &field->value))
						{
							field->value = std::clamp(field->value, field->min, field->max);
							push_field_to_queue(field);
						}
						break;
					}
					case lua_field_type_drag_float:
					{
						lua_field_float *field = static_cast<lua_field_float*>(fields[i]);
						if(ImGui::DragFloat(field->name.c_str(), &field->value))
						{
							field->value = std::clamp(field->value, field->min, field->max);
							push_field_to_queue(field);
						}
						break;
					}
					case lua_field_type_drag_int:
					{
						lua_field_int *field = static_cast<lua_field_int*>(fields[i]);
						if(ImGui::DragInt(field->name.c_str(), &field->value))
						{
							field->value = std::clamp(field->value, field->min, field->max);
							push_field_to_queue(field);
						}
						break;
					}
					case lua_field_type_checkbox:
					{
						lua_field_bool *field = static_cast<lua_field_bool*>(fields[i]);
						if(ImGui::Checkbox(field->name.c_str(), &field->value))
						{
							push_field_to_queue(field);
						}
						break;
					}
					case lua_field_type_knob_float:
					{
						lua_field_float *field = static_cast<lua_field_float*>(fields[i]);
						ImKnobInfo knobInfo = {
							.textureId = knobTexture.get_id(),
							.numberOfSprites = 89,
							.rows = 9,
							.columns = 10
						};

						if(ImGuiEx::Knob(field->name.c_str(), knobInfo, ImVec2(32, 32), &field->value, field->min, field->max, field->steps))
						{
							push_field_to_queue(field);
						}
						ImGui::SameLine();
						float cursorY = ImGui::GetCursorPosY() + 16;
						ImGui::SetCursorPosY(cursorY);
						ImGui::Text(field->name.c_str());
						break;
					}
					default:
						break;
				}
			}
			ImGui::End();
		}
	}

	void app::clear_fields()
	{
		if(fields.size() > 0)
		{
			for(size_t i = 0; i < fields.size(); i++)
			{
				delete fields[i];
			}
			fields.clear();
		}
	}

	void app::push_field_to_queue(lua_field *field)
	{
		//Don't queue data if audio isn't playing
		if(ma_ex_audio_source_get_is_playing(pSource) == MA_FALSE)
			return;

		lua_field_info info;
		info.type = field->type;
		info.name = field->name;
		
		switch(field->type)
		{
			case lua_field_type_drag_float:
			case lua_field_type_input_float:
			case lua_field_type_slider_float:
			case lua_field_type_knob_float:
			{
				lua_field_float *f = static_cast<lua_field_float*>(field);
				info.valueAsFloat = f->value;
				break;
			}
			case lua_field_type_drag_int:
			case lua_field_type_input_int:
			case lua_field_type_slider_int:
			{
				lua_field_int *f = static_cast<lua_field_int*>(field);
				info.valueAsInt = f->value;
				break;
			}
			case lua_field_type_checkbox:
			{
				lua_field_bool *f = static_cast<lua_field_bool*>(field);
				info.valueAsBool = f->value;
				break;
			}
		}

		fieldQueue.enqueue(info);
	}

	void app::on_script_start()
	{
		std::lock_guard<std::mutex> lock(luaMutex);

		lua_State *L = compiler::get_lua_state();

		lua_getglobal(L, "on_start");

		if(lua_isfunction(L, -1))
			lua_pcall(L, 0, 0, 0);

		int top = lua_gettop(L);

		if(top > 0)
			lua_pop(L, top);
	}

	void app::on_script_stop()
	{
		std::lock_guard<std::mutex> lock(luaMutex);

		lua_State *L = compiler::get_lua_state();

		lua_getglobal(L, "on_stop");

		if(lua_isfunction(L, -1))
			lua_pcall(L, 0, 0, 0);

		int top = lua_gettop(L);

		if(top > 0)
			lua_pop(L, top);
	}

	void app::on_script_update()
	{
		if(ma_ex_audio_source_get_is_playing(pSource) == MA_FALSE)
			return;

		std::lock_guard<std::mutex> lock(luaMutex);

		lua_State *L = compiler::get_lua_state();

		lua_getglobal(L, "on_update");

		if(lua_isfunction(L, -1))
		{
			lua_pushnumber(L, updateTimer.deltaTime);
			lua_pcall(L, 1, 0, 0);
		}

		update_fields();

		int top = lua_gettop(L);

		if(top > 0)
			lua_pop(L, top);
	}

	void app::update_fields()
	{
		if(fieldQueue.size() == 0)
			return;

		auto L = compiler::get_lua_state();

		lua_field_info field;

		while(fieldQueue.try_dequeue(field))
		{
			switch(field.type)
			{
				case lua_field_type_drag_float:
				case lua_field_type_input_float:
				case lua_field_type_slider_float:
				case lua_field_type_knob_float:
				{
					compiler::push_float(L, field.name, field.valueAsFloat);
					break;
				}
				case lua_field_type_drag_int:
				case lua_field_type_input_int:
				case lua_field_type_slider_int:
				{
					compiler::push_int(L, field.name, field.valueAsInt);
					break;
				}
				case lua_field_type_checkbox:
				{
					compiler::push_bool(L, field.name, field.valueAsBool);
					break;
				}
			}
		}
	}

	void app::on_log_message(const std::string &message)
	{
		eventQueue.enqueue(queue_item(item_type_log, message));
	}

	void app::on_queue_audio(const std::string &filepath)
	{
		eventQueue.enqueue(queue_item(item_type_audio, filepath));
	}

	void app::on_audio_read(void *pUserData, void *pFramesOut, ma_uint64 frameCount, ma_uint32 channels)
	{
		app *pApp = reinterpret_cast<app*>(pUserData);

		std::lock_guard<std::mutex> lock(pApp->luaMutex);

		std::memset(pFramesOut, 0, frameCount * channels * sizeof(float));

		lua_State *L = compiler::get_lua_state();

		if(L == nullptr)
			return;

		lua_getglobal(L, "on_audio_read");

		if(lua_isfunction(L, -1))
		{
			lua_pushlightuserdata(L, pFramesOut);
			lua_pushinteger(L, (frameCount * channels));
			lua_pushinteger(L, channels);

			if(lua_pcall(L, 3, 0, 0) == 0)
			{
				// float *pData = reinterpret_cast<float*>(pFramesOut);
				// pApp->concurrentBuffer.write(pData, frameCount * channels);
			}
		}

		int top = lua_gettop(L);

		if(top > 0)
			lua_pop(L, top);
	}

	void app::on_audio_effect(ma_node *pNode, const float **ppFramesIn, ma_uint32 *pFrameCountIn, float **ppFramesOut, ma_uint32 *pFrameCountOut)
	{
		ma_effect_node *pEffectNode = reinterpret_cast<ma_effect_node*>(pNode);
		app *pApp = reinterpret_cast<app*>(pEffectNode->config.pUserData);		
		std::lock_guard<std::mutex> lock(pApp->luaMutex);
		lua_State *L = compiler::get_lua_state();

		if(L == nullptr)
			return;

		if(ma_ex_audio_source_get_is_playing(pApp->pSource) == MA_FALSE)
			return;

		const size_t sizeInBytes = *pFrameCountIn * 2 * sizeof(float);

		std::memcpy(ppFramesOut[0], ppFramesIn[0], sizeInBytes);

		lua_getglobal(L, "on_audio_effect");

		if(lua_isfunction(L, -1))
		{
			void *framesIn = (void*)ppFramesIn[0];
			void *frameCountIn = (void*)pFrameCountIn;
			void *framesOut = (void*)ppFramesOut[0];
			void *frameCountOut = (void*)pFrameCountOut;
			int32_t channels = pEffectNode->config.channels;

			lua_pushlightuserdata(L, framesIn);
			lua_pushlightuserdata(L, frameCountIn);
			lua_pushlightuserdata(L, framesOut);
			lua_pushlightuserdata(L, frameCountOut);
			lua_pushinteger(L, channels);

			if(lua_pcall(L, 5, 0, 0) == 0)
			{
				const size_t sizeInSamples = *pFrameCountOut * 2;
				pApp->concurrentBuffer.write(ppFramesOut[0], sizeInSamples);
			}
		}

		int top = lua_gettop(L);

		if(top > 0)
			lua_pop(L, top);
	}
}