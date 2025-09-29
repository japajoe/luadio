#include "compiler.hpp"
#include <iostream>
#include <unordered_map>
#include <algorithm>  // For std::transform
#include <cctype>     // For std::tolower
#include <sstream>

namespace luadio
{
	static lua_State *L = nullptr;

	static std::unordered_map<std::string,lua_field_type> gNumericTypes {
		{ "sliderfloat", lua_field_type_slider_float },
		{ "sliderint", lua_field_type_slider_int },
		{ "inputfloat", lua_field_type_input_float },
		{ "knobfloat", lua_field_type_knob_float },
		{ "inputint", lua_field_type_input_int },
		{ "dragfloat", lua_field_type_drag_float },
		{ "dragint", lua_field_type_drag_int },
		{ "checkbox", lua_field_type_checkbox }
	};

	bool compiler::initialize()
	{
		L = luaL_newstate();

		if(L)
		{
    		luaL_openlibs(L);
			return true;
		}

		return false;
	}

	void compiler::destroy()
	{
		lua_close(L);
	}

	std::string compiler::parse_code(const std::vector<token> &tokens, const std::string &code)
	{           
		//When we insert comments, token positions increase by 2 characters each time
		int offset = 0;
		std::string newCode = code;

		for(size_t i = 0; i < tokens.size(); i++)
		{
			if(tokens[i].type != token_type_square_bracket_open)
				continue;
			
			int tokenIndex = i;

			if(is_numeric_attribute_type_a(tokens, tokenIndex))
			{
				int index = tokens[tokenIndex+0].position + offset;
				offset += 2;
				newCode = newCode.insert(index, "--");
			}
			else if(is_numeric_attribute_type_b(tokens, tokenIndex))
			{
				int index = tokens[tokenIndex+0].position + offset;
				offset += 2;
				newCode = newCode.insert(index, "--");
			}
			else if(is_boolean_attribute_type(tokens, tokenIndex))
			{
				int index = tokens[tokenIndex+0].position + offset;
				offset += 2;
				newCode = newCode.insert(index, "--");
			}
		}

		return newCode;
	}

	static bool try_parse_float(const std::string &str, float &value) 
	{
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
	}

	static bool try_parse_int(const std::string &str, int &value) 
	{
        std::stringstream stream(str);
        stream >> value;
        return stream.fail() == false;
	}

	static bool try_parse_bool(const std::string &str, bool &value) 
	{
		std::string s = str;
		std::transform(s.begin(), s.end(), s.begin(), ::tolower);  // Convert to lowercase
		if (s == "true" || s == "1") 
		{
			value = true;
			return true;
		}
		if (s == "false" || s == "0") 
		{
			value = false;
			return true;
		}

		return false;  // If none of the valid boolean strings match
	}

	static std::string to_lower_case(const std::string &str)
	{
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
			return std::tolower(c);
		});
		return result;
	}

	std::vector<lua_field*> compiler::get_fields(const std::vector<token> &tokens)
	{
		std::vector<lua_field*> fields;

		for(size_t i = 0; i < tokens.size(); i++)
		{
			if(tokens[i].type != token_type_square_bracket_open)
				continue;
			
			int tokenIndex = i;

			if(is_numeric_attribute_type_a(tokens, tokenIndex) || is_numeric_attribute_type_b(tokens, tokenIndex))
			{
				std::string fieldType = to_lower_case(tokens[tokenIndex+1].value);

				lua_field_type type = lua_field_type_slider_float;

				if(gNumericTypes.count(fieldType) == 0)
					continue;
				
				type = gNumericTypes[fieldType];

				switch(type)
				{
					case lua_field_type_drag_float:
					case lua_field_type_input_float:
					case lua_field_type_slider_float:
					{
						lua_field_float *field = new lua_field_float();
						field->type = type;

						field->name = tokens[tokenIndex+8].value;

						if(!try_parse_float(tokens[tokenIndex+3].value, field->min))
							continue;
						
						if(!try_parse_float(tokens[tokenIndex+5].value, field->max))
							continue;
						
						if(!try_parse_float(tokens[tokenIndex+10].value, field->value))
							continue;

						fields.push_back(field);
						break;
					}
					case lua_field_type_knob_float:
					{
						lua_field_float *field = new lua_field_float();
						field->type = type;

						field->name = tokens[tokenIndex+10].value;

						if(!try_parse_float(tokens[tokenIndex+3].value, field->min))
							continue;
						
						if(!try_parse_float(tokens[tokenIndex+5].value, field->max))
							continue;

						if(!try_parse_int(tokens[tokenIndex+7].value, field->steps))
							continue;
						
						if(!try_parse_float(tokens[tokenIndex+12].value, field->value))
							continue;

						fields.push_back(field);
						break;
					}
					case lua_field_type_drag_int:
					case lua_field_type_input_int:
					case lua_field_type_slider_int:
					{
						lua_field_int *field = new lua_field_int();
						field->type = type;

						field->name = tokens[tokenIndex+8].value;

						if(!try_parse_int(tokens[tokenIndex+3].value, field->min))
							continue;
						
						if(!try_parse_int(tokens[tokenIndex+5].value, field->max))
							continue;
						
						if(!try_parse_int(tokens[tokenIndex+10].value, field->value))
							continue;

						fields.push_back(field);
						break;
					}
					default:
						break;
				}
			}
			else if(is_boolean_attribute_type(tokens, tokenIndex))
			{
				std::string fieldType = to_lower_case(tokens[tokenIndex+1].value);
				
				lua_field_bool *field = new lua_field_bool();
				field->type = lua_field_type_checkbox;
				field->name = tokens[tokenIndex+3].value;
				
				if(!try_parse_bool(tokens[tokenIndex+5].value, field->value))
					continue;
				
				fields.push_back(field);
			}
		}

		return fields;
	}



	bool compiler::is_numeric_attribute_type_a(const std::vector<token> &tokens, int currentIndex)
	{
		if(currentIndex + 10 >= tokens.size())
			return false;

		if( tokens[currentIndex+0].type == token_type_square_bracket_open &&
			tokens[currentIndex+1].type == token_type_identifier &&
			tokens[currentIndex+2].type == token_type_parenthesis_open &&
			tokens[currentIndex+3].type == token_type_number &&
			tokens[currentIndex+4].type == token_type_comma &&
			tokens[currentIndex+5].type == token_type_number &&
			tokens[currentIndex+6].type == token_type_parenthesis_close &&
			tokens[currentIndex+7].type == token_type_square_bracket_close)
		{
			std::string fieldType = to_lower_case(tokens[currentIndex+1].value);
			return gNumericTypes.count(fieldType) > 0;
		}

		return false;
	}

	bool compiler::is_numeric_attribute_type_b(const std::vector<token> &tokens, int currentIndex)
	{
		if(currentIndex + 12 >= tokens.size())
			return false;

		if( tokens[currentIndex+0].type == token_type_square_bracket_open &&
			tokens[currentIndex+1].type == token_type_identifier &&
			tokens[currentIndex+2].type == token_type_parenthesis_open &&
			tokens[currentIndex+3].type == token_type_number &&
			tokens[currentIndex+4].type == token_type_comma &&
			tokens[currentIndex+5].type == token_type_number &&
			tokens[currentIndex+6].type == token_type_comma &&
			tokens[currentIndex+7].type == token_type_number &&
			tokens[currentIndex+8].type == token_type_parenthesis_close &&
			tokens[currentIndex+9].type == token_type_square_bracket_close)
		{
			std::string fieldType = to_lower_case(tokens[currentIndex+1].value);
			return gNumericTypes.count(fieldType) > 0;
		}

		return false;
	}

	bool compiler::is_boolean_attribute_type(const std::vector<token> &tokens, int currentIndex)
	{
		if(currentIndex + 6 >= tokens.size())
			return false;

		if( tokens[currentIndex+0].type == token_type_square_bracket_open &&
			tokens[currentIndex+1].type == token_type_identifier &&
			tokens[currentIndex+2].type == token_type_square_bracket_close)
		{
			std::string fieldType = to_lower_case(tokens[currentIndex+1].value);
			if(fieldType == "checkbox")
				return true;
		}

		return false;
	}

	void compiler::push_float(lua_State *L, const std::string &fieldName, float value)
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

	void compiler::push_int(lua_State *L, const std::string &fieldName, int value)
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

	void compiler::push_bool(lua_State *L, const std::string &fieldName, bool value)
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

	lua_State *compiler::get_lua_state()
	{
		return L;
	}
}