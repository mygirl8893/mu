#include "expression.h"

#include <lambda_p_script/context.h>

lambda_p_script::expression::expression (size_t index_a)
	: index (index_a)
{
}

void lambda_p_script::expression::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p_script::context & context, std::vector <boost::shared_ptr <lambda_p::node>> & target)
{
	assert (context.nodes.size () > index);
	std::vector <boost::shared_ptr <lambda_p::node>> & source (context.nodes [index]);
	target.insert (target.end (), source.begin (), source.end ());
}