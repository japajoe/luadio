#include "tokenizer.hpp"
#include <cctype>
#include <sstream>

namespace luadio
{
	const std::unordered_set<std::string> tokenizer::keywords = {
		"and", "break", "do", "else", "elseif", "end", "false", "for", "function",
		"if", "in", "local", "nil", "not", "or", "repeat", "return", "then",
		"true", "until", "while"};

	// --- Token implementation ---

	token::token(token_type type, const std::string &value, int position)
		: type(type), value(value), position(position) {}

	std::string token::to_string() const
	{
		std::ostringstream oss;
		oss << position << " ";
		switch (type)
		{
		case token_type_number:
			oss << "number";
			break;
		case token_type_identifier:
			oss << "identifier";
			break;
		case token_type_keyword:
			oss << "keyword";
			break;
		case token_type_string:
			oss << "string";
			break;
		case token_type_operator:
			oss << "operator";
			break;
		case token_type_comma:
			oss << "comma";
			break;
		case token_type_semicolon:
			oss << "semicolon";
			break;
		case token_type_colon:
			oss << "colon";
			break;
		case token_type_square_bracket_open:
			oss << "square_bracket_open";
			break;
		case token_type_square_bracket_close:
			oss << "square_bracket_close";
			break;
		case token_type_parenthesis_open:
			oss << "parenthesis_open";
			break;
		case token_type_parenthesis_close:
			oss << "parenthesis_close";
			break;
		case token_type_curly_brace_open:
			oss << "curly_brace_open";
			break;
		case token_type_curly_brace_close:
			oss << "curly_brace_close";
			break;
		case token_type_comment:
			oss << "comment";
			break;
		case token_type_end_of_file:
			oss << "end_of_file";
			break;
		default:
			oss << "Unknown";
			break;
		}
		oss << " " << value;
		return oss.str();
	}

	// --- Tokenizer implementation ---

	tokenizer::tokenizer() : _input(), _position(0) {}

	char tokenizer::current_char() const
	{
		return _position < static_cast<int>(_input.size()) ? _input[_position] : '\0';
	}

	void tokenizer::advance()
	{
		++_position;
	}

	bool tokenizer::is_end_of_file() const
	{
		return _position >= static_cast<int>(_input.size());
	}

	char tokenizer::peek() const
	{
		return (_position + 1 < static_cast<int>(_input.size())) ? _input[_position + 1] : '\0';
	}

	std::vector<token> tokenizer::tokenize(const std::string &input)
	{
		_input = input;
		_position = 0;
		std::vector<token> tokens;

		while (!is_end_of_file())
		{
			if (std::isspace(current_char()))
			{
				advance();
			}
			else if (current_char() == '-' && peek() == '-')
			{
				tokens.push_back(tokenize_comment());
			}
			else if (current_char() == '-' && (!is_end_of_file() || std::isdigit(peek())) && peek() != '-')
			{
				tokens.push_back(tokenize_negative_number());
			}
			else if (std::isdigit(current_char()) || (current_char() == '.' && std::isdigit(peek())))
			{
				tokens.push_back(tokenize_number());
			}
			else if (std::isalpha(current_char()) || current_char() == '_')
			{
				tokens.push_back(tokenize_identifier());
			}
			else if (current_char() == '"')
			{
				tokens.push_back(tokenize_string());
			}
			else if (std::string("+-*/=<>!&|").find(current_char()) != std::string::npos)
			{
				tokens.push_back(tokenize_operator());
			}
			else if (current_char() == '[')
			{
				tokens.push_back(tokenize_square_bracket_open());
			}
			else if (current_char() == ']')
			{
				tokens.push_back(tokenize_square_bracket_close());
			}
			else if (current_char() == '(')
			{
				tokens.push_back(tokenize_parenthesis_open());
			}
			else if (current_char() == ')')
			{
				tokens.push_back(tokenize_parenthesis_close());
			}
			else if (current_char() == '{')
			{
				tokens.push_back(tokenize_curly_brace_open());
			}
			else if (current_char() == '}')
			{
				tokens.push_back(tokenize_curly_brace_close());
			}
			else if (current_char() == ',')
			{
				tokens.push_back(tokenize_comma());
			}
			else if (current_char() == ';')
			{
				tokens.push_back(tokenize_semicolon());
			}
			else if (current_char() == ':')
			{
				tokens.push_back(tokenize_colon());
			}
			else
			{
				tokens.push_back(token(token_type_unknown, std::string(1, current_char()), _position));
				advance();
			}
		}

		tokens.push_back(token(token_type_end_of_file, "", _position));
		return tokens;
	}

	token tokenizer::tokenize_comment()
	{
		int start = _position;
		advance(); // Skip 1st '-'
		advance(); // Skip 2nd '-'
		while (!is_end_of_file() && current_char() != '\n')
		{
			advance();
		}
		std::string value = _input.substr(start, _position - start);
		return token(token_type_comment, value, start);
	}

	token tokenizer::tokenize_negative_number()
	{
		int start = _position;
		advance(); // Skip the minus sign
		bool hasDecimal = false;
		while (!is_end_of_file() && (std::isdigit(current_char()) || current_char() == '.'))
		{
			if (current_char() == '.')
			{
				if (hasDecimal)
					break;
				hasDecimal = true;
			}
			advance();
		}
		std::string value = _input.substr(start, _position - start);
		return token(token_type_number, value, start);
	}

	token tokenizer::tokenize_number()
	{
		int start = _position;
		bool hasDecimal = false;
		while (!is_end_of_file() && (std::isdigit(current_char()) || current_char() == '.'))
		{
			if (current_char() == '.')
			{
				if (hasDecimal)
					break;
				hasDecimal = true;
			}
			advance();
		}
		std::string value = _input.substr(start, _position - start);
		return token(token_type_number, value, start);
	}

	token tokenizer::tokenize_identifier()
	{
		int start = _position;
		while (!is_end_of_file() && (std::isalnum(current_char()) || current_char() == '_'))
		{
			advance();
		}
		std::string value = _input.substr(start, _position - start);
		token_type type = (keywords.find(value) != keywords.end()) ? token_type_keyword : token_type_identifier;
		return token(type, value, start);
	}

	token tokenizer::tokenize_string()
	{
		int start = _position;
		advance(); // Skip opening quote
		while (!is_end_of_file() && current_char() != '"')
		{
			advance();
		}
		advance(); // Skip closing quote
		std::string value = _input.substr(start, _position - start);
		return token(token_type_string, value, start);
	}

	token tokenizer::tokenize_operator()
	{
		int start = _position;
		advance();
		std::string value = _input.substr(start, _position - start);
		return token(token_type_operator, value, start);
	}

	token tokenizer::tokenize_square_bracket_open()
	{
		std::string value(1, current_char());
		int pos = _position;
		advance();
		return token(token_type_square_bracket_open, value, pos);
	}

	token tokenizer::tokenize_square_bracket_close()
	{
		std::string value(1, current_char());
		int pos = _position;
		advance();
		return token(token_type_square_bracket_close, value, pos);
	}

	token tokenizer::tokenize_parenthesis_open()
	{
		std::string value(1, current_char());
		int pos = _position;
		advance();
		return token(token_type_parenthesis_open, value, pos);
	}

	token tokenizer::tokenize_parenthesis_close()
	{
		std::string value(1, current_char());
		int pos = _position;
		advance();
		return token(token_type_parenthesis_close, value, pos);
	}

	token tokenizer::tokenize_curly_brace_open()
	{
		std::string value(1, current_char());
		int pos = _position;
		advance();
		return token(token_type_curly_brace_open, value, pos);
	}

	token tokenizer::tokenize_curly_brace_close()
	{
		std::string value(1, current_char());
		int pos = _position;
		advance();
		return token(token_type_curly_brace_close, value, pos);
	}

	token tokenizer::tokenize_comma()
	{
		std::string value(1, current_char());
		int pos = _position;
		advance();
		return token(token_type_comma, value, pos);
	}

	token tokenizer::tokenize_semicolon()
	{
		std::string value(1, current_char());
		int pos = _position;
		advance();
		return token(token_type_semicolon, value, pos);
	}

	token tokenizer::tokenize_colon()
	{
		std::string value(1, current_char());
		int pos = _position;
		advance();
		return token(token_type_colon, value, pos);
	}
}