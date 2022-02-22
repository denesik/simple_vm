#include "VirtualMachine.h"

namespace
{
	enum : uint8_t
	{
		INC = 0x01,		// Rx				��������� �������� x
		DEC = 0x02,		// Rx				��������� �������� x
		MOV = 0x03,		// Rx, Ry		Rx = Ry �������� ������� Ry � Rx
		MOVC = 0x04,	// "const"	���������� ��������� � R0. 2� ��� �������
		LSL = 0x05,		// Rx				����� �������� Rx �� 1 ��� �����
		LSR = 0x06,		// Rx				����� �������� Rx �� 1 ��� ������
		JMP = 0x07,		// addr			����������� ������� �� �������� "addr"(int8_t).��� ������� �������� ����� 2� ���
		JZ = 0x08,		// addr			�������� ������� �� �������� "addr"(���� ���� `ZERO` == 0).
		JNZ = 0x09,		// addr			�������� ������� �� �������� "addr"(���� ���� `ZERO` != 0).
		JFE = 0x0A,		// addr			�������� �������, ���� ��������� ����� �����(��.IN)
		RET = 0x0B,		//					��������� ������(�������) VM
		ADD = 0x0C,		// Rx, Ry		Rx = Rx + Ry
		SUB = 0x0D,		// Rx, Ry		Rx = Rx - Ry
		XOR = 0x0E,		// Rx, Ry		Rx = Rx xor Ry
		OR = 0x0F,		// Rx, Ry		Rx = Rx bit_or Ry
		IN = 0x10,		// Rx				������ ����� �� ����� � Rx � ��������� ����� "����� �����", ���� �� ��� ���������
		OUT = 0x11,		// Rx				������ ����� �� Rx � ���� / ����� �� �����
		CMPZ = 0x12,	// Rx				��������� �������� � �����, ���������� ���� `ZERO`
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
