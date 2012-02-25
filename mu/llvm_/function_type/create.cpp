#include "create.h"

#include <mu/core/errors/error_target.h>
#include <mu/script/values/operation.h>
#include <mu/llvm_/type/node.h>
#include <mu/llvm_/context/node.h>
#include <mu/llvm_/function_type/node.h>
#include <mu/llvm_/set_type/node.h>
#include <mu/llvm_/void_type/node.h>

#include <llvm/DerivedTypes.h>
#include <llvm/ADT/ArrayRef.h>

#include <sstream>

#include <boost/make_shared.hpp>

void mu::llvm_::function_type::create::operator () (boost::shared_ptr <mu::core::errors::error_target> errors_a, mu::core::segment <boost::shared_ptr <mu::core::node>> parameters, std::vector <boost::shared_ptr <mu::core::node>> & results_a)
{
	auto context (boost::dynamic_pointer_cast <mu::llvm_::context::node> (parameters [0]));
	auto one (boost::dynamic_pointer_cast <mu::script::values::operation> (parameters [1]));
	auto two (boost::dynamic_pointer_cast <mu::script::values::operation> (parameters [2]));
	if (one.get () != nullptr)
	{
		if (two.get () != nullptr)
		{
			std::vector <boost::shared_ptr <mu::llvm_::type::node>> arguments;
			std::vector <boost::shared_ptr <mu::llvm_::type::node>> results;
			for (auto i (one->values.begin ()), j (one->values.end ()); i != j; ++i)
			{
				auto type (boost::dynamic_pointer_cast <mu::llvm_::type::node> (*i));
				if (type.get () != nullptr)
				{
					arguments.push_back (type);
				}
				else
				{
					std::wstringstream message;
					message << L"Expecting type, have: ";
					message << (*i)->name ();
					(*errors_a) (message.str ());
				}
			}
			for (auto i (two->values.begin ()), j (two->values.end ()); i != j; ++i)
			{
				auto type (boost::dynamic_pointer_cast <mu::llvm_::type::node> (*i));
				if (type.get () != nullptr)
				{
					results.push_back (type);
				}
				else
				{
					std::wstringstream message;
					message << L"Expecting type, have: ";
					message << (*i)->name ();
					(*errors_a) (message.str ());
				}
			}
			if (results.size () == 0)
			{				
				results_a.push_back (boost::make_shared <mu::llvm_::function_type::node> (context, arguments, boost::make_shared <mu::llvm_::void_type::node> (context)));
			}
			else if (results.size () == 1)
			{
				results_a.push_back (boost::make_shared <mu::llvm_::function_type::node> (context, arguments, results [0]));
			}
			else
			{
				results_a.push_back (boost::make_shared <mu::llvm_::function_type::node> (context, arguments, boost::make_shared <mu::llvm_::set_type::node> (context, results)));
			}
		}
		else
		{
			invalid_type (errors_a, parameters [0], 0);
		}
	}
	else
	{
			invalid_type (errors_a, parameters [1], 1);
	}
}

size_t mu::llvm_::function_type::create::count ()
{
	return 3;
}

std::wstring mu::llvm_::function_type::create::name ()
{
	return std::wstring (L"mu::llvm_::function_type::create");
}