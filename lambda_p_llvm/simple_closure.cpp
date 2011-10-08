#include "simple_closure.h"

#include <lambda_p_llvm/generation_context.h>
#include <lambda_p_llvm/value.h>

#include <llvm/Instructions.h>
#include <llvm/BasicBlock.h>
#include <llvm/Function.h>

#include <boost/shared_ptr.hpp>

lambda_p_llvm::simple_closure::simple_closure (::lambda_p_llvm::generation_context & context_a, ::llvm::Function * function_a, ::std::vector < ::llvm::Value *> arguments_a)
	: context (context_a),
	function (function_a),
	arguments (arguments_a)
{
}

::llvm::Type const * lambda_p_llvm::simple_closure::type ()
{
	return function->getType ();
}

::llvm::Value * lambda_p_llvm::simple_closure::operator () ()
{	
	::llvm::CallInst * call (::llvm::CallInst::Create (function, arguments.begin (), arguments.end ()));
	context.block->getInstList ().push_back (call);
	return call;
}