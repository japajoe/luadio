#ifndef LUADIO_COMPILER_HPP
#define LUADIO_COMPILER_HPP

#include "external/lua/lua.hpp"
#include "../system/tokenizer.hpp"
#include <string>
#include <vector>

namespace luadio
{
    enum lua_field_type
    {
        lua_field_type_checkbox,
        lua_field_type_drag_float,
        lua_field_type_drag_int,
        lua_field_type_input_float,
        lua_field_type_input_int,
        lua_field_type_slider_float,
        lua_field_type_slider_int,
        lua_field_type_knob_float
    };

    class lua_field
    {
	public:
        lua_field_type type;
        std::string name;
    };

    class lua_field_float : public lua_field
    {
	public:
        float value;
        float min;
        float max;
        int steps;
    };

    class lua_field_int : public lua_field
    {
	public:
        int value;
        int min;
        int max;
    };

    class lua_field_bool : public lua_field
    {
	public:
        bool value;
    };

	struct lua_field_info 
	{
		union {
			int valueAsInt;
			float valueAsFloat;
			bool valueAsBool;
		};
		lua_field_type type;
		std::string name;
	};
	
	class compiler
	{
	public:
		static bool initialize();
		static void destroy();
		static std::string parse_code(const std::vector<token> &tokens, const std::string &code);
		static std::vector<lua_field*> get_fields(const std::vector<token> &tokens);
		static void push_float(lua_State *L, const std::string &fieldName, float value);
		static void push_int(lua_State *L, const std::string &fieldName, int value);
		static void push_bool(lua_State *L, const std::string &fieldName, bool value);
        static lua_State *get_lua_state();
	private:
		static bool is_numeric_attribute_type_a(const std::vector<token> &tokens, int currentIndex);
		static bool is_numeric_attribute_type_b(const std::vector<token> &tokens, int currentIndex);
		static bool is_boolean_attribute_type(const std::vector<token> &tokens, int currentIndex);
	};
}

#endif