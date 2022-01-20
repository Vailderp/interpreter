#pragma once
#include <chrono>
#include <vector>
#include <iostream>

#define _BI_BEGIN namespace byte_interpreter {
#define _BI_END }
#define _BI byte_interpreter::
#define TO_2_BIT(value)	(byte_t)(value), (byte_t)(value >> 8)
#define TO_4_BIT(value)	(byte_t)(value), (byte_t)(value >> 8), (byte_t)(value >> 16), (byte_t)(value >> 24)

_BI_BEGIN
using byte_t = unsigned char;
using address_t = unsigned int;
struct instruction_args;
using instr_args_t = std::vector<instruction_args>;
_BI_END

_BI_BEGIN
typedef enum _e_virtual_machine_bitness : byte_t
{
	BI_VIRTUAL_MACHINE_X32 = 0x00,
	BI_VIRTUAL_MACHINE_X64 = 0x01,
} _e_virtual_machine_bitness;
_BI_END

_BI_BEGIN
typedef enum _e_ident_scopes : byte_t
{
	BI_IDENT_SCOPES_GLOBAL = 0x02,
	BI_IDENT_SCOPES_LOCAL = 0x03,
} _e_ident_scopes;
_BI_END

_BI_BEGIN
enum _e_value_types : byte_t
{
	BI_VT_CHAR = 0x04,
	BI_VT_INT = 0x05,
	BI_VT_FLOAT = 0x06,
	BI_VT_STRUCT = 0x07
} _e_value_types;
_BI_END

_BI_BEGIN
enum _e_copy_types : byte_t
{
	BI_CT_VALUE = 0x08,
	BI_CT_VARIABLE = 0x09,
	BI_CT_FUNCTION = 0x0A
} _e_copy_types;
_BI_END

_BI_BEGIN
enum _e_compile_rules : byte_t
{
	BI_RULE_REGION_IDENT_SCOPES = 0x0B,
	BI_RULE_REGION_MAIN_LOCAL = 0x0C
} _e_compile_rules;
_BI_END

_BI_BEGIN
typedef enum _e_instructions : byte_t
{
	BI_INS_PUSH = 0x0D,
	BI_INS_POP,
	BI_INS_SET,
	BI_INS_ADD,
	BI_INS_SUB,
	BI_INS_MUL,
	BI_INS_DIV,
} _e_instructions;
_BI_END

_BI_BEGIN
struct ExecProgram;
class ILogger;
struct bi_state
{
	ExecProgram& program;
	size_t& offset;
	address_t& ident_scopes_local_address;
};
struct instruction_args
{
	//__fastcall because parameters are passed through registers RCX, RDX
	instruction_args(void(__fastcall* instruction)(bi_state&, void*), void* args) :
		instruction(instruction),
		args(args)
	{}

	//instruction, for tape performer
	void(__fastcall* instruction)(bi_state&, void*);

	//instruction arguments, for tape performer
	void* args;
};

struct ins_args_ident_scopes_local_address
{
	instruction_args* address;
	size_t count;
};


struct ins_args_ident_scopes_local_addresses_with_main
{
	ins_args_ident_scopes_local_address* ins_args_ident_scopes_local_addresses;
	address_t local_main_address;
};

_BI_END

_BI_BEGIN
struct ExecProgram
{
	ins_args_ident_scopes_local_addresses_with_main	ins_args_ident_scopes_local_addrs_with_main;
	size_t											instr_args_count;
	void** memory;
	size_t											memory_count;
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
		arg(arg),
		next_args(args...)
	{}
	_Arg_type			arg;
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

_BI_BEGIN
class ILogger
{
public:
	enum class Error
	{
		UNKNOWN_VALUE_TYPE,
		UNKNOWN_INSTRUCTION,
		INCORRECT_PUSH_INSTRUCTION
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
_BI_END

_BI_BEGIN
template <typename _Value_type>
_Value_type __fastcall make_value(byte_t*& code)
{
	_Value_type value{};
	for (unsigned int i = 0; i < sizeof(_Value_type); i++)
	{
		((byte_t*)(&value))[i] = *code;
		code++;
	}
	return value;
}

template <typename _Value_type>
void* __fastcall make_new_value(byte_t*& code)
{
	byte_t* value_new_memory = new byte_t[sizeof(_Value_type)];
	const auto value = make_value<_Value_type>(code);
	memcpy(value_new_memory, &value, sizeof(_Value_type));
	return value_new_memory;
}

void* __fastcall make_unknown_value(byte_t*& code)
{
	const auto size = make_value<address_t>(code);
	void* value = new byte_t[size];
	for (address_t i = 0; i < size; i++)
	{
		static_cast<byte_t*>(value)[i] = *code;
		code++;
	}
	return value;
}

address_t __fastcall make_address(byte_t*& code)
{
	return make_value<address_t>(code);
}

void* __fastcall make_unknown_variable_args(byte_t*& code)
{
	const auto size = make_value<address_t>(code);
	void* value = new byte_t[size];
	for (address_t i = 0; i < size; i++)
	{
		static_cast<byte_t*>(value)[i] = *code;
		code++;
	}
	return value;
}
_BI_END

_BI_BEGIN
typedef struct _s_bi_pt_variable
{
	const address_t address_dst;
	const address_t address_src;
} _s_bi_pt_variable;

template<typename _Value_type>
struct _s_bi_pt_value
{
	const address_t address_dst;
	const _Value_type value_src;
};

struct _s_bi_ins_set_pt_variable
{
	struct _s_bi_pt_variable var_copying;
	const address_t sizeof_value_type;
	static void __fastcall instruction(bi_state& state, void* args)
	{
		const auto& arg = *static_cast<_s_bi_ins_set_pt_variable*>(args);
		memcpy(
			state.program.memory[arg.var_copying.address_dst],
			state.program.memory[arg.var_copying.address_src],
			static_cast<size_t>(arg.sizeof_value_type)
		);
	}
};
_BI_END

_BI_BEGIN

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// MUL
template <typename _Value_type>
struct _s_bi_ins_mul_vt_type_pt_value
{
	const _s_bi_pt_value<_Value_type> var_copying;
	static void __fastcall instruction(bi_state& state, void* args)
	{
		const auto& arg = *static_cast<_s_bi_ins_mul_vt_type_pt_value<_Value_type>*>(args);
		*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_dst]) *= arg.var_copying.value_src;
	}
};

template <typename _Value_type>
struct _s_bi_ins_mul_vt_type_pt_variable
{
	const _s_bi_pt_variable var_copying;
	static void __fastcall instruction(bi_state& state, void* args)
	{
		const auto& arg = *static_cast<_s_bi_ins_mul_vt_type_pt_variable<_Value_type>*>(args);
		*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_dst]) *=
			*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_src]);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// DIV
template <typename _Value_type>
struct _s_bi_ins_div_vt_type_pt_value
{
	const _s_bi_pt_value<_Value_type> var_copying;
	static void __fastcall instruction(bi_state& state, void* args)
	{
		const auto& arg = *static_cast<_s_bi_ins_mul_vt_type_pt_value<_Value_type>*>(args);
		*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_dst]) /= arg.var_copying.value_src;
	}
};

template <typename _Value_type>
struct _s_bi_ins_div_vt_type_pt_variable
{
	const _s_bi_pt_variable var_copying;
	static void __fastcall instruction(bi_state& state, void* args)
	{
		const auto& arg = *static_cast<_s_bi_ins_mul_vt_type_pt_variable<_Value_type>*>(args);
		*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_dst]) /=
			*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_src]);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ADD
template <typename _Value_type>
struct _s_bi_ins_add_vt_type_pt_value
{
	const _s_bi_pt_value<_Value_type> var_copying;
	static void __fastcall instruction(bi_state& state, void* args)
	{
		const auto& arg = *static_cast<_s_bi_ins_mul_vt_type_pt_value<_Value_type>*>(args);
		*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_dst]) += arg.var_copying.value_src;
	}
};

template <typename _Value_type>
struct _s_bi_ins_add_vt_type_pt_variable
{
	const _s_bi_pt_variable var_copying;
	static void __fastcall instruction(bi_state& state, void* args)
	{
		const auto& arg = *static_cast<_s_bi_ins_mul_vt_type_pt_variable<_Value_type>*>(args);
		*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_dst]) +=
			*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_src]);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////// SUB
template <typename _Value_type>
struct _s_bi_ins_sub_vt_type_pt_value
{
	const _s_bi_pt_value<_Value_type> var_copying;
	static void __fastcall instruction(bi_state& state, void* args)
	{
		const auto& arg = *static_cast<_s_bi_ins_mul_vt_type_pt_value<_Value_type>*>(args);
		*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_dst]) -= arg.var_copying.value_src;
	}
};

template <typename _Value_type>
struct _s_bi_ins_sub_vt_type_pt_variable
{
	const _s_bi_pt_variable var_copying;
	static void __fastcall instruction(bi_state& state, void* args)
	{
		const auto& arg = *static_cast<_s_bi_ins_mul_vt_type_pt_variable<_Value_type>*>(args);
		*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_dst]) -=
			*static_cast<_Value_type*>(state.program.memory[arg.var_copying.address_src]);
	}
};
_BI_END

_BI_BEGIN
template <template <typename> class _s_bi_ins_pt_value, template <typename> class _s_bi_ins_pt_variable>
void __fastcall compile_on_vt_type_operation(
	byte_t*& code, std::vector<instruction_args>& instr_args_local)
{
	switch (*code)
	{
	case BI_VT_INT:
		switch (*++code)
		{
		case BI_CT_VALUE:
			instr_args_local.emplace_back
			(_s_bi_ins_pt_value<int>::instruction,
				make_new_value<_s_bi_ins_pt_value<int>>(++code)
			);
			return;

		case BI_CT_VARIABLE:
			instr_args_local.emplace_back(
				_s_bi_ins_pt_variable<int>::instruction,
				make_new_value<_s_bi_ins_pt_variable<int>>(++code)
			);
			return;
		}
		return;

	case BI_VT_FLOAT:
		switch (*++code)
		{
		case BI_CT_VALUE:
			instr_args_local.emplace_back
			(_s_bi_ins_pt_value<float>::instruction,
				make_new_value<_s_bi_ins_pt_value<float>>(++code)
			);
			return;

		case BI_CT_VARIABLE:
			instr_args_local.emplace_back(
				_s_bi_ins_pt_variable<float>::instruction,
				make_new_value<_s_bi_ins_pt_variable<float>>(++code)
			);
			return;
		}
		return;
	}
}
_BI_END

_BI_BEGIN
inline ExecProgram* __fastcall compile_to_exec_program(byte_t* code, const size_t code_length, ILogger* logger = nullptr)
{

	std::vector<instr_args_t>& instr_args_vector = *new std::vector<instr_args_t>;
	std::vector<void*>& memory = *new std::vector<void*>;
	const byte_t* const code_end = code + code_length;
	address_t ident_scopes_local_address = 0;
	byte_t current_ident_scopes = BI_IDENT_SCOPES_GLOBAL;
	while (code < code_end)
	{
		switch (*code)
		{
		case BI_RULE_REGION_MAIN_LOCAL:
			ident_scopes_local_address = make_address(++code);
			break;
		case BI_INS_PUSH:
			memory.push_back(make_unknown_value(++code));
			break;
		case BI_RULE_REGION_IDENT_SCOPES:
			current_ident_scopes = *++code;
			++code;
			if (current_ident_scopes == BI_IDENT_SCOPES_LOCAL)
			{
				instr_args_vector.emplace_back();
			}
		}

		if (current_ident_scopes != BI_IDENT_SCOPES_LOCAL)
			continue;

		switch (*code)
		{
		case BI_INS_SET:
			switch (*++code)
			{
			case BI_CT_VALUE:
			{
				const size_t address = make_address(++code);
				memory[address] = make_unknown_value(++code);
			}
			continue;

			case BI_CT_VARIABLE:
				instr_args_vector.back().emplace_back(
					_s_bi_ins_set_pt_variable::instruction,
					make_new_value<_s_bi_ins_set_pt_variable>(++code)
				);
				continue;
			}
			continue;

		case BI_INS_MUL:
			compile_on_vt_type_operation<_s_bi_ins_mul_vt_type_pt_value, _s_bi_ins_mul_vt_type_pt_variable>(++code, instr_args_vector.back());
			continue;
		case BI_INS_DIV:
			compile_on_vt_type_operation<_s_bi_ins_div_vt_type_pt_value, _s_bi_ins_div_vt_type_pt_variable>(++code, instr_args_vector.back());
			continue;
		case BI_INS_ADD:
			compile_on_vt_type_operation<_s_bi_ins_add_vt_type_pt_value, _s_bi_ins_add_vt_type_pt_variable>(++code, instr_args_vector.back());
			continue;
		case BI_INS_SUB:
			compile_on_vt_type_operation<_s_bi_ins_sub_vt_type_pt_value, _s_bi_ins_sub_vt_type_pt_variable>(++code, instr_args_vector.back());
			continue;
		}
	}

	const ins_args_ident_scopes_local_addresses_with_main ins_args_ident_scopes_local_addrs_with_main{
		new ins_args_ident_scopes_local_address[instr_args_vector.size()], ident_scopes_local_address
	};
	for (size_t i = 0; i < instr_args_vector.size(); i++)
	{
		ins_args_ident_scopes_local_addrs_with_main.ins_args_ident_scopes_local_addresses[i] = {
			instr_args_vector[i].data(), instr_args_vector[i].size()
		};
	}

	return new ExecProgram
	{
		ins_args_ident_scopes_local_addrs_with_main,
		instr_args_vector.size(),
		memory.data(),
		memory.size()
	};
}
_BI_END

_BI_BEGIN
inline void execute(ExecProgram* exec_program)
{
	size_t offset = 0;
	bi_state state{ *exec_program, offset, exec_program->ins_args_ident_scopes_local_addrs_with_main.local_main_address };
	for (; offset < exec_program->ins_args_ident_scopes_local_addrs_with_main.ins_args_ident_scopes_local_addresses[
		       state.ident_scopes_local_address].count; offset++)
		exec_program->ins_args_ident_scopes_local_addrs_with_main.ins_args_ident_scopes_local_addresses[
			state.ident_scopes_local_address].address[offset].instruction(state,
				exec_program->ins_args_ident_scopes_local_addrs_with_main.ins_args_ident_scopes_local_addresses[
					state.ident_scopes_local_address].address[offset].args);
}

inline void execute_with_timer(ExecProgram* exec_program)
{
	const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	execute(exec_program);
	const std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	const std::chrono::high_resolution_clock::duration dur = end - start;
	std::cout << "timing in milliseconds: " << std::chrono::duration<double, std::milli>(dur).count() << " ms" << std::endl;
	std::cout << "timing in nanoseconds: " << std::chrono::duration<double, std::nano>(dur).count() << " ns" << std::endl;
}
_BI_END
