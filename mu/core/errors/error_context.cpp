#include <mu/core/errors/error_context.h>

mu::core::errors::error_context::error_context (boost::shared_ptr <mu::core::errors::error_target> target_a, mu::core::context context_a)
	: target (target_a),
	context (context_a)
{
}

void mu::core::errors::error_context::operator () (boost::shared_ptr <mu::core::errors::error> error)
{
	(*target) (error);
}

bool mu::core::errors::error_context::operator () ()
{
	return (*target) ();
}

void mu::core::errors::error_context::print (std::wostream & target_a)
{
    target->print (target_a);
}