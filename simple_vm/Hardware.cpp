#include "Hardware.h"

#include <stdexcept>

Hardware::Hardware(std::string_view data)
{
	if (data.size() > mProgramMemory.size())
		throw std::runtime_error("[vm] Large program size.");

	std::copy(data.begin(), data.end(), mProgramMemory.begin());
}

void Hardware::SetOpcodeFunc(uint8_t index, const OpcodeFunction& func)
{
	mOpcodeFunctions[index] = func;
}

bool Hardware::Tick()
{
	if (mVirtualSpace.program_counter >= mProgramMemory.size() - 1)
		throw std::runtime_error("[vm] Out of program memory.");

	auto code = mProgramMemory[mVirtualSpace.program_counter];
	auto data = mProgramMemory[mVirtualSpace.program_counter + 1];
	if (!mOpcodeFunctions[code])
		throw std::runtime_error("[vm] Unknown opcode.");

	mOpcodeFunctions[code](mVirtualSpace, data);

	if (mVirtualSpace.power_off)
		return false;
	return true;
}
