#include <mu/script/runtime/trace.h>

#include <mu/core/node.h>
#include <mu/script/context.h>

mu::script::runtime::trace::trace (mu::script::context & context_a, mu::core::node * node_a)
	: context (context_a)
{
	context_a.function_stack.push_back (&typeid (*node_a));
}

mu::script::runtime::trace::~trace ()
{
	context.function_stack.pop_back ();
}