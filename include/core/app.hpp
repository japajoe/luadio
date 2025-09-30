#include "application.hpp"
#include "compiler.hpp"
#include "queue_item.hpp"
#include "texture_2d.hpp"
#include "../external/imgui/TextEditor.h"
#include "../system/concurrent_queue.hpp"
#include "../system/tokenizer.hpp"
#include "../system/concurrent_buffer.hpp"
#include "../system/timer.hpp"
#include "../system/fft.hpp"
#include "../../libs/miniaudioex/include/miniaudioex.h"
#include <string>
#include <vector>
#include <mutex>

namespace luadio
{
	enum plot_mode
	{
		plot_mode_waveform,
		plot_mode_fft
	};

	enum menu_state
	{
		menu_state_settings_visuals,
		menu_state_none
	};

	struct wave_form_settings
	{
		ImVec4 foregroundColor;
		ImVec4 backgroundColor;
		plot_mode plotMode;
		int selectedMode;
	};

	class app : public application
	{
	public:
		void on_load() override ;
		void on_destroy() override; 
		void on_update() override;
		void on_late_update() override;
		void on_gui() override;
	private:
		TextEditor editor;
		std::string logText;
		std::vector<lua_field*> fields;
		std::vector<float> outputData;
		concurrent_queue<queue_item> eventQueue;
		concurrent_queue<lua_field_info> fieldQueue;
		tokenizer codeTokenizer;
		concurrent_buffer concurrentBuffer;
		std::vector<std::complex<double>> fftBuffer;
		ma_ex_context *pContext;
		ma_ex_audio_source *pSource;
		ma_effect_node effectNode;
		ma_sound_group soundGroup;
		texture_2d knobTexture;
		timer updateTimer;		
		wave_form_settings waveformSettings;
		menu_state menuState;
		std::mutex luaMutex;
		void show_menu();
		void show_panel();
		void show_editor();
		void show_log();
		void show_inspector();
		void clear_fields();
		void push_field_to_queue(lua_field *field);
		void on_script_start();
		void on_script_stop();
		void on_script_update();
		void update_fields();
		void on_log_message(const std::string &message);
		void on_queue_audio(const std::string &filepath);
		static void on_audio_read(void *pUserData, void *pFramesOut, ma_uint64 frameCount, ma_uint32 channels);
		static void on_audio_effect(ma_node *pNode, const float **ppFramesIn, ma_uint32 *pFrameCountIn, float **ppFramesOut, ma_uint32 *pFrameCountOut);
	};
}