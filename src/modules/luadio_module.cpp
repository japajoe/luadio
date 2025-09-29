#include "luadio_module.hpp"
#include <unordered_map>

namespace luadio
{
    luadio_log_func luadio_module::onLog = nullptr;
    luadio_queue_audio_func luadio_module::onQueueAudio = nullptr;

	static std::string gSource = R"(local ffi = require ('ffi')
local luadio = {}

function luadio.findMethod(name, signature)
    local address = luadio_find_function_pointer(name)
    if address == nil then
        return nil
    end

    return ffi.cast(signature, tonumber(address))
end

local luadio_print = luadio.findMethod('luadio_print', 'void (__cdecl*)(const char*)')
local luadio_play = luadio.findMethod('luadio_play', 'void (__cdecl*)(void)')
local luadio_play_from_file = luadio.findMethod('luadio_play_from_file', 'void (__cdecl*)(const char*)')

local function c_string(str)
    if type(str) == 'number' then
        str = tostring(str)
    end

    local c_str = ffi.new('char[?]', #str + 1)
    ffi.copy(c_str, str)
    return c_str
end

function luadio.print(message)
    local c_message = c_string(message)
    luadio_print(c_message)
end

function luadio.play(...)
    
    local args = {...}
    local numArgs = #args
    if numArgs == 1 then
        local filepath = args[1]
        local c_filepath = c_string(filepath)
        luadio_play_from_file(c_filepath)
    else
        print("Hello 2??")
        luadio_play()
    end
end

-- Override print function with our own
print = luadio.print

-- Metatable to prevent overwriting
local mt = {
    __newindex = function(table, key, value)
        error('Attempt to modify read-only method: ' .. key)
    end,
}

setmetatable(luadio, mt)

return luadio)";

	void luadio_module::load(lua_State *L)
	{
        register_method(L, "luadio_find_function_pointer", luadio_find_function_pointer);
        
        register_external_method(L, "luadio_print", reinterpret_cast<void*>(luadio_print));
        register_external_method(L, "luadio_play", reinterpret_cast<void*>(luadio_play));
        register_external_method(L, "luadio_play_from_file", reinterpret_cast<void*>(luadio_play_from_file));
		
        register_source(L, gSource, "luadio");
	}

    int luadio_module::luadio_find_function_pointer(lua_State *L)
    {
        if(lua_gettop(L) != 1)
        {
            lua_settop(L, -1);
            return -1;
        }

        if(!lua_isstring(L, -1))
        {
            lua_settop(L, -1);
            return -1;
        }

        std::string name = lua_tostring(L, -1);
        lua_pop(L, 1);

        if(!delegates.contains(name))
        {
            lua_pushnil(L);
            return 1;
        }

        void *pFunc = delegates[name];
        lua_pushinteger(L, reinterpret_cast<uint64_t>(pFunc));

        return 1;
    }

    void luadio_module::luadio_print(const char *message)
    {
        if(onLog)
            onLog(message);
    }

    void luadio_module::luadio_play()
    {
        if(onQueueAudio)
        {
            std::string s;
            onQueueAudio(s);
        }
    }

    void luadio_module::luadio_play_from_file(const char *filePath)
    {
        if(onQueueAudio)
        {
            onQueueAudio(filePath);
        }
    }
}