#include "data.h"

#include <lambda_p_io/analyzer/expression.h>
#include <lambda_p_io/analyzer/routine.h>
#include <lambda_p_io/analyzer/analyzer.h>
#include <lambda_p_io/ast/expression.h>
#include <lambda_p/errors/error_target.h>
#include <lambda_p_io/ast/identifier.h>
#include <lambda_p/call.h>
#include <lambda_p_script/nodes/data.h>

#include <sstream>

void lambda_p_script::extensions::data::operator () (lambda_p_io::analyzer::expression & expression_a)
{	
	auto data_position (expression_a.position + 1);
	expression_a.position = data_position;
	if (expression_a.expression_m->values.size () > data_position)
	{
		auto data (boost::dynamic_pointer_cast <lambda_p_io::ast::identifier> (expression_a.expression_m->values [data_position]));
		if (data.get () != nullptr)
		{
			expression_a.result->dependencies.push_back (boost::shared_ptr <lambda_p_script::nodes::data> (new lambda_p_script::nodes::data (data->string)));
		}
		else
		{
			(*expression_a.routine.analyzer.errors) (L"Data extension requires its argument to be an identifier");
		}
	}
	else
	{
		(*expression_a.routine.analyzer.errors) (L"Data extension requires one argument");
	}
}