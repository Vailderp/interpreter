#include "c_parser.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#define TO_2_BIT(value)	reinterpret_cast<char*>(&(value))[0], reinterpret_cast<char*>(&(value))[1]
#define TO_4_BIT(value)	reinterpret_cast<char*>(&(value))[0], reinterpret_cast<char*>(&(value))[1],\
						reinterpret_cast<char*>(&(value))[2], reinterpret_cast<char*>(&(value))[3] 
#define TO_8_BIT(value)	reinterpret_cast<char*>(&(value))[0], reinterpret_cast<char*>(&(value))[1],\
						reinterpret_cast<char*>(&(value))[2], reinterpret_cast<char*>(&(value))[3], \
						reinterpret_cast<char*>(&(value))[4], reinterpret_cast<char*>(&(value))[5],\
						reinterpret_cast<char*>(&(value))[6], reinterpret_cast<char*>(&(value))[7]
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
		const std::string c_code = " \
			char ch1 = 'a';\
			char ch1 = 'b';\
			char ch3 = 'c';\
			char ch4 = 'd';\
		";

		const auto compiler = new c_parser::Compiler;
		const auto ErrorLogger = new c_parser::ErrorLogger;

		const steady_clock::time_point start = steady_clock::now();
		try
		{
			compile_c_to_byte_code_from_str(c_code.c_str(), c_code.size(), compiler, ErrorLogger);
		}
		catch (const std::exception& e)
		{
			std::cout << "ERROR: " << e.what() << std::endl;
		}
		const steady_clock::time_point end = steady_clock::now();
		const steady_clock::duration dur = end - start;
		std::cout << "timing in milliseconds: " << duration<double, std::milli>(dur).count() << " ms" << std::endl;
		std::cout << "timing in nanoseconds: " << duration<double, std::nano>(dur).count() << " ns" << std::endl;

		for (auto& el : compiler->variables_char)
		{
			std::cout << "value of " << el.first << " = " << el.second.value << " : " << el.second.address << "\n";
		}
	}

		std::vector<char> program_bytes =
		{
			PUSH, INT,	TO_4_BIT(var1),
			PUSH, INT,	TO_4_BIT(var2),
			ADD, INT, TO_8_BIT(one), TO_8_BIT(two),
			//JMP, TO_8_BIT(to_jmp)
		};

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
	