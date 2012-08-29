#include <mu/llvm_/module_add_function.h>

#include <mu/llvm_/function_node.h>
#include <mu/llvm_/module_node.h>
#include <mu/core/check.h>
#include <mu/script/context.h>

#include <llvm/Module.h>

#include <gc_cpp.h>

bool mu::llvm_::module::add_function::operator () (mu::script::context & context_a)
{
	bool result (mu::core::check <mu::llvm_::module::node, mu::llvm_::function::node> (context_a));
	if (result)
	{
		auto one (static_cast <mu::llvm_::module::node *> (context_a.parameters (0)));
		auto two (static_cast <mu::llvm_::function::node *> (context_a.parameters (1)));
		one->module->getFunctionList ().push_back (two->function ());
	}
	return result;
}

mu::string mu::llvm_::module::add_function::name ()
{
	return mu::string (U"mu::llvm_::module::add_function");
}