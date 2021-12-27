#pragma once
#include <map>
#include <string>

#include "byte_interpreter.h"
#pragma warning(disable : 4996)
#define _CP_BEGIN namespace c_parser {
#define _CP_END }
#define _CP byte_interpreter::

_CP_BEGIN
class ILogger
{
public:
	virtual ~ILogger() = default;

	enum class Error
	{
		UNKNOWN_VALUE_TYPE,
		UNKNOWN_INSTRUCTION,
		MISSING_SEMICOLON,
		INCORRECT_CHAR_VALUE,
		INCORRECT_INT_VALUE,
		INCORRECT_FLOAT_VALUE,
		RE_DECLARING_A_VARIABLE
	};
	enum class Warning
	{

	};
	enum class Success
	{

	};
	virtual void OnError(Error error, size_t byte_offset) = 0;
	virtual void OnWarning(Warning warning, size_t byte_offset) = 0;
	virtual void OnSuccess(Success success, size_t byte_offset) = 0;
};
_CP_END

_CP_BEGIN
template<typename _Value_type>
size_t variables_count = 0;
template<typename _Value_type>
struct Variable
{
	explicit Variable(_Value_type value, const size_t address = variables_count<_Value_type>) :
		value(value),
		address(address)
	{
		++variables_count<_Value_type>;
	}
	Variable() = default;
	_Value_type value;
	size_t address;
};
_CP_END

_CP_BEGIN
struct cmp_str
{
	bool operator()(char const* a, char const* b) const
	{
		return std::strcmp(a, b) < 0;
	}
};
struct Compiler
{
	std::map<const char*, Variable<int>, cmp_str> variables_int;
	std::map<const char*, Variable<float>, cmp_str> variables_float;
	std::map<const char*, Variable<char>, cmp_str> variables_char;
};
_CP_END

_CP_BEGIN
typedef enum token_value_types
{
	INT = 0,
	FLOAT,
	CHAR
} ValueTypes;
constexpr const char* value_types_naming[] =
{
	"int",
	"float",
	"char"
};
constexpr unsigned short value_types_length[] =
{
	3,
	5,
	4
};
_CP_END

_CP_BEGIN
constexpr char name_chars[] = "qwertyuiopasdfghjklzxcvbnm1234567890_";
constexpr char name_correct_float_value[] = "1234567890.";
constexpr char name_correct_int_value[] = "1234567890";
_CP_END

_CP_BEGIN
template <unsigned short _Lexeme_length>
struct Lexeme
{
	explicit Lexeme(const char lexeme_str[_Lexeme_length]) :
		lexeme_str(lexeme_str)
	{}

	bool operator == (Lexeme<_Lexeme_length> lexeme)
	{
		bool flag = true;
		for (unsigned short i = 0; i < _Lexeme_length; i++)
			if (this->lexeme_str[i] != lexeme.lexeme_str[i])
				flag = false;
		return flag;
	}
	const char* lexeme_str;
};
_CP_END

_CP_BEGIN
template <unsigned short _Chars_length>
bool char_equal_chars(const char ch, const char chs[_Chars_length])
{
	for (unsigned short i = 0; i < _Chars_length; i++)
		if (ch == chs[i])
			return true;
	return false;
}
_CP_END

_CP_BEGIN
template <unsigned short _Chars_length>
unsigned short __fastcall how_long_to_chars(const char* code, size_t& offset, const size_t code_length, const char chs[_Chars_length])
{
	unsigned short i = 0;
	if (offset + 1 >= code_length)
		return NULL;
	while (!char_equal_chars<_Chars_length>(code[offset], chs))
	{
		if (offset++ >= code_length)
			return NULL;
		i++;
	}
	return i;
}
_CP_END

_CP_BEGIN
inline char* __fastcall create_chars(const char* source, const unsigned short chars_length)
{
	char* chars = new char[static_cast<size_t>(chars_length) + 1];
	memcpy(chars, source, chars_length);
	chars[chars_length] = '\0';
	return chars;
}
_CP_END

_CP_BEGIN
inline char* __fastcall get_variable_name(const char* code, size_t& offset, const size_t code_length)
{
	const unsigned short name_size = how_long_to_chars<4>(code, offset, code_length, " ;={");
	return create_chars(&code[offset - name_size], name_size);
}
_CP_END

_CP_BEGIN
inline bool __fastcall is_correct_int_value(const char* value_str, const unsigned short value_size)
{
	for (unsigned short i = 0; i < value_size; i++)
		if (!char_equal_chars<10>(value_str[i], name_correct_int_value))
			return false;
	return true;
}
inline int __fastcall get_int_value(const char* code, size_t& offset, const size_t code_length, ILogger* logger)
{
	const unsigned short value_size = how_long_to_chars<4>(code, offset, code_length, " ;={");
	const char* value_str = create_chars(&code[offset - value_size], value_size);
	if (logger != nullptr)
		if (!is_correct_int_value(value_str, value_size))
			logger->OnError(ILogger::Error::INCORRECT_INT_VALUE, offset);
	return atoi(value_str);
}

inline bool __fastcall is_correct_float_value(const char* value_str, const unsigned short value_size)
{
	for (unsigned short i = 0; i < value_size; i++)
		if (!char_equal_chars<11>(value_str[i], name_correct_float_value))
			return false;
	return true;
}

inline float __fastcall get_float_value(const char* code, size_t& offset, const size_t code_length, ILogger* logger)
{
	const unsigned short value_size = how_long_to_chars<4>(code, offset, code_length, " ;={");
	const char* value_str = create_chars(&code[offset - value_size], value_size);
	if (logger != nullptr)
		if (!is_correct_float_value(value_str, value_size))
			logger->OnError(ILogger::Error::INCORRECT_FLOAT_VALUE, offset);
	return atof(value_str);
}

inline char __fastcall get_char_value(const char* code, size_t& offset, const size_t code_length, ILogger* logger)
{
	const unsigned short value_size = how_long_to_chars<4>(code, offset, code_length, " ;={");
	const char* value_str = create_chars(&code[offset - value_size], value_size);
	if (logger != nullptr)
		if (value_size != 3 || value_str[0] != '\'' || value_str[2] != '\'')
			logger->OnError(ILogger::Error::INCORRECT_CHAR_VALUE, offset);
	return value_str[1];
}
_CP_END


_CP_BEGIN
inline void __fastcall skip_char(const char* code, size_t& offset, const size_t code_length, const char char_to_skip)
{
	if (offset >= code_length)
		return;
	while (code[offset] == char_to_skip)
		if (offset++ >= code_length)
			return;
}

template <unsigned short _Chars_length>
void __fastcall skip_chars(const char* code, size_t& offset, const size_t code_length,
                           const char chars_to_skip[_Chars_length])
{
	if (offset >= code_length)
		return;
	while (char_equal_chars<_Chars_length>(code[offset], chars_to_skip))
		if (offset++ >= code_length)
			return;
}

inline void __fastcall skip_space(const char* code, size_t& offset, const size_t code_length)
{
	skip_chars<2>(code, offset, code_length, " \n");
}
_CP_END

_CP_BEGIN
template <typename _Variables>
bool __fastcall variable_re_declared(_Variables& variables, const char* variable_name)
{
	if (variables.find(variable_name) != variables.end())
		return true;
	return false;
}
_CP_END

_CP_BEGIN
inline const char* compile_c_to_byte_code_from_str(const char* code, const size_t code_length,
                                                   Compiler* compiler = new Compiler, ILogger* logger = nullptr)
{
	for (size_t offset = 0; offset < code_length; offset++)
	{
		if (Lexeme<value_types_length[INT]>(&code[offset]) == Lexeme<value_types_length[INT]>(value_types_naming[INT]))
		{
			offset += value_types_length[INT];
			skip_space(code, offset, code_length);
			const char* variable_name = get_variable_name(code, offset, code_length);
			skip_space(code, offset, code_length);
			int value = 0;
			if (code[offset] == '=')
			{
				offset++;
				skip_space(code, offset, code_length);
				value = get_int_value(code, offset, code_length, logger);
			}
			skip_space(code, offset, code_length);
			if (logger != nullptr)
				if (code[offset] != ';')
					logger->OnError(ILogger::Error::MISSING_SEMICOLON, offset);
			if (logger != nullptr)
				if (variable_re_declared(compiler->variables_int, variable_name))
					logger->OnError(ILogger::Error::RE_DECLARING_A_VARIABLE, offset);
			compiler->variables_int[variable_name] = Variable<int>(value);
		}
		else if (Lexeme<value_types_length[FLOAT]>(&code[offset]) == Lexeme<value_types_length[FLOAT]>(value_types_naming[FLOAT]))
		{
			offset += value_types_length[FLOAT];
			skip_space(code, offset, code_length);
			const char* variable_name = get_variable_name(code, offset, code_length);
			skip_space(code, offset, code_length);
			float value = 0;
			if (code[offset] == '=')
			{
				offset++;
				skip_space(code, offset, code_length);
				value = get_float_value(code, offset, code_length, logger);
			}
			skip_space(code, offset, code_length);
			if (logger != nullptr)
				if (code[offset] != ';')
					logger->OnError(ILogger::Error::MISSING_SEMICOLON, offset);
			if (logger != nullptr)
				if (variable_re_declared(compiler->variables_float, variable_name))
					logger->OnError(ILogger::Error::RE_DECLARING_A_VARIABLE, offset);
			compiler->variables_float[variable_name] = Variable<float>(value);
		}
		else if (Lexeme<value_types_length[CHAR]>(&code[offset]) == Lexeme<value_types_length[CHAR]>(value_types_naming[CHAR]))
		{
			offset += value_types_length[CHAR];
			skip_space(code, offset, code_length);
			const char* variable_name = get_variable_name(code, offset, code_length);
			skip_space(code, offset, code_length);
			char value = 0;
			if (code[offset] == '=')
			{
				offset++;
				skip_space(code, offset, code_length);
				value = get_char_value(code, offset, code_length, logger);
			}
			skip_space(code, offset, code_length);
			if (logger != nullptr)
				if (code[offset] != ';')
					logger->OnError(ILogger::Error::MISSING_SEMICOLON, offset);
			if (logger != nullptr)
				if (variable_re_declared(compiler->variables_char, variable_name))
					logger->OnError(ILogger::Error::RE_DECLARING_A_VARIABLE, offset);
			compiler->variables_char[variable_name] = Variable<char>(value);
		}
	}
	return nullptr;
}
_CP_END

_CP_BEGIN
class ErrorLogger : public ILogger
{
	void OnError(const Error error, const size_t byte_offset) override
	{
		switch (error) {
		case Error::UNKNOWN_VALUE_TYPE: 
			throw std::runtime_error("UNKNOWN_VALUE_TYPE, offset: " + std::to_string(byte_offset));
		case Error::UNKNOWN_INSTRUCTION: 
			throw std::runtime_error("UNKNOWN_INSTRUCTION, offset: " + std::to_string(byte_offset));
		case Error::MISSING_SEMICOLON: 
			throw std::runtime_error("MISSING_SEMICOLON, offset: " + std::to_string(byte_offset));
		case Error::INCORRECT_CHAR_VALUE: 
			throw std::runtime_error("INCORRECT_CHAR_VALUE, offset: " + std::to_string(byte_offset));
		case Error::INCORRECT_INT_VALUE: 
			throw std::runtime_error("INCORRECT_INT_VALUE, offset: " + std::to_string(byte_offset));
		case Error::INCORRECT_FLOAT_VALUE:
			throw std::runtime_error("INCORRECT_FLOAT_VALUE, offset: " + std::to_string(byte_offset));
		case Error::RE_DECLARING_A_VARIABLE: 
			throw std::runtime_error("RE_DECLARING_A_VARIABLE, offset: " + std::to_string(byte_offset));
		}
	}
	void OnSuccess(Success success, size_t byte_offset) override
	{
		
	}

	void OnWarning(Warning warning, size_t byte_offset) override
	{
		
	}
};
_CP_END