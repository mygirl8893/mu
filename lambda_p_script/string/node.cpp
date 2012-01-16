#include "node.h"

#include <sstream>

lambda_p_script::string::node::node (std::wstring string_a)
	: string (string_a)
{
}

std::wstring lambda_p_script::string::node::debug ()
{
	std::wstringstream result;
	result << L"\"";
	result << string;
	result << L"\"";
	return result.str ();
}