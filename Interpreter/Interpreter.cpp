#include <chrono>
#include <iostream>
#include <vector>

#include "byte_interpreter.h"
using namespace byte_interpreter;

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

using namespace std::chrono;

int main()
{
	int var1 = 10;
	int var2 = 15;

	unsigned short count = 1;

	size_t one = 0;
	size_t two = 1;
	/*const std::vector<char> program_bytes =
	{
		PUSH, INT,	reinterpret_cast<char*>(&var1)[0],  reinterpret_cast<char*>(&var1)[1],  reinterpret_cast<char*>(&var1)[2],  reinterpret_cast<char*>(&var1)[3],
		SET, INT,	reinterpret_cast<char*>(&one)[0],  reinterpret_cast<char*>(&one)[1],  reinterpret_cast<char*>(&one)[2],  reinterpret_cast<char*>(&one)[3],
					reinterpret_cast<char*>(&var2)[0],  reinterpret_cast<char*>(&var2)[1],  reinterpret_cast<char*>(&var2)[2],  reinterpret_cast<char*>(&var2)[3],
		PUSH, INT,	reinterpret_cast<char*>(&var2)[0],  reinterpret_cast<char*>(&var2)[1],  reinterpret_cast<char*>(&var2)[2],  reinterpret_cast<char*>(&var2)[3],
		IMUL,		reinterpret_cast<char*>(&one)[0],  reinterpret_cast<char*>(&one)[1],  reinterpret_cast<char*>(&one)[2],  reinterpret_cast<char*>(&one)[3],
					reinterpret_cast<char*>(&two)[0],  reinterpret_cast<char*>(&two)[1],  reinterpret_cast<char*>(&two)[2],  reinterpret_cast<char*>(&two)[3],
		IMUL,		reinterpret_cast<char*>(&one)[0],  reinterpret_cast<char*>(&one)[1],  reinterpret_cast<char*>(&one)[2],  reinterpret_cast<char*>(&one)[3],
					reinterpret_cast<char*>(&two)[0],  reinterpret_cast<char*>(&two)[1],  reinterpret_cast<char*>(&two)[2],  reinterpret_cast<char*>(&two)[3]
	};*/

	const std::vector<char> program_bytes =
	{
		PUSH,STRUCT,reinterpret_cast<char*>(&count)[0], reinterpret_cast<char*>(&count)[1], INT, reinterpret_cast<char*>(&var1)[0],  reinterpret_cast<char*>(&var1)[1],
					reinterpret_cast<char*>(&var1)[2],  reinterpret_cast<char*>(&var1)[3],
		PUSH, INT,	reinterpret_cast<char*>(&var1)[0],  reinterpret_cast<char*>(&var1)[1],  reinterpret_cast<char*>(&var1)[2],  reinterpret_cast<char*>(&var1)[3],
		SET, INT,	reinterpret_cast<char*>(&one)[0],  reinterpret_cast<char*>(&one)[1],  reinterpret_cast<char*>(&one)[2],  reinterpret_cast<char*>(&one)[3],
					reinterpret_cast<char*>(&var2)[0],  reinterpret_cast<char*>(&var2)[1],  reinterpret_cast<char*>(&var2)[2],  reinterpret_cast<char*>(&var2)[3],
		PUSH, INT,	reinterpret_cast<char*>(&var2)[0],  reinterpret_cast<char*>(&var2)[1],  reinterpret_cast<char*>(&var2)[2],  reinterpret_cast<char*>(&var2)[3],
		IMUL,		reinterpret_cast<char*>(&one)[0],  reinterpret_cast<char*>(&one)[1],  reinterpret_cast<char*>(&one)[2],  reinterpret_cast<char*>(&one)[3],
					reinterpret_cast<char*>(&two)[0],  reinterpret_cast<char*>(&two)[1],  reinterpret_cast<char*>(&two)[2],  reinterpret_cast<char*>(&two)[3],
		IMUL,		reinterpret_cast<char*>(&one)[0],  reinterpret_cast<char*>(&one)[1],  reinterpret_cast<char*>(&one)[2],  reinterpret_cast<char*>(&one)[3],
					reinterpret_cast<char*>(&two)[0],  reinterpret_cast<char*>(&two)[1],  reinterpret_cast<char*>(&two)[2],  reinterpret_cast<char*>(&two)[3]
	};

	Program& program = compile_to_executable(program_bytes.data(), program_bytes.size());

	const steady_clock::time_point start = steady_clock::now();

	execute(program);

	const steady_clock::time_point end = steady_clock::now();
	const steady_clock::duration dur = end - start;
	std::cout << "timing in milliseconds: " << duration<double, std::milli>(dur).count() << " ms" << std::endl;
	std::cout << "timing in nanoseconds: " << duration<double, std::nano>(dur).count() << " ns" << std::endl;
	std::cout << "output: " << program.integer_stack[0] << std::endl;
	std::cout << "output: " << program.struct_stack[0]->integer_stack_vector[0] << std::endl;

	while (true)
	{

	}
}
	