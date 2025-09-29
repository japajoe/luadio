#ifndef LUADIO_TOKENIZER_HPP
#define LUADIO_TOKENIZER_HPP

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>

namespace luadio
{
	enum token_type
	{
		token_type_number,
		token_type_identifier,
		token_type_keyword,
		token_type_string,
		token_type_operator,
		token_type_comma,
		token_type_semicolon,
		token_type_colon,
		token_type_square_bracket_open,
		token_type_square_bracket_close,
		token_type_parenthesis_open,
		token_type_parenthesis_close,
		token_type_curly_brace_open,
		token_type_curly_brace_close,
		token_type_comment,
		token_type_end_of_file,
		token_type_unknown
	};

	struct token
	{
		token_type type;
		std::string value;
		int position;

		token(token_type type, const std::string &value, int position);

		std::string to_string() const;
	};

	class tokenizer
	{
	public:
		tokenizer();
		std::vector<token> tokenize(const std::string &input);
	private:
		std::string _input;
		int _position;
		static const std::unordered_set<std::string> keywords;
		char current_char() const;
		void advance();
		bool is_end_of_file() const;
		char peek() const;
		token tokenize_comment();
		token tokenize_negative_number();
		token tokenize_number();
		token tokenize_identifier();
		token tokenize_string();
		token tokenize_operator();
		token tokenize_square_bracket_open();
		token tokenize_square_bracket_close();
		token tokenize_parenthesis_open();
		token tokenize_parenthesis_close();
		token tokenize_curly_brace_open();
		token tokenize_curly_brace_close();
		token tokenize_comma();
		token tokenize_semicolon();
		token tokenize_colon();
	};
}

#endif