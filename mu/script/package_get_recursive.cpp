#include <mu/script/package_get_recursive.h>

#include <mu/core/errors/error_target.h>
#include <mu/script/package_node.h>
#include <mu/script/string_node.h>
#include <mu/script/package_get.h>
#include <mu/script/context.h>

#include <sstream>

#include <gc_cpp.h>

bool mu::script::package::get_recursive::operator () (mu::script::context & context_a)
{
	bool result (true);
	if (context_a.parameters_size () > 0)
	{
		auto node (context_a.parameters (0));
		bool good (true);
		auto i (context_a.parameters_begin () + 1);
		auto j (context_a.parameters_end ());
		for (; i != j && good; ++i)
		{
			context_a.push (new (GC) mu::script::package::get);
			context_a.push (node);
			context_a.push (*i);
			result = context_a ();
			if (result)
			{
				node = context_a.working (0);
				context_a.drop ();
			}
			else
			{
				good = false;
				context_a.errors (U"Get operation did not return result");
			}
		}
		context_a.push (node);
	}
	else
	{
		mu::stringstream message;
		message << L"Operation: ";
		message << name ();
		message << L" requires at least one argument";
		context_a.errors (message.str ());
		result = false;
	}
	return result;
}

mu::string mu::script::package::get_recursive::name ()
{
	return mu::string (U"mu::script::package::get_recursive");
}