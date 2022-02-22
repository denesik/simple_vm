#include <stdint.h>
#include <array>
#include <bitset>
#include <functional>
#include <string>

//11110000
inline uint8_t GetHightPart(uint8_t val) { return val >> 4; }
inline void SetHightPart(uint8_t val, uint8_t& out) { out |= (val << 4); }

//00001111
inline uint8_t GetLowPart(uint8_t val) { return val & 0xF; }
inline void SetLowPart(uint8_t val, uint8_t& out) { out |= (val & 0xF); }


class Hardware
{
public:
	Hardware(std::string_view data);

	struct virtual_space
	{
		enum Flags
		{
			FEOF,
			ZERO,

			FLAGS_COUNT,
		};

		std::array<uint8_t, 16> regs;
		uint8_t program_counter = 0;
		std::bitset<Flags::FLAGS_COUNT> flags;

		bool power_off = false;
	};

	using OpcodeFunction = std::function<void(virtual_space& vs, uint8_t data)>;

	void SetOpcodeFunc(uint8_t index, const OpcodeFunction &func);

	bool Tick();

protected:
	std::array<OpcodeFunction, 1 << (sizeof(uint8_t) * 8)> mOpcodeFunctions;

	std::array<uint8_t, 256> mProgramMemory;
	virtual_space mVirtualSpace;
};


