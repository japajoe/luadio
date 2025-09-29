#ifndef LUADIO_LUADIO_MODULE_HPP
#define LUADIO_LUADIO_MODULE_HPP

#include "lua_module.hpp"
#include <functional>
#include <string>

namespace luadio
{
	using luadio_log_func = std::function<void(const std::string&)>;
	using luadio_queue_audio_func = std::function<void(const std::string&)>;

	class luadio_module : public lua_module
	{
	public:
		static luadio_log_func onLog;
		static luadio_queue_audio_func onQueueAudio;
		void load(lua_State *L) override;
	private:
		static int luadio_find_function_pointer(lua_State *L);
		static void luadio_print(const char *message);
		static void luadio_play();
		static void luadio_play_from_file(const char *filePath);
	};
}

#endif