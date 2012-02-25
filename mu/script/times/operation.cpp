#include "operation.h"

#include <mu/core/errors/error_target.h>
#include <mu/script/integer/node.h>

#include <sstream>

void mu::script::times::operation::perform (boost::shared_ptr <mu::core::errors::error_target> errors_a, mu::core::segment <boost::shared_ptr <mu::core::node>> parameters, std::vector <boost::shared_ptr <mu::core::node>> & results)
{
	if (parameters.size () > 1)
	{
		auto one (boost::dynamic_pointer_cast <mu::script::integer::node> (parameters [0]));
		auto two (boost::dynamic_pointer_cast <mu::script::operation> (parameters [1]));
		if (one.get () != nullptr)
		{
			if (two.get () != nullptr)
			{
				std::vector <boost::shared_ptr <mu::core::node>> results_l (parameters.begin () + 2, parameters.end ());
				for (size_t i (0), j (one->value); i != j; ++i)
				{
					std::vector <boost::shared_ptr <mu::core::node>> arguments;
					arguments.swap (results_l);
					two->perform (errors_a, arguments, results_l);
				}
				results.insert (results.end (), results_l.begin (), results_l.end ());
			}
			else
			{
				invalid_type (errors_a, parameters [1], 1);
			}
		}
		else
		{
			invalid_type (errors_a, parameters [0], 0);
		}
	}
	else
	{
		std::wstringstream message;
		message << name ();
		message << L" must have at least two arguments";
		(*errors_a) (message.str ());
	}
}

std::wstring mu::script::times::operation::name ()
{
	return std::wstring (L"mu::script::times::operation");
}