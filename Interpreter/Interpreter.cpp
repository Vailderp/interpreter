#include "c_parser.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>
#include "byte_interpreter.h"

#define TO_2_BIT(value)	reinterpret_cast<char*>(&(value))[0], reinterpret_cast<char*>(&(value))[1]
#define TO_4_BIT(value)	reinterpret_cast<char*>(&(value))[0], reinterpret_cast<char*>(&(value))[1],\
						reinterpret_cast<char*>(&(value))[2], reinterpret_cast<char*>(&(value))[3] 
#define TO_8_BIT(value)	reinterpret_cast<char*>(&(value))[0], reinterpret_cast<char*>(&(value))[1],\
						reinterpret_cast<char*>(&(value))[2], reinterpret_cast<char*>(&(value))[3], \
						reinterpret_cast<char*>(&(value))[4], reinterpret_cast<char*>(&(value))[5],\
						reinterpret_cast<char*>(&(value))[6], reinterpret_cast<char*>(&(value))[7]

#ifdef _WIN64
#define TO_ADDRESS(value) TO_8_BIT(value)
#else
#define TO_ADDRESS(value) TO_4_BIT(value)
#endif

using namespace byte_interpreter;
using namespace std::chrono;

template<typename _Value_type>
constexpr std::vector<_Value_type>& operator << (
	std::vector<_Value_type>& vector_first,
	std::vector<_Value_type>& vector_second
	)
{
	for (_Value_type& element : vector_second)
	{
		vector_first.push_back(element);
	}
	return vector_first;
}


int main()
{
	int var1 = 0;
	int var2 = 1;
	const char ch1 = 'a';

	size_t one = 0;
	size_t two = 1;
	size_t to_jmp = 1;
	{
		tokenizer_parser::Tokenizer tokenizer;
		tokenize_chars_from_file("program.cpp", tokenizer);

		for (const auto& el : tokenizer.token_list)
		{
			//std::cout << el.name << ' ';
		}

		const auto ErrorLogger = new tokenizer_parser::ErrorLogger;
		tokenizer_parser::ParserTree parser_tree;
		try
		{
			parse_tokenizer_with_time(tokenizer, parser_tree, ErrorLogger);
		}
		catch (const std::exception& e)
		{
			std::cout << "ERROR: " << e.what() << std::endl;

		}

		for (const auto& el : parser_tree.nodes)
		{
			switch (el->type)
			{
			case tokenizer_parser::EXPR_CREATING_VARIABLE:
				std::cout << static_cast<char*>(el->left->value) << '=' << *static_cast<int*>(el->right->value) << std::endl;
				break;
			}
		}
	}

	std::vector<char> program_bytes =
	{
		PUSH, INT,	TO_4_BIT(var1),
		PUSH, INT,	TO_4_BIT(var2),
			//JMP, TO_ADDRESS(to_jmp)
	};

	std::vector<char> muls = {
		MUL, INT, TO_ADDRESS(one), TO_ADDRESS(two),
		DIV, INT, TO_ADDRESS(one), TO_ADDRESS(two),
		SUB, INT, TO_ADDRESS(one), TO_ADDRESS(two),
		ADD, INT, TO_ADDRESS(one), TO_ADDRESS(two),
	};

	for (int i = 0; i < 10000 / 4; i++)
	{
		program_bytes << muls;
	}


	Program& program = compile_to_executable(program_bytes.data(), program_bytes.size());

	const steady_clock::time_point start = steady_clock::now();

	//execute(program);

	execute(program);

	const steady_clock::time_point end = steady_clock::now();
	const steady_clock::duration dur = end - start;
	std::cout << "timing in milliseconds: " << duration<double, std::milli>(dur).count() << " ms" << std::endl;
	std::cout << "timing in nanoseconds: " << duration<double, std::nano>(dur).count() << " ns" << std::endl;
	std::cout << "output: " << *program.integer_stack[0] << std::endl;
	//std::cout << "output: " << program.struct_stack[0]->integer_stack_vector[0] << std::endl;
	while (true)
	{
		//std::cout << "output: " <<var1 << std::endl;
	}
}
	