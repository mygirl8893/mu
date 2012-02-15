#include "set_linkage.h"

#include <lambda_p_llvm/global_value/node.h>
#include <lambda_p_llvm/linkage_types/node.h>

void lambda_p_llvm::global_value::set_linkage::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters, std::vector <boost::shared_ptr <lambda_p::node>> & results)
{
	auto one (boost::dynamic_pointer_cast <lambda_p_llvm::global_value::node> (parameters [0]));
	auto two (boost::dynamic_pointer_cast <lambda_p_llvm::linkage_types::node> (parameters [1]));
	if (one.get () != nullptr)
	{
		if (two.get () != nullptr)
		{
			one->global_value ()->setLinkage (two->type);
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

size_t lambda_p_llvm::global_value::set_linkage::count ()
{
	return 2;
}