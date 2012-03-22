#include <mu/script/check.h>

#include <mu/core/errors/error_target.h>
#include <sstream>
#include <mu/core/node.h>

bool mu::script::check_count (mu::script::context & context_a, size_t expected)
{
	bool result (context_a.parameters_size () == expected);
	if (!result)
	{
		std::wstringstream message;
		message << L"In operation: ";
		//message << name ();
		message << L" expecting: ";
		message << expected;
		message << L" arguments, have: ";
		message << context_a.parameters_size ();
		context_a.errors (message.str ());
	}
	return result;
}

void mu::script::invalid_type (mu::script::context & context_a, boost::shared_ptr <mu::core::node> node_a, std::type_info const & expected, size_t position)
{
	std::wstringstream message;
	message << L"In operation: ";
	//message << name ();
	message << L" invalid node type:";
	message << (typeid (*node_a)).name ();
	message << L" at position: ";
	message << position;
	message << L" expecting: ";
	message << expected.name ();
	context_a.errors (message.str ());
}