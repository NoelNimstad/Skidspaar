#include "debug.hpp"

namespace Spår::Debug
{
void printMessage(const std::string &from, const std::string &message)
{
	size_t length = from.length();
	size_t tabs = tabAmount - ((length + 2 + TAB_SIZE - 1) / TAB_SIZE);
	
	if(tabs > tabAmount)
	{
		std::cout << "[Debug::printMessage] Please increase tabAmount!\n";
		return;
	}

	std::cout << "[" << from << "]";
	for(size_t i = 0; i < tabs; i++)
	{
		std::cout << "\t";
	}
	std::cout << message << "\n";
}

void printMessage(const std::string &message)
{
	size_t tabs = tabAmount - ((3 + TAB_SIZE - 1) / TAB_SIZE);
	if(tabs > tabAmount)
	{
		std::cout << "[Debug::printMessage] Please increase tabAmount!\n";
		return;
	}

	std::cout << "[*]";
	for(size_t i = 0; i < tabs; i++)
	{
		std::cout << "\t";
	}
	std::cout << message << "\n";
}

void printError(const std::string &from, const std::string &message)
{
	size_t length = from.length();
	size_t tabs = tabAmount - ((length + 2 + TAB_SIZE - 1) / TAB_SIZE);
	
	if(tabs > tabAmount)
	{
		std::cerr << "{Debug::printError} Please increase tabAmount!\n";
		return;
	}

	std::cerr << "{" << from << "}";
	for(size_t i = 0; i < tabs; i++)
	{
		std::cerr << "\t";
	}
	std::cerr << message << "\n";
}
}