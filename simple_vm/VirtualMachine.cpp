#include "VirtualMachine.h"

namespace
{
	enum : uint8_t
	{
		INC = 0x01,		// Rx				инкремент регистра x
		DEC = 0x02,		// Rx				декремент регистра x
		MOV = 0x03,		// Rx, Ry		Rx = Ry копирует регистр Ry в Rx
		MOVC = 0x04,	// "const"	записывает константу в R0. 2й тип команды
		LSL = 0x05,		// Rx				сдвиг регистра Rx на 1 бит влево
		LSR = 0x06,		// Rx				сдвиг регистра Rx на 1 бит вправо
		JMP = 0x07,		// addr			безусловный переход по смещению "addr"(int8_t).все команды перехода имеют 2й тип
		JZ = 0x08,		// addr			условный переход по смещению "addr"(если флаг `ZERO` == 0).
		JNZ = 0x09,		// addr			условный переход по смещению "addr"(если флаг `ZERO` != 0).
		JFE = 0x0A,		// addr			условный переход, если достигнут конец файла(см.IN)
		RET = 0x0B,		//					окончание работы(останов) VM
		ADD = 0x0C,		// Rx, Ry		Rx = Rx + Ry
		SUB = 0x0D,		// Rx, Ry		Rx = Rx - Ry
		XOR = 0x0E,		// Rx, Ry		Rx = Rx xor Ry
		OR = 0x0F,		// Rx, Ry		Rx = Rx bit_or Ry
		IN = 0x10,		// Rx				чтение байта из файла в Rx и установка флага "конец файла", если он был достигнут
		OUT = 0x11,		// Rx				запись байта из Rx в файл / вывод на экран
		CMPZ = 0x12,	// Rx				сравнение регистра с нулем, выставляет флаг `ZERO`
	};
}


VirtualMachine::VirtualMachine(std::string_view data, std::istream& input, std::ostream& output)
	: Hardware(data)
{
	const uint8_t pc_inc = 2;

	SetOpcodeFunc(INC, [](virtual_space& vs, uint8_t data) 
		{ 
			++vs.regs[GetLowPart(data)]; 
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(DEC, [](virtual_space& vs, uint8_t data)
		{
			--vs.regs[GetLowPart(data)];
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(MOV, [](virtual_space& vs, uint8_t data)
		{
			vs.regs[GetLowPart(data)] = vs.regs[GetHightPart(data)];
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(MOVC, [](virtual_space& vs, uint8_t data)
		{
			vs.regs[0] = data;
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(LSL, [](virtual_space& vs, uint8_t data)
		{
			vs.regs[GetLowPart(data)] <<= 1;
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(JMP, [](virtual_space& vs, uint8_t data)
		{
			vs.program_counter += data;
		});

	SetOpcodeFunc(JZ, [](virtual_space& vs, uint8_t data)
		{
			if (!vs.flags[virtual_space::ZERO])
				vs.program_counter += data;
			else
				vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(JNZ, [](virtual_space& vs, uint8_t data)
		{
			if (vs.flags[virtual_space::ZERO])
				vs.program_counter += data;
			else
				vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(JFE, [](virtual_space& vs, uint8_t data)
		{
			if (vs.flags[virtual_space::FEOF])
				vs.program_counter += data;
			else
				vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(RET, [](virtual_space& vs, uint8_t data)
		{
			vs.power_off = true;
		});

	SetOpcodeFunc(ADD, [](virtual_space& vs, uint8_t data)
		{
			vs.regs[GetLowPart(data)] += vs.regs[GetHightPart(data)];
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(SUB, [](virtual_space& vs, uint8_t data)
		{
			vs.regs[GetLowPart(data)] -= vs.regs[GetHightPart(data)];
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(XOR, [](virtual_space& vs, uint8_t data)
		{
			vs.regs[GetLowPart(data)] ^= vs.regs[GetHightPart(data)];
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(OR, [](virtual_space& vs, uint8_t data)
		{
			vs.regs[GetLowPart(data)] |= vs.regs[GetHightPart(data)];
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(IN, [&input](virtual_space& vs, uint8_t data)
		{
			if (input.eof())
				vs.flags[virtual_space::FEOF] = 1;
			else
				input >> vs.regs[GetLowPart(data)];
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(OUT, [&output](virtual_space& vs, uint8_t data)
		{
			output << vs.regs[GetLowPart(data)];
			vs.program_counter += pc_inc;
		});

	SetOpcodeFunc(CMPZ, [](virtual_space& vs, uint8_t data)
		{
			if (vs.regs[GetLowPart(data)] == 0)
				vs.flags[virtual_space::ZERO] = 1;
			vs.program_counter += pc_inc;
		});
}
