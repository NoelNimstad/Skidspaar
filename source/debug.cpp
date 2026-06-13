#include "debug.hpp"

/*
	Example

	tabAmmount = 4
	TAB_SIZE = 4
*/
//	[hello world]
//					***
/*
	13 / 4 = 3
*/

namespace Spår::Debug
{
void printMessage(const std::string &from, const std::string &message)
{
	size_t length = from.length() + 2;
	if(from.length() + 2 >= tabAmount * TAB_SIZE)
	{
		std::cout << "{Debug::printMessage} Please increase tabAmount!\n";
		return;
	}

	size_t tabs = tabAmount - length / TAB_SIZE;

	std::cout << "[" << from << "]";
	for(size_t i = 0; i < tabs; i++)
	{
		std::cout << "\t";
	}
	std::cout << message << "\n";
}

void printMessage(const std::string &message)
{
	if(3 >= tabAmount * TAB_SIZE) [[unlikely]]
	{
		std::cout << "{Debug::printMessage} Please increase tabAmount!\n";
		return;
	}

	constexpr size_t tabs = 16;

	std::cout << "[*]";
	for(size_t i = 0; i < tabs; i++)
	{
		std::cout << "\t";
	}
	std::cout << message << "\n";
}

/*
	Yes, I know it is supposed to be `std::cerr` or something
	but unfortunately that doesn't log in my debugging system
*/
std::runtime_error printError(const std::string &from, const std::string &message)
{
	size_t length = from.length() + 2;
	if(from.length() + 2 >= tabAmount * TAB_SIZE)
	{
		std::cout << "{Debug::printError} Please increase tabAmount!\n";
		return std::runtime_error("Please increase tabAmount!");
	}

	size_t tabs = tabAmount - length / TAB_SIZE;

	std::cout << "{" << from << "}";
	for(size_t i = 0; i < tabs; i++)
	{
		std::cout << "\t";
	}
	std::cout << message << "\n";
	
	return std::runtime_error(message);
}
}