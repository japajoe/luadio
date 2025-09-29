#ifndef LUADIO_LUA_CONTEXT_HPP
#define LUADIO_LUA_CONTEXT_HPP

#include "external/lua/lua.hpp"
#include <string>
#include <functional>
#include <mutex>
#include <cstdint>

namespace luadio
{
	using lua_log_function = std::function<void(const char*)>;

	class lua_context
	{
	public:
		lua_log_function onLog;
		lua_context();
		bool initialize();
		void destroy();
		bool compile(const std::string &code);
		void push_float(const std::string &fieldName, float value);
		void push_int(const std::string &fieldName, int value);
		void push_bool(const std::string &fieldName, bool value);
		bool call_script_on_audio_read(void *pFramesOut, uint64_t frameCount, uint32_t channels);
		bool call_script_on_start();
		bool call_script_on_stop();
		bool call_script_on_update(float deltaTime);
	private:
		lua_State *L;
		std::mutex mutex;
	};
}

#endif