#include "analyzer.h"

#include <lambda_p_io/ast/expression.h>
#include <lambda_p_io/ast/identifier.h>
#include <lambda_p_io/analyzer/routine.h>
#include <lambda_p/errors/string_error.h>
#include <lambda_p/errors/error_target.h>
#include <lambda_p_io/analyzer/extensions/extensions.h>

#include <sstream>

lambda_p_io::analyzer::analyzer::analyzer (boost::function <void (boost::shared_ptr <lambda_p::cluster>)> target_a, boost::shared_ptr <lambda_p::errors::error_target> errors_a)
	: target (target_a),
	errors (errors_a),
	extensions (new lambda_p_io::analyzer::extensions::extensions),
	cluster (new lambda_p::cluster)
{
}

lambda_p_io::analyzer::analyzer::analyzer (boost::function <void (boost::shared_ptr <lambda_p::cluster>)> target_a, boost::shared_ptr <lambda_p::errors::error_target> errors_a, boost::shared_ptr <lambda_p_io::analyzer::extensions::extensions> extensions_a)
	: target (target_a),
	errors (errors_a),
	extensions (extensions_a),
	cluster (new lambda_p::cluster)
{
}

void lambda_p_io::analyzer::analyzer::input (boost::shared_ptr <lambda_p_io::ast::node> node_a)
{
	(*node_a) (this);
}

void lambda_p_io::analyzer::analyzer::operator () (lambda_p_io::ast::parameters * parameters_a)
{
	(*errors) (L"Analyzer not expecting parameters");
}

void lambda_p_io::analyzer::analyzer::operator () (lambda_p_io::ast::expression * expression_a)
{
	lambda_p_io::analyzer::routine (*this, expression_a);
}

void lambda_p_io::analyzer::analyzer::operator () (lambda_p_io::ast::identifier * identifier_a)
{
	(*errors) (L"Analyzer not expecting identifiers");
}

void lambda_p_io::analyzer::analyzer::operator () (lambda_p_io::ast::end * end_a)
{	
	if (unresolved.empty ())
	{
		if (!(*errors) ())
		{
			target (cluster);
		}
		else
		{
			(*errors) (L"Not generating cluster due to other errors");
		}
	}
	else
	{
		for (auto i (unresolved.begin ()), j (unresolved.end ()); i != j; ++i)
		{
			std::wstringstream message;
			message << L"Unresolved identifier: ";
			message << i->first;
			(*errors) (message.str (), i->second.second);
		}
	}
}

void lambda_p_io::analyzer::analyzer::operator () (std::wstring name_a, lambda_p::context context_a)
{
	used_names.insert (std::multimap <std::wstring, lambda_p::context>::value_type (name_a, context_a));
}

void lambda_p_io::analyzer::analyzer::operator () (std::wstring name_a, boost::shared_ptr <lambda_p::routine> routine_a, lambda_p::context context_a)
{
	auto existing (used_names.find (name_a));
	if (existing == used_names.end ())
	{
		(*this) (name_a, context_a);
		assert (cluster->routines.find (name_a) == cluster->routines.end ());
		cluster->routines [name_a] = routine_a;
	}
	else
	{
		for (; existing != used_names.end () && existing->first == name_a; ++existing)
		{
			std::wstringstream message;
			message << L"Routine name: ";
			message << name_a;
			message << L" collides with usage at: ";
			message << existing->second.string ();
		}
	}
}