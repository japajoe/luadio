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
		texture_2d knobTexture;
		timer updateTimer;
		plot_mode plotMode;
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
		void log_message(const std::string &message);
		static void on_audio_read(void *pUserData, void *pFramesOut, ma_uint64 frameCount, ma_uint32 channels);
	};
}