#pragma once
#include <sstream>
#include "DelimitedString.h"

std::istream& operator>>(std::istream& is, DelimitedString<','>& out)
{
	std::getline(is, out, ',');
	return is;
}

std::istream& operator>>(std::istream& is, DelimitedString<'-'>& out)
{
	std::getline(is, out, '-');
	return is;
}

std::istream& operator>>(std::istream& is, DelimitedString<'_'>& out)
{
	std::getline(is, out, '_');
	return is;
}

std::istream& operator>>(std::istream& is, DelimitedString<'/'>& out)
{
	std::getline(is, out, '/');
	return is;
}
