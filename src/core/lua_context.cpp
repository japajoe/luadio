#include "lua_context.hpp"
#include "../modules/oscillator_module.hpp"
#include "../modules/wavetable_module.hpp"

namespace luadio
{
	lua_context::lua_context()
	{
		L = nullptr;
		onLog = nullptr;
	}

	bool lua_context::initialize()
	{
		L = luaL_newstate();

		if(L)
		{
    		luaL_openlibs(L);
			
			oscillator_module oscillatorModule;
			wavetable_module wavetableModule;

			oscillatorModule.load(L);
			wavetableModule.load(L);
			
			return true;
		}

		return false;
	}

	void lua_context::destroy()
	{
		lua_close(L);
		L = nullptr;
	}

	bool lua_context::compile(const std::string &code)
	{
		if (luaL_dostring(L, code.c_str()) == LUA_OK) 
		{
			return true;
		}
		else
		{
			const char *pMessage = lua_tostring(L, -1);
			if(onLog)
				onLog(pMessage);
			lua_pop(L, 1);
			return false;
		}
	}

	void lua_context::push_float(const std::string &fieldName, float value)
	{
		lua_getglobal(L, fieldName.c_str()); // Push value onto the stack
		if(lua_isnumber(L, -1))
		{ 
			lua_pop(L, 1); // Remove the old value from the stack
			lua_pushnumber(L, value); // Push the new value onto the stack
			lua_setglobal(L, fieldName.c_str()); // Set the global variable someValue
		}
		else 
		{
			lua_pop(L, 1); // Remove the old value from the stack
		}
	}

	void lua_context::push_int(const std::string &fieldName, int value)
	{
		lua_getglobal(L, fieldName.c_str()); // Push value onto the stack
		if(lua_isnumber(L, -1))
		{ 
			lua_pop(L, 1); // Remove the old value from the stack
			lua_pushinteger(L, value); // Push the new value onto the stack
			lua_setglobal(L, fieldName.c_str()); // Set the global variable someValue
		}
		else 
		{
			lua_pop(L, 1); // Remove the old value from the stack
		}
	}

	void lua_context::push_bool(const std::string &fieldName, bool value)
	{
		lua_getglobal(L, fieldName.c_str()); // Push value onto the stack
		if (lua_isboolean(L, -1)) 
		{ 
			lua_pop(L, 1); // Remove the old value from the stack
			lua_pushboolean(L, value ? 1 : 0); // Push the new value onto the stack
			lua_setglobal(L, fieldName.c_str()); // Set the global variable someValue
		} 
		else 
		{
			lua_pop(L, 1); // Remove the old value from the stack
		}
	}

	bool lua_context::call_script_on_audio_read(void *pFramesOut, uint64_t frameCount, uint32_t channels)
	{
		std::lock_guard<std::mutex> lock(mutex);

		bool result = false;

		lua_getglobal(L, "on_audio_read");

		if(lua_isfunction(L, -1))
		{
			lua_pushlightuserdata(L, pFramesOut);
			lua_pushinteger(L, (frameCount * channels));
			lua_pushinteger(L, channels);

			if(lua_pcall(L, 3, 0, 0) == 0)
			{
				result = true;
			}
		}

		int top = lua_gettop(L);

		if(top > 0)
			lua_pop(L, top);

		return result;
	}

	bool lua_context::call_script_on_start()
	{
		std::lock_guard<std::mutex> lock(mutex);

		bool result = false;

		lua_getglobal(L, "on_start");

		if(lua_isfunction(L, -1))
		{
			if (lua_pcall(L, 0, 0, 0) == 0)
			{
				result = true;
			}
		}

		int top = lua_gettop(L);

		if(top > 0)
			lua_pop(L, top);

		return result;
	}

	bool lua_context::call_script_on_stop()
	{
		std::lock_guard<std::mutex> lock(mutex);

		bool result = false;

		lua_getglobal(L, "on_stop");

		if(lua_isfunction(L, -1))
		{
			if (lua_pcall(L, 0, 0, 0) == 0)
			{
				result = true;
			}
		}

		int top = lua_gettop(L);

		if(top > 0)
			lua_pop(L, top);

		return result;
	}

	bool lua_context::call_script_on_update(float deltaTime)
	{
		std::lock_guard<std::mutex> lock(mutex);

		bool result = false;

		lua_getglobal(L, "on_update");

		if(lua_isfunction(L, -1))
		{
			lua_pushnumber(L, deltaTime);
			if (lua_pcall(L, 1, 0, 0) == 0)
			{
				result = true;
			}
		}

		int top = lua_gettop(L);

		if(top > 0)
			lua_pop(L, top);

		return result;
	}
}