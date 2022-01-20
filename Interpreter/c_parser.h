#pragma once
#include <unordered_map>
#include "byte_interpreter.h"
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <chrono>
using namespace std::chrono;
#pragma warning(disable : 4996)
#define _TP_BEGIN namespace tokenizer_parser {
#define _TP_END }
#define _TP tokenizer_parser::

_TP_BEGIN
typedef enum _e_value_type : unsigned int
{
	TP_VALUE_TYPE_ITERATOR = 0x00000001,
	TP_VALUE_TYPE_BEGIN = 0x00000000,
	TP_VALUE_TYPE_INT = TP_VALUE_TYPE_BEGIN * TP_VALUE_TYPE_ITERATOR,
	TP_VALUE_TYPE_FLOAT = 1 * TP_VALUE_TYPE_ITERATOR,
	TP_VALUE_TYPE_CHAR = 2 * TP_VALUE_TYPE_ITERATOR,
	TP_VALUE_TYPE_END = TP_VALUE_TYPE_CHAR,
	TP_VALUE_TYPE_MAX = 0x000000FF
} _e_value_type;

static_assert(TP_VALUE_TYPE_END <= TP_VALUE_TYPE_MAX, "limit of _e_value_type");

typedef enum _e_ident_scopes : unsigned int
{
	TP_IDENT_SCOPES_ITERATOR = 0x00000100,
	TP_IDENT_SCOPES_BEGIN = 0x00000100,
	TP_IDENT_SCOPES_GLOBAL = 1 * TP_IDENT_SCOPES_ITERATOR,
	TP_IDENT_SCOPES_LOCAL = 2 * TP_IDENT_SCOPES_ITERATOR,
	TP_IDENT_SCOPES_END = TP_IDENT_SCOPES_LOCAL,
	TP_IDENT_SCOPES_MAX = 0x0000FF00
} _e_ident_scopes;

static_assert(TP_IDENT_SCOPES_END <= TP_IDENT_SCOPES_MAX, "limit of value _e_ident_scopes");

typedef enum _e_ident_modifer : unsigned int
{
	TP_IDENT_MODIFER_ITERATOR = 0x00010000,
	TP_IDENT_MODIFER_BEGIN = 0x00010000,
	TP_IDENT_MODIFER_CONSTANT = 1 * TP_IDENT_MODIFER_ITERATOR,
	TP_IDENT_MODIFER_MUTABLE = 2 * TP_IDENT_MODIFER_ITERATOR,
	TP_IDENT_MODIFER_CONSTEXPR = 3 * TP_IDENT_MODIFER_ITERATOR,
	TP_IDENT_MODIFER_MACROS = 4 * TP_IDENT_MODIFER_ITERATOR,
	TP_IDENT_MODIFER_END = TP_IDENT_MODIFER_MACROS,
	TP_IDENT_MODIFER_MAX = 0x00FF0000
} _e_ident_modifer;

static_assert(TP_IDENT_MODIFER_END <= TP_IDENT_MODIFER_MAX, "limit of value _e_ident_modifer");

typedef enum _e_token : unsigned int
{
	TP_TOKEN_BEGIN = 0x00000000,
	TP_PUNCTUATOR_BEGIN = TP_VALUE_TYPE_END + 1,
	TP_SQUARE_BKT_OPEN = TP_PUNCTUATOR_BEGIN,
	TP_SQUARE_BKT_CLOSE,
	TP_ROUND_BKT_OPEN,
	TP_ROUND_BKT_CLOSE,
	TP_BRACE_OPEN,
	TP_BRACE_CLOSE,
	TP_DOT,
	TP_ARROW_RIGHT,
	TP_INCREMENT,
	TP_DECREMENT,
	TP_BITAND,
	TP_ASTERISK,
	TP_PLUS,
	TP_MINUS,
	TP_NOT,
	TP_DIV,
	TP_MOD,
	TP_SHL,
	TP_SHR,
	TP_LESS,
	TP_GREAT,
	TP_LESS_EQ,
	TP_GREAT_EQ,
	TP_EQ,
	TP_NOT_EQ,
	TP_CIRCUMFLEX,
	TP_BITOR,
	TP_AND,
	TP_OR,
	TP_COLON,
	TP_SEMICOLON,
	TP_ASSIGNMENT,
	TP_MUL_ASSIGNMENT,
	TP_DIV_ASSIGNMENT,
	TP_MOD_ASSIGNMENT,
	TP_ADD_ASSIGNMENT,
	TP_SUB_ASSIGNMENT,
	TP_SHL_ASSIGNMENT,
	TP_SHR_ASSIGNMENT,
	TP_BITAND_ASSIGNMENT,
	TP_CIRCUMFLEX_ASSIGNMENT,
	TP_BITOR_ASSIGNMENT,
	TP_COMMA,
	TP_SHARP,
	TP_PUNCTUATOR_END = TP_SHARP,

	TP_TOKEN_END = TP_PUNCTUATOR_END,

	TP_UNKNOWN,
	TP_NAME,
	TP_NUMBER_VALUE,
	TP_CHAR_VALUE,
	TP_STRING_VALUE

} _e_token;

struct Lexeme
{
	constexpr Lexeme(const char* name = nullptr, unsigned short length = 0) :
		name(name), length(length)
	{

	}
	const char* name;
	unsigned short length;
	constexpr bool __fastcall compare(Lexeme lexeme) const
	{
		if (this->length != lexeme.length)
			return false;
		for (unsigned short i = 0; i < length; i++)
			if (this->name[i] != lexeme.name[i])
				return  false;
		return true;
	}
	constexpr bool __fastcall compare(const char* chars) const
	{
		for (unsigned short i = 0; i < length; i++)
			if (this->name[i] != chars[i])
				return  false;
		return true;
	}
};


struct TokLexeme
{
	constexpr TokLexeme(unsigned int token, const char* name = nullptr, unsigned short length = 0) :
		token(token), lexeme(name, length)
	{

	}
	unsigned int token;
	Lexeme lexeme;

};

struct _s_cmp_str
{
	bool operator()(char const* a, char const* b) const
	{
		while(*a != '\0')
		{
			if (*a != *b)
				return false;
			a++;
			b++;
		}
		return true;
	}
};

constexpr TokLexeme tokenizer_lexicon[] =
{
	{ TP_VALUE_TYPE_INT,  "int", 3 },
	{TP_VALUE_TYPE_FLOAT,  "float", 5,}			,
	{TP_VALUE_TYPE_CHAR, "char", 4,		 }		,
	{TP_SQUARE_BKT_OPEN, "[", 1,		 }			,
	{TP_SQUARE_BKT_CLOSE, "]", 1,		 }			,
	{TP_ROUND_BKT_OPEN, "(", 1,			 }		,
	{TP_ROUND_BKT_CLOSE, ")", 1,		 }			,
	{TP_BRACE_OPEN, "{", 1,				 }		,
	{TP_BRACE_CLOSE, "}", 1,			 }			,
	{TP_DOT, ".", 1,					 }			,
	{TP_ARROW_RIGHT, "->", 2,			 }			,
	{TP_INCREMENT, "++", 2,				 }		,
	{TP_DECREMENT, "--", 2,				 }		,
	{TP_BITAND, "&", 1,					 }		,
	{TP_ASTERISK, "*", 1,				 }			,
	{TP_PLUS, "+", 1,					 }			,
	{TP_MINUS, "-", 1,					 }		,
	{TP_NOT, "!", 1,					 }			,
	{TP_DIV, "/", 1,					 }			,
	{TP_MOD, "%", 1,					 }			,
	{TP_SHL, "<<", 2,					 }			,
	{TP_SHR, ">>", 2,					 }			,
	{TP_LESS, "<", 1,					 }			,
	{TP_GREAT, ">", 1,					 }		,
	{TP_LESS_EQ, "<=", 2,				 }			,
	{TP_GREAT_EQ, ">=", 2,				 }		,
	{TP_EQ, "==", 2,					 }			,
	{TP_NOT_EQ, "!=", 2,				 }			,
	{TP_CIRCUMFLEX, "^", 1,				 }		,
	{TP_BITOR, "|", 1,					 }		,
	{TP_AND, "&&", 2,					 }			,
	{TP_OR, "||", 2,					 }			,
	{TP_COLON, ":", 1,					 }		,
	{TP_SEMICOLON, ";", 1,				 }		,
	{TP_ASSIGNMENT, "=", 1,				 }		,
	{TP_MUL_ASSIGNMENT, "*=", 2,		 }			,
	{TP_DIV_ASSIGNMENT, "/=", 2,		 }			,
	{TP_MOD_ASSIGNMENT, "%=", 2,		 }			,
	{TP_ADD_ASSIGNMENT, "+=", 2,		 }			,
	{TP_SUB_ASSIGNMENT, "-=", 2,		 }			,
	{TP_SHL_ASSIGNMENT, "<<=", 3,		 }			,
	{TP_SHR_ASSIGNMENT, ">>=", 3,		 }			,
	{TP_BITAND_ASSIGNMENT, "&=", 2,		 }		,
	{TP_CIRCUMFLEX_ASSIGNMENT, "^=", 2,	 }		,
	{TP_BITOR_ASSIGNMENT, "|=", 2,		 }		,
	{TP_COMMA, ",", 1,					 }		,
	{TP_SHARP, "#", 1					 }			,
};
_TP_END

_TP_BEGIN
constexpr char name_chars[] = "qwertyuiopasdfghjklzxcvbnm1234567890_";
constexpr char punctuator_chars[] = "[](){}.,-+<>!/%^&|#*=;: ";
constexpr char name_correct_number_value[] = "1234567890";
_TP_END

_TP_BEGIN
class ILogger
{
public:
	virtual ~ILogger() = default;

	enum class Error : char
	{
		UNKNOWN_LEXEME,
		UNKNOWN_SYNTAX,
		MISSING_SEMICOLON,
		MISSING_BRACE_CLOSE,
		INCORRECT_CHAR_VALUE,
		INCORRECT_NUMBER_VALUE,
		INCORRECT_FLOAT_VALUE,
		RE_DECLARING_A_VARIABLE,
		MISSING_PARENTHESES,
		INCORRECT_MATHEMATICAL_EXPRESSION,
		INCORRECT_VARIABLE_NAME,
		EMPTY_VARIABLE_NAME,
		FILE_CANNOT_BE_OPENED,
		NOT_ERROR
	};

	enum class Warning
	{

	};

	enum class Success
	{
		VALUE_TYPE_INT_DETECTED,
		VALUE_TYPE_FLOAT_DETECTED,
		VALUE_TYPE_CHAR_DETECTED,
		VALUE_CHAR_PUSHED,
		VALUE_FLOAT_PUSHED,
		VALUE_INT_PUSHED,

	};

	virtual void OnError(Error error, size_t byte_offset = 0) = 0;
	virtual void OnWarning(Warning warning, size_t byte_offset) = 0;
	virtual void OnSuccess(Success success, size_t byte_offset) = 0;
};
_TP_END

_TP_BEGIN
template<typename _Array_type>
constexpr unsigned int get_size_of_constexpr_array(_Array_type& arr)
{
	return sizeof arr / sizeof arr[0];
}
_TP_END

_TP_BEGIN
struct variables_dispatcher
{
	static size_t variables_count;
};
size_t variables_dispatcher::variables_count = 0;

struct Variable
{
	explicit Variable(const _e_token value_type, void* value, const size_t address = variables_dispatcher::variables_count) :
		value_type(value_type),
		value(value),
		address(address)
	{
		++variables_dispatcher::variables_count;
	}
	Variable() = default;
	_e_token value_type;
	void* value;
	size_t address;
};

_TP_END

_TP_BEGIN
struct MemoryAble
{
	std::map<const char*, Variable, _s_cmp_str> variables;
};
_TP_END

_TP_BEGIN
template<typename _Value_type>
size_t functions_count = 1;
template<typename _Value_type>
struct Function : MemoryAble
{
	explicit Function(_Value_type value, const size_t address ) :
		value(value),
		address(address)
	{
		//++variables_dispatcher::variables_count;
	}
	Function() = default;
	_Value_type value;
	size_t address{};
};
_TP_END



_TP_BEGIN
template <unsigned short _Chars_length>
bool char_equal_chars(const char ch, const char chs[_Chars_length])
{
	for (unsigned short i = 0; i < _Chars_length; i++)
		if (ch == chs[i])
			return true;
	return false;
}
_TP_END

_TP_BEGIN
template <unsigned short _Chars_length>
unsigned short __fastcall how_long_to_chars(const char* code, size_t& offset, const size_t code_length, const char chs[_Chars_length])
{
	unsigned short i = 0;
	if (offset + 1 >= code_length)
		return i;
	while (!char_equal_chars<_Chars_length>(code[offset], chs))
	{
		if (offset++ >= code_length)
			return i;
		i++;
	}
	return i;
}
_TP_END

_TP_BEGIN
inline char* __fastcall create_chars(const char* source, const size_t chars_length)
{
	char* chars = new char[chars_length + 1];
	memcpy(chars, source, chars_length);
	chars[chars_length] = '\0';
	return chars;
}
_TP_END

_TP_BEGIN
struct _s_trimmed_chars_result
{
	char* chars;
	unsigned short chars_size;
};

template <unsigned short _Chars_length>
_s_trimmed_chars_result __fastcall get_trimmed_chars(const char* code, size_t& offset, const size_t code_length, const char chs[_Chars_length])
{
	const unsigned short chars_size = how_long_to_chars<_Chars_length>(code, offset, code_length, chs);
	return { create_chars(&code[offset - chars_size], chars_size),  chars_size };
}
_TP_END

_TP_BEGIN
inline bool __fastcall is_digit(const char ch)
{
	return char_equal_chars<10>(ch, name_correct_number_value);
}
_TP_END

_TP_BEGIN
inline bool __fastcall is_correct_number_value(const char* value_chars)
{
	unsigned short i = 0;
	if (value_chars[0] == '-')
	{
		return false;
	}
	while (value_chars[i] != '\0')
	{
		if (!char_equal_chars<10>(value_chars[i], name_correct_number_value))
			return false;
		i++;
	}
	return true;
}

inline bool __fastcall is_correct_char_value(const char* value_chars)
{
	if (value_chars[0] != '\'' || value_chars[2] != '\'')
		return false;
	return true;
}

inline bool __fastcall is_correct_variable_name(const char* value_chars)
{
	if (is_digit(value_chars[0]))
		return false;
	unsigned short i = 0;
	while (value_chars[i] != '\0')
	{
		if (!char_equal_chars<38>(value_chars[i], name_chars))
			return false;
		i++;
	}
	return true;
}

inline char __fastcall get_char_value(const char* code, size_t& offset, const size_t code_length, ILogger* logger)
{
	const unsigned short value_size = how_long_to_chars<5>(code, offset, code_length, " ;=})");
	const char* value_str = create_chars(&code[offset - value_size], value_size);
	if (logger != nullptr)
		if (value_size != 3 || value_str[0] != '\'' || value_str[2] != '\'')
			logger->OnError(ILogger::Error::INCORRECT_CHAR_VALUE, offset);
	return value_str[1];
}

_TP_END

_TP_BEGIN
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
	{
		offset++;
		return;
	}
	if (!char_equal_chars<_Chars_length>(code[offset], chars_to_skip))
		return;
	while (char_equal_chars<_Chars_length>(code[offset], chars_to_skip))
		if (offset++ >= code_length)
			return;
}

inline void __fastcall skip_space_chars(const char* code, size_t& offset, const size_t code_length)
{
	skip_chars<1>(code, offset, code_length, " ");
}
_TP_END

_TP_BEGIN
struct _s_split_result
{
	char** words = {};
	size_t words_count = 0;
	char* operator[](size_t word_index) const
	{
		if (word_index >= words_count)
			return nullptr;
		return words[word_index];
	}
};

inline void __fastcall delete_split(const _s_split_result* split_result)
{
	for (int i = 0; i < split_result->words_count; i++)
		delete[] split_result->words[i];
	delete[] split_result->words;
}

template <unsigned short _Chars_length>
_s_split_result __fastcall split(const char* chars, const size_t chars_length, const char chs[_Chars_length])
{
	size_t words_count = 0;
	for (size_t offset = 0; offset < chars_length; offset++)
	{
		skip_chars<_Chars_length>(chars, offset, chars_length, chs);
		how_long_to_chars<_Chars_length>(chars, offset, chars_length, chs);
		words_count++;
	}
	if (words_count <= 0)
	{
		char* empty_result = create_chars(chars, chars_length);
		return { &empty_result, 1 };
	}
	char** words = new char* [words_count];
	size_t word_offset = 0;
	for (size_t offset = 0; offset < chars_length; offset++)
	{
		skip_chars<_Chars_length>(chars, offset, chars_length, chs);
		if (word_offset >= words_count)
			return { nullptr, 0 };
		words[word_offset] = get_trimmed_chars<_Chars_length>(chars, offset, chars_length, chs).chars;
		word_offset++;
	}
	return { words, words_count };
}


inline _s_split_result __fastcall split_space(const char* chars, const size_t chars_length)
{
	return split<2>(chars, chars_length, " \n");
}
_TP_END

_TP_BEGIN
struct _s_unite_words_result
{
	char* chars;
	size_t chars_count;
	char operator[](size_t char_index) const
	{
		if (char_index >= chars_count)
			return '\0';
		return chars[char_index];
	}
};

inline _s_unite_words_result __fastcall unite_words(const char** words, const size_t words_count)
{
	if (words_count == 0)
		return { nullptr, 0 };
	size_t chars_count = 0;
	for (size_t i = 0; i < words_count; i++)
	{
		size_t offset = 0;
		while (words[i][offset] != '\0')
		{
			offset++;
			chars_count++;
		}
	}
	if (chars_count == 0)
		return { nullptr, 0};
	char* united_chars = new char[chars_count + 1];
	size_t global_offset = 0;
	for (size_t i = 0; i < words_count; i++)
	{
		size_t offset = 0;
		while (words[i][offset] != '\0')
		{
			united_chars[global_offset] = words[i][offset];
			offset++;
			global_offset++;
		}
	}
	united_chars[chars_count] = '\0';
	return { united_chars, chars_count };
}
inline _s_unite_words_result __fastcall unite_words(const _s_split_result split_result)
{
	return unite_words(const_cast<const char**>(split_result.words), split_result.words_count);
}
_TP_END

_TP_BEGIN
class Tokenizer
{
public:
	mutable std::vector<TokLexeme> token_list;
	void __fastcall operator << (const TokLexeme token) const
	{
		token_list.push_back(token);
	}
};

inline void __fastcall tokenize_chars(const char* code, size_t code_length, const Tokenizer& tokenizer)
{
	size_t offset = 0;
	while (code[offset] != '\0')
	{
		bool find = false;
		skip_space_chars(code, offset, code_length);
		for (unsigned int i = TP_TOKEN_BEGIN; i <= TP_TOKEN_END; i++)
		{
			const TokLexeme lexeme = { TP_UNKNOWN, &code[offset], tokenizer_lexicon[i].lexeme.length };
			if (tokenizer_lexicon[i].lexeme.compare(lexeme.lexeme))
			{
				tokenizer << tokenizer_lexicon[i];
				offset += tokenizer_lexicon[i].lexeme.length;
				find = true;
				break;
			}
		}
		if (find)
			continue;
		const auto trimmed_result = get_trimmed_chars<24>(code, offset, code_length, punctuator_chars);
		if (is_correct_variable_name(trimmed_result.chars))
		{
			const TokLexeme lexeme = { TP_NAME, trimmed_result.chars, trimmed_result.chars_size };
			tokenizer << lexeme;
			continue;
		}
		if (is_correct_char_value(trimmed_result.chars))
		{
			const TokLexeme lexeme = { TP_CHAR_VALUE, trimmed_result.chars, trimmed_result.chars_size };
			tokenizer << lexeme;
			continue;
		}
		if (is_correct_number_value(trimmed_result.chars))
		{
			const TokLexeme lexeme = { TP_NUMBER_VALUE, trimmed_result.chars, trimmed_result.chars_size };
			tokenizer << lexeme;
			continue;
		}
		const TokLexeme lexeme = { TP_UNKNOWN, trimmed_result.chars, trimmed_result.chars_size };
		tokenizer << lexeme;
	}
}
_TP_END

_TP_BEGIN
inline void tokenize_chars_from_file(const char* source_file, const Tokenizer& tokenizer)
{
	std::ifstream fs(source_file, std::fstream::binary);
	std::string chars;
	if (fs.is_open()) 
	{
		std::string line;
		while (getline(fs, line))
		{
			if (line[line.size() - 1] == '\r' || line[line.size() - 1] == '\n')
				line.erase(line.end() - 1);
			chars += line;
		}
	}
	const high_resolution_clock::time_point start = high_resolution_clock::now();
	tokenize_chars(chars.c_str(), chars.size(), tokenizer);
	const high_resolution_clock::time_point end = high_resolution_clock::now();
	const high_resolution_clock::duration dur = end - start;
	std::cout << "timing in milliseconds: " << duration<double, std::milli>(dur).count() << " ms" << std::endl;
	std::cout << "timing in nanoseconds: " << duration<double, std::nano>(dur).count() << " ns" << std::endl;
}
_TP_END

_TP_BEGIN
	/**
 * \brief в тестах этот вектор быстрее в 4-5 раз
 * \tparam _Type тип данных
 */
template <typename _Type>
class FastVector final
{
public:
	constexpr explicit __fastcall FastVector(const size_t max_size) :
		data(new _Type[max_size])
	{

	}

	_Type* data;
	size_t offset = 0;

	void __fastcall push_back(_Type value)
	{
		data[offset] = value;
		offset++;
	}

	_Type& __fastcall operator[] (const size_t index)
	{
		return data[index];
	}

	_Type& top()
	{
		return data[offset - 1];
	}

	size_t size() const
	{
		return offset;
	}
};
_TP_END

_TP_BEGIN
class PushAble
{
public:
	template <typename _Arg>
	void push(_Arg arg)
	{
		for (unsigned int i = 0; i < sizeof(_Arg); i++)
			instr_args_.push_back(reinterpret_cast<_BI byte_t*>(&arg)[i]);
	}

	template <typename _Arg, typename... _Args>
	void push(_Arg arg, _Args... args)
	{
		for (unsigned int i = 0; i < sizeof(_Arg); i++)
			instr_args_.push_back(reinterpret_cast<_BI byte_t*>(&arg)[i]);
		this->push(args...);
	}
protected:
	std::vector<_BI byte_t> instr_args_;
};

class RegionLocal : public PushAble
{
	_BI address_t address_ = 0;
};

using RegionGlobal = PushAble;

class Translator
{
	struct compare_lexeme
	{
		int operator()(const Lexeme& lexeme1, const Lexeme& lexeme2) const
		{
			return lexeme1.compare(lexeme2);
		}
	};
public:
	void set_compile_rule_region_main_local(_BI address_t address_region_main_local)
	{
		this->address_region_main_local_ = address_region_main_local;
	}

	void set_compile_rule_current_region(_BI address_t address_current_region)
	{
		this->address_current_region_ = address_current_region;
	}

	void set_current_region_local(const char* name)
	{
		region_local_count_++;
		current_region_ = &region_local_at_name_[name];
	}

	decltype(auto) back_region_local() const
	{
		return *current_region_;
	}

	decltype(auto) get_region_local(const char* name)
	{
		return region_local_at_name_[name];
	}

	decltype(auto) get_region_global()
	{
		return region_global_;
	}

	void add_new_variable_address_at_name(const char* name)
	{
		variable_address_at_name_[name] = variable_address_count_;
		std::cout << "count = " << variable_address_at_name_["fl_value2"] << std::endl;
		variable_address_count_++;
	}

	_BI address_t get_variable_address_at_name(const char* name)
	{
		return variable_address_at_name_[name];
	}

private:
	_BI address_t address_region_main_local_;
	_BI address_t address_current_region_;
	std::map<const char*, RegionLocal, _s_cmp_str> region_local_at_name_;


	std::map<const char*, _BI address_t, _s_cmp_str> variable_address_at_name_;


	_BI address_t variable_address_count_;
	_BI address_t region_local_count_;
	RegionLocal* current_region_ = nullptr;
	RegionGlobal region_global_;


};
_TP_END

_TP_BEGIN
#define EXPRESSION_END \
	return;\
	default:\
	if (state.logger != nullptr)\
		state.logger->OnError(ILogger::Error::UNKNOWN_SYNTAX);\
	return;\

_TP_END

_TP_BEGIN
struct parser_state
{
	TokLexeme*& lexeme;
	Translator translator;
	ILogger* logger;
	decltype(auto) __fastcall operator -- () const
	{
		--this->lexeme;
		return *this;
	}
	decltype(auto) __fastcall operator ++ () const
	{
		++this->lexeme;
		return *this;
	}
};
_TP_END

_TP_BEGIN
inline unsigned int __fastcall fast_chars_to_uint_value(const parser_state& state)
{
	unsigned int uint_value = 0;
	switch (state.lexeme->lexeme.length)
	{
	case 10:
		uint_value += (state.lexeme->lexeme.name[state.lexeme->lexeme.length - 10] - '0') * 1000000000;
	case  9:
		uint_value += (state.lexeme->lexeme.name[state.lexeme->lexeme.length - 9] - '0') * 100000000;
	case  8:
		uint_value += (state.lexeme->lexeme.name[state.lexeme->lexeme.length - 8] - '0') * 10000000;
	case  7:
		uint_value += (state.lexeme->lexeme.name[state.lexeme->lexeme.length - 7] - '0') * 1000000;
	case  6:
		uint_value += (state.lexeme->lexeme.name[state.lexeme->lexeme.length - 6] - '0') * 100000;
	case  5:
		uint_value += (state.lexeme->lexeme.name[state.lexeme->lexeme.length - 5] - '0') * 10000;
	case  4:
		uint_value += (state.lexeme->lexeme.name[state.lexeme->lexeme.length - 4] - '0') * 1000;
	case  3:
		uint_value += (state.lexeme->lexeme.name[state.lexeme->lexeme.length - 3] - '0') * 100;
	case  2:
		uint_value += (state.lexeme->lexeme.name[state.lexeme->lexeme.length - 2] - '0') * 10;
	case  1:
		uint_value += state.lexeme->lexeme.name[state.lexeme->lexeme.length - 1] - '0';
		state.lexeme++;
		return uint_value;
	default:
		if (state.logger)
			state.logger->OnError(ILogger::Error::INCORRECT_NUMBER_VALUE);
	}
	return uint_value;
}
_TP_END

_TP_BEGIN
inline signed int __fastcall fast_chars_to_int_value(const parser_state& state)
{
	if (state.lexeme->token == TP_MINUS)
		return -static_cast<signed int>(fast_chars_to_uint_value(++state));
	return static_cast<signed int>(fast_chars_to_uint_value(state));
}
_TP_END

_TP_BEGIN


inline float __fastcall fast_chars_to_float_value(const parser_state& state)
{
	auto float_value = static_cast<float>(fast_chars_to_int_value(state));
	if (static_cast<int>(float_value) > 0)
	{
		switch ((++state.lexeme)->lexeme.length)
		{
		case  7:
			(float_value *= 10) += state.lexeme->lexeme.name[state.lexeme->lexeme.length - 7] - '0';
		case  6:
			(float_value *= 10) += state.lexeme->lexeme.name[state.lexeme->lexeme.length - 6] - '0';
		case  5:
			(float_value *= 10) += state.lexeme->lexeme.name[state.lexeme->lexeme.length - 5] - '0';
		case  4:
			(float_value *= 10) += state.lexeme->lexeme.name[state.lexeme->lexeme.length - 4] - '0';
		case  3:
			(float_value *= 10) += state.lexeme->lexeme.name[state.lexeme->lexeme.length - 3] - '0';
		case  2:
			(float_value *= 10) += state.lexeme->lexeme.name[state.lexeme->lexeme.length - 2] - '0';
		case  1:
			(float_value *= 10) += state.lexeme->lexeme.name[state.lexeme->lexeme.length - 1] - '0';
			break;
		default:
			if (state.logger)
				state.logger->OnError(ILogger::Error::INCORRECT_NUMBER_VALUE);
		}
	}
	else
	{
		switch ((++state.lexeme)->lexeme.length)
		{
		case  7:
			(float_value *= 10) -= state.lexeme->lexeme.name[state.lexeme->lexeme.length - 7] - '0';
		case  6:
			(float_value *= 10) -= state.lexeme->lexeme.name[state.lexeme->lexeme.length - 6] - '0';
		case  5:
			(float_value *= 10) -= state.lexeme->lexeme.name[state.lexeme->lexeme.length - 5] - '0';
		case  4:
			(float_value *= 10) -= state.lexeme->lexeme.name[state.lexeme->lexeme.length - 4] - '0';
		case  3:
			(float_value *= 10) -= state.lexeme->lexeme.name[state.lexeme->lexeme.length - 3] - '0';
		case  2:
			(float_value *= 10) -= state.lexeme->lexeme.name[state.lexeme->lexeme.length - 2] - '0';
		case  1:
			(float_value *= 10) -= state.lexeme->lexeme.name[state.lexeme->lexeme.length - 1] - '0';
			break;
		default:
			if (state.logger)
				state.logger->OnError(ILogger::Error::INCORRECT_NUMBER_VALUE);
		}
	}

	switch (state.lexeme->lexeme.length)
	{
	case  7:
		float_value /= 10000000;
		break;
	case  6:
		float_value /= 1000000;
		break;
	case  5:
		float_value /= 100000;
		break;
	case  4:
		float_value /= 10000;
		break;
	case  3:
		float_value /= 1000;
		break;
	case  2:
		float_value /= 100;
		break;
	case  1:
		float_value /= 10;
		break;
	}
	state.lexeme++;
	return float_value;
}
_TP_END

_TP_BEGIN
inline void __fastcall on_token_get_numerical_value(parser_state& state, const TokLexeme* lexeme_value_type)
{
	if ((state.lexeme + 1)->token == TP_DOT || state.lexeme->token == TP_MINUS && (state.lexeme + 2)->token == TP_DOT)
	{
		state.translator.get_region_global().push(_BI BI_INS_PUSH, 4, fast_chars_to_float_value(state));
		state.translator.add_new_variable_address_at_name((lexeme_value_type + 1)->lexeme.name);
	}
	else
	{
		state.translator.get_region_global().push(_BI BI_INS_PUSH, 4, fast_chars_to_int_value(state));
		state.translator.add_new_variable_address_at_name((lexeme_value_type + 1)->lexeme.name);
	}
}

inline void __fastcall on_token_get_value(parser_state& state, const TokLexeme* lexeme_value_type)
{
	on_token_get_numerical_value(state, lexeme_value_type);
}


inline void __fastcall on_token_type(parser_state& state)
{
	TokLexeme* lexeme_type = state.lexeme;
	switch ((++state.lexeme)->token)
	{
	case TP_NAME:
		switch ((++state.lexeme)->token)
		{
		case TP_ASSIGNMENT:
			switch ((++state.lexeme)->token)
			{
			case TP_NUMBER_VALUE:
			case TP_MINUS:
				on_token_get_value(state, state.lexeme - 3);
				if (state.lexeme->token != TP_SEMICOLON)
					if (state.logger != nullptr)
						state.logger->OnError(ILogger::Error::MISSING_SEMICOLON);
				return;
			}
			return;
		case TP_BRACE_OPEN:
			switch ((++state.lexeme)->token)
			{
			case TP_BRACE_CLOSE:
				state.translator.get_region_global().push(_BI BI_INS_PUSH, 4, static_cast<float>(0));
				state.translator.add_new_variable_address_at_name((state.lexeme - 2)->lexeme.name);
				if ((++state.lexeme)->token != TP_SEMICOLON)
					if (state.logger != nullptr)
						state.logger->OnError(ILogger::Error::MISSING_SEMICOLON);
				return;
			case TP_NUMBER_VALUE:
			case TP_MINUS:
				on_token_get_value(state, state.lexeme - 3);
				if (state.lexeme->token != TP_BRACE_CLOSE)
					if (state.logger != nullptr)
						state.logger->OnError(ILogger::Error::MISSING_BRACE_CLOSE);
				if ((++state.lexeme)->token != TP_SEMICOLON)
					if (state.logger != nullptr)
						state.logger->OnError(ILogger::Error::MISSING_SEMICOLON);
				EXPRESSION_END
			}
			EXPRESSION_END
		}
	}
}

inline void __fastcall parse_tokenizer(const Tokenizer& tokenizer, Translator& translator, ILogger* logger = nullptr)
{
	TokLexeme* lexeme = tokenizer.token_list.data();
	const size_t lexeme_count = tokenizer.token_list.size();
	const TokLexeme* lexeme_end = lexeme + lexeme_count;
	parser_state state
	{
		lexeme,
		translator,
		logger
	};
	while (lexeme <= lexeme_end)
	{
		if (lexeme->token <= TP_VALUE_TYPE_END)
		{
			on_token_type(state);
			continue;
		}
		switch (lexeme->token)
		{
		case TP_UNKNOWN:
			if (logger != nullptr)
				logger->OnError(ILogger::Error::UNKNOWN_LEXEME);
			break;
		default:
			lexeme++;
		}

	}
}

inline void __fastcall parse_tokenizer_with_time(const Tokenizer& tokenizer, Translator& translator, ILogger* logger = nullptr)
{
	const high_resolution_clock::time_point start = high_resolution_clock::now();
	parse_tokenizer(tokenizer, translator, logger);
	const high_resolution_clock::time_point end = high_resolution_clock::now();
	const high_resolution_clock::duration dur = end - start;
	std::cout << "timing in milliseconds: " << duration<double, std::milli>(dur).count() << " ms" << std::endl;
	std::cout << "timing in nanoseconds: " << duration<double, std::nano>(dur).count() << " ns" << std::endl;
}
_TP_END

#include <stdexcept>
#include <string>
_TP_BEGIN
class ErrorLogger : public ILogger
{
	void OnError(const Error error, const size_t byte_offset) override
	{
		switch (error)
		{
		case Error::UNKNOWN_LEXEME:
			throw std::runtime_error("UNKNOWN_LEXEME, offset: " + std::to_string(byte_offset));
		case Error::UNKNOWN_SYNTAX:
			throw std::runtime_error("UNKNOWN_SYNATAX, offset: " + std::to_string(byte_offset));
		case Error::MISSING_SEMICOLON:
			throw std::runtime_error("MISSING_SEMICOLON, offset: " + std::to_string(byte_offset));
		case Error::INCORRECT_CHAR_VALUE:
			throw std::runtime_error("INCORRECT_CHAR_VALUE, offset: " + std::to_string(byte_offset));
		case Error::INCORRECT_NUMBER_VALUE:
			throw std::runtime_error("INCORRECT_NUMBER_VALUE, offset: " + std::to_string(byte_offset));
		case Error::INCORRECT_FLOAT_VALUE:
			throw std::runtime_error("INCORRECT_FLOAT_VALUE, offset: " + std::to_string(byte_offset));
		case Error::RE_DECLARING_A_VARIABLE:
			throw std::runtime_error("RE_DECLARING_A_VARIABLE, offset: " + std::to_string(byte_offset));
		case Error::MISSING_PARENTHESES:
			throw std::runtime_error("MISSING_PARENTHESES, offset: " + std::to_string(byte_offset));
		case Error::INCORRECT_MATHEMATICAL_EXPRESSION:
			throw std::runtime_error("INCORRECT_MATHEMATICAL_EXPRESSION, offset: " + std::to_string(byte_offset));
		case Error::INCORRECT_VARIABLE_NAME:
			throw std::runtime_error("INCORRECT_VARIABLE_NAME, offset: " + std::to_string(byte_offset));
		case Error::EMPTY_VARIABLE_NAME:
			throw std::runtime_error("EMPTY_VARIABLE_NAME, offset: " + std::to_string(byte_offset));
		case Error::MISSING_BRACE_CLOSE:
			throw std::runtime_error("MISSING_BRACE_CLOSE, offset: " + std::to_string(byte_offset));
		case Error::FILE_CANNOT_BE_OPENED:
			throw std::runtime_error("FILE_CANNOT_BE_OPENED, offset: " + std::to_string(byte_offset));
		case Error::NOT_ERROR:
			break;
		default:
			break;
		}
	}
	void OnSuccess(Success success, size_t byte_offset) override
	{
		
	}

	void OnWarning(Warning warning, size_t byte_offset) override
	{
		
	}
};
_TP_END
