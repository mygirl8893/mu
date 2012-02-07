#include "void_c.h"

#include <lambda_p/errors/error_target.h>
#include <lambda_p_llvm/instruction/node.h>
#include <lambda_p_llvm/apint/node.h>
#include <lambda_p_llvm/void_type/node.h>
#include <lambda_p_llvm/context/node.h>

#include <llvm/Value.h>
#include <llvm/DerivedTypes.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>

#include <sstream>

#include <boost/make_shared.hpp>

void lambda_p_llvm::types::void_c::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters_a, std::vector <boost::shared_ptr <lambda_p::node>> & results_a)
{
	auto one (boost::dynamic_pointer_cast <lambda_p_llvm::context::node> (parameters_a [0]));
	if (one.get () != nullptr)
	{
		results_a.push_back (boost::make_shared <lambda_p_llvm::void_type::node> (one));
	}
}

size_t lambda_p_llvm::types::void_c::count ()
{
	return 1;
}