#include "expression.h"

#include <lambda_p_serialization/ast/expression.h>
#include <lambda_p_serialization/ast/identifier.h>
#include <lambda_p_serialization/analyzer/routine.h>
#include <lambda_p/core/connection.h>
#include <lambda_p/core/gather.h>
#include <lambda_p/core/tee.h>
#include <lambda_p/core/call.h>
#include <lambda_p_serialization/analyzer/declaration.h>

lambda_p_serialization::analyzer::expression::expression (lambda_p_serialization::analyzer::routine & routine_a, lambda_p_serialization::ast::expression * expression_a, boost::shared_ptr <lambda_p::core::target> target_a)
	: routine (routine_a),
	target (target_a),
	position (0),
	tee (new lambda_p::core::tee),
	call (new lambda_p::core::call (tee, routine_a.errors)),
	gather (new lambda_p::core::gather (call))
{
	for (auto i (expression_a->values.begin ()), j (expression_a->values.end ()); i != j; ++i)
	{
		(*(*i)) (this);
		++position;
		gather->increment ();
	}
}

void lambda_p_serialization::analyzer::expression::operator () (lambda_p_serialization::ast::expression * expression_a)
{
	auto connection (boost::shared_ptr <lambda_p::core::target> (new lambda_p::core::connection (gather, position)));
	lambda_p_serialization::analyzer::expression expression (routine, expression_a, connection);
}

void lambda_p_serialization::analyzer::expression::operator () (lambda_p_serialization::ast::identifier * identifier_a)
{
	auto connection (boost::shared_ptr <lambda_p::core::target> (new lambda_p::core::connection (gather, position)));
	auto existing (routine.declarations.find (identifier_a->string));
	if (existing != routine.declarations.end ())
	{
		(*existing->second) (connection);
	}
	else
	{
		routine.unresolved.insert (std::multimap <std::wstring, boost::shared_ptr <lambda_p::core::target>>::value_type (identifier_a->string, connection));
	}
}