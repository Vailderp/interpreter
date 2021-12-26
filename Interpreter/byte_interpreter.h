#pragma once
#define _BI_BEGIN namespace byte_interpreter {
#define _BI_END }
#define _BI byte_interpreter::
#include <vector>

_BI_BEGIN
typedef enum ValueTypes : char
{
	INT,
	FLOAT,
	STRUCT,
	POINTER,
} ValueTypes;
_BI_END

_BI_BEGIN
typedef enum Instructions : char
{
	PUSH,
	POP,
	MOV,
	SET,

	IADD,
	ISUB,
	IINC,
	IDEC,
	IMUL,
	IDIV,

	FRES,
	FADD,
	FSUB,
	FINC,
	FDEC,
	FMUL,
	FDIV,

	PRINT,

	CMP,
	JL,
	AND,
	XOR,
	OR,
	NOT,
	JMP
} Instructions;
_BI_END

_BI_BEGIN
unsigned int get_size_of_value_type(const char value_type)
{
	switch (value_type)
	{
	case INT:
		return sizeof(int);
	case FLOAT:
		return sizeof(float);
	case POINTER:
		return sizeof(size_t);
	default:
		return 0;
	}
}
_BI_END

// BEGIN OF BLACK MAGIC //

_BI_BEGIN
class Program;
struct InstrArgs
{
	InstrArgs(void(__fastcall* instr)(Program&, size_t&, void*), void* args) :
		instr(instr),
		args(args)
	{}
	void(__fastcall* instr)(Program&, size_t&, void*);
	void* args;
};
_BI_END

_BI_BEGIN
struct Struct
{
	template<typename _Value_type>
	static void _push_value(std::vector<_Value_type>& stack_vector, _Value_type value)
	{
		stack_vector.push_back(value);
	}

	explicit Struct() = default;

	bool push_value_type(const char* value)
	{
		switch (value[-1])
		{
		case INT:
			_push_value(integer_stack_vector, static_cast<int>(value[0]));
			break;
		case FLOAT:
			_push_value(float_stack_vector, static_cast<float>(value[0]));
			break;
		case POINTER:
			_push_value(pointer_stack_vector, static_cast<size_t>(value[0]));
			break;
		default:
			return false;
		}
		return true;
	}
	void* get_value_type_at(const ValueTypes value_type, const unsigned short index) const
	{
		switch (value_type)
		{
		case INT:
			if (index < integer_stack_vector.size())
				return (void*)&integer_stack_vector[index];
			break;
		case FLOAT:
			if (index < float_stack_vector.size())
				return (void*)&float_stack_vector[index];
			break;
		case STRUCT:
			if (index < struct_stack_vector.size())
				return (void*)&struct_stack_vector[index];
			break;
		case POINTER:
			if (index < pointer_stack_vector.size())
				return (void*)&pointer_stack_vector[index];
			break;
		default:
			return nullptr;
		}
		return nullptr;
	}

	/*void complete_allocation()
	{
		integer_stack_vector
	}*/

	std::vector<int> integer_stack_vector;
	std::vector<float> float_stack_vector;
	std::vector<Struct> struct_stack_vector;
	std::vector<size_t> pointer_stack_vector;

	/*int* integer_stack;
	float* float_stack;
	Struct* struct_stack;
	size_t* pointer_stack;*/
};
_BI_END

_BI_BEGIN

template<typename... _Arg_type>
struct Args;

template<typename _Arg_type>
struct Args<_Arg_type>
{
	constexpr explicit Args(_Arg_type arg) :
		arg(arg)
	{}
	_Arg_type arg;
};

template<typename _Arg_type, typename... _Args_type>
struct Args<_Arg_type, _Args_type...>
{
	constexpr explicit Args(_Arg_type arg, _Args_type... args) :
		arg(arg), next_args(args...)
	{}
	_Arg_type arg;
	Args<_Args_type...> next_args;
};
_BI_END

_BI_BEGIN
template<typename... _Args_types>
void* make_args(_Args_types... args)
{
	return new Args<_Args_types...>(args...);
}
_BI_END
// END OF BLACK MAGIC //

_BI_BEGIN
class ILogger;
Program& compile_to_executable(const char*, const size_t, ILogger*);
class ILogger
{
	friend Program& compile_to_executable(const char*, const size_t, ILogger*);
protected:
	enum class Error
	{
		UNKNOWN_VALUE_TYPE,
		UNKNOWN_INSTRUCTION
	};
	enum class Warning
	{
		
	};
	enum class Success
	{

	};
	virtual void OnError(Error error, size_t byte_offset)		= 0;
	virtual void OnWarning(Warning warning, size_t byte_offset)	= 0;
	virtual void OnSuccess(Success success, size_t byte_offset)	= 0;
};
_BI_END

_BI_BEGIN
class Program
{
public:
	std::vector<InstrArgs> instr_args_vector;
	InstrArgs* instr_args;
	std::vector<int> integer_stack;
	std::vector<float> float_stack;
	std::vector<size_t> pointer_stack;
	std::vector<Struct*> struct_stack;
};
_BI_END

// BEGIN OF INSTRUCTIONS //

_BI_BEGIN
void __fastcall _IPUSH(Program& program, size_t&, void* data)
{
	program.integer_stack.push_back(static_cast<Args<int>*>(data)->arg);
}

void __fastcall _FPUSH(Program& program, size_t&, void* data)
{
	program.float_stack.push_back(static_cast<Args<float>*>(data)->arg);
}

void __fastcall _PPUSH(Program& program, size_t&, void* data)
{
	program.pointer_stack.push_back(static_cast<Args<size_t>*>(data)->arg);
}

void __fastcall _SPUSH(Program& program, size_t&, void* data)
{
	program.struct_stack.push_back(static_cast<Args<Struct*>*>(data)->arg);
}
_BI_END

_BI_BEGIN
void __fastcall _IPOP(Program& program, size_t&, void* data)
{
	program.integer_stack.erase(program.integer_stack.begin() + static_cast<Args<size_t>*>(data)->arg);
}

void __fastcall _FPOP(Program& program, size_t&, void* data)
{
	program.float_stack.erase(program.integer_stack.begin() + static_cast<Args<size_t>*>(data)->arg);
}

void __fastcall _SPOP(Program& program, size_t&, void* data)
{
	program.struct_stack.erase(program.integer_stack.begin() + static_cast<Args<size_t>*>(data)->arg);
}
_BI_END

_BI_BEGIN
void __fastcall _ISET(Program& program, size_t&, void* data)
{
	program.integer_stack[static_cast<Args<size_t, int>*>(data)->arg] = 
		static_cast<Args<size_t, int>*>(data)->next_args.arg;
}

void __fastcall _FSET(Program& program, size_t&, void* data)
{
	program.float_stack.push_back(static_cast<Args<float>*>(data)->arg);
}

void __fastcall _PSET(Program& program, size_t&, void* data)
{
	program.pointer_stack.push_back(static_cast<Args<size_t>*>(data)->arg);
}

void __fastcall _SSET(Program& program, size_t&, void* data)
{
	program.struct_stack.push_back(static_cast<Args<Struct*>*>(data)->arg);
}
_BI_END

_BI_BEGIN
void __fastcall _IADD(Program& program, size_t&, void* data)
{
	program.integer_stack[static_cast<Args<size_t, size_t>*>(data)->arg] +=
		program.integer_stack[static_cast<Args<size_t, size_t>*>(data)->next_args.arg];
}

void __fastcall _ISUB(Program& program, size_t&, void* data)
{
	program.integer_stack[static_cast<Args<size_t, size_t>*>(data)->arg] -=
		program.integer_stack[static_cast<Args<size_t, size_t>*>(data)->next_args.arg];
}

void __fastcall _IMUL(Program& program, size_t&, void* data)
{
	program.integer_stack[static_cast<Args<size_t, size_t>*>(data)->arg] *=
		program.integer_stack[static_cast<Args<size_t, size_t>*>(data)->next_args.arg];
}

void __fastcall _IDIV(Program& program, size_t&, void* data)
{
	program.integer_stack[static_cast<Args<size_t, size_t>*>(data)->arg] /=
		program.integer_stack[static_cast<Args<size_t, size_t>*>(data)->next_args.arg];
}
_BI_END

// END OF INSTRUCTIONS //

_BI_BEGIN
Program& compile_to_executable(const char* code, const size_t code_length, ILogger* logger = nullptr)
{
	const auto program_ptr = new Program;
	decltype(auto) program = *program_ptr;
	for (size_t offset = 0; offset < code_length; offset++)
	{
		switch (code[offset])
		{
		case PUSH:
			switch (code[++offset])
			{
			case INT:
				program.instr_args_vector.emplace_back(
					&_IPUSH, 
					make_args(*reinterpret_cast<const int*>(&code[++offset])
					));
				offset += sizeof(int) - 1;
				break;
			case FLOAT:
				program.instr_args_vector.emplace_back(
					&_FPUSH, 
					make_args(*reinterpret_cast<const float*>(&code[++offset])
					));
				offset += sizeof(float) - 1;
				break;
			case POINTER:
				program.instr_args_vector.emplace_back(
					&_PPUSH, 
					make_args(*reinterpret_cast<const size_t*>(&code[++offset])
					));
				offset += sizeof(size_t) - 1;
				break;
			case STRUCT:
			{
				const auto _struct = new Struct;
				const unsigned short size = *reinterpret_cast<const unsigned short*>(&code[++offset]);
				offset += 2;
				for (unsigned short i = 0; i < size; i++)
				{
					_struct->push_value_type(&code[++offset]);
					offset += get_size_of_value_type(code[offset - 1]);
				}
				program.instr_args_vector.emplace_back(&_SPUSH, make_args(_struct));
				offset -= 1;
			}
				break;
			default:
				if (logger != nullptr)
					logger->OnError(ILogger::Error::UNKNOWN_VALUE_TYPE, offset);
				break;
			}
			break;
		case POP:
			program.instr_args_vector.emplace_back(&_IPOP, make_args(*reinterpret_cast<const size_t*>(&code[++offset]), *reinterpret_cast<const size_t*>(&code[offset + sizeof(size_t)])));
			break;
		case MOV:
			break;
		case SET:
			switch (code[++offset])
			{
			case INT:
				program.instr_args_vector.emplace_back(
					&_ISET, 
					make_args(
						*reinterpret_cast<const size_t*>(&code[++offset]),
						*reinterpret_cast<const int*>(&code[offset + sizeof(size_t)])
					));
				std::cout << "num: " << *reinterpret_cast<const int*>(&code[offset + sizeof(size_t)]) << std::endl;
				offset += sizeof(int) - 1 + sizeof(size_t);
				break;
			case FLOAT:
				program.instr_args_vector.emplace_back(
					&_FSET,
					make_args(
						*reinterpret_cast<const size_t*>(&code[++offset]),
						*reinterpret_cast<const float*>(&code[offset + sizeof(size_t)])
					));
				offset += sizeof(float) - 1;
				break;
			case POINTER:
				program.instr_args_vector.emplace_back(
					&_PSET,
					make_args(
						*reinterpret_cast<const size_t*>(&code[++offset]),
						*reinterpret_cast<const size_t*>(&code[offset + sizeof(size_t)])
					));
				offset += sizeof(size_t) - 1;
				break;
			default:
				if (logger != nullptr)
					logger->OnError(ILogger::Error::UNKNOWN_VALUE_TYPE, offset);
				break;
			}
			break;
		case IADD:
			program.instr_args_vector.emplace_back(&_IADD, 
				make_args(
					*reinterpret_cast<const size_t*>(&code[++offset]), 
					*reinterpret_cast<const size_t*>(&code[offset + sizeof(size_t)])
				));
			offset += sizeof(size_t) * 2 - 1;
			break;
		case ISUB:
			program.instr_args_vector.emplace_back(
				&_ISUB, 
				make_args(
					*reinterpret_cast<const size_t*>(&code[++offset]),
					*reinterpret_cast<const size_t*>(&code[offset + sizeof(size_t)])
				));
			offset += sizeof(size_t) * 2 - 1;
			break;
		case IMUL:
			program.instr_args_vector.emplace_back(
				&_IMUL, 
				make_args(*reinterpret_cast<const size_t*>(&code[++offset]),
					*reinterpret_cast<const size_t*>(&code[offset + sizeof(size_t)])
				));
			offset += sizeof(size_t) * 2 - 1;
			break;
		case IDIV:
			program.instr_args_vector.emplace_back(
				&_IDIV, 
				make_args(
					*reinterpret_cast<const size_t*>(&code[++offset]), 
					*reinterpret_cast<const size_t*>(&code[offset + sizeof(size_t)])
				));
			offset += sizeof(size_t) * 2 - 1;
			break;
		default:
			if (logger != nullptr)
				logger->OnError(ILogger::Error::UNKNOWN_INSTRUCTION, offset);
		}
	}
	program.instr_args = program.instr_args_vector.data();
	return program;
}
_BI_END

_BI_BEGIN
void execute(Program& program)
{
	const size_t size = program.instr_args_vector.size();
	for (size_t offset = 0; offset < size; offset++)
		program.instr_args[offset].instr(program, offset, program.instr_args[offset].args);
}
_BI_END