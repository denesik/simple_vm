#include "Hardware.h"

#include <istream>
#include <ostream>

class VirtualMachine : public Hardware
{
public:
	VirtualMachine(std::string_view data, std::istream& input, std::ostream& output);

};


