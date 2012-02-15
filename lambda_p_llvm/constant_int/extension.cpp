#include "extension.h"

#include <lambda_p/errors/error_target.h>
#include <lambda_p_io/analyzer/expression.h>
#include <lambda_p_script/integer/extension.h>
#include <lambda_p_io/ast/expression.h>
#include <lambda_p_io/ast/identifier.h>
#include <lambda_p/expression.h>
#include <lambda_p_llvm/value/node.h>
#include <lambda_p_script/integer/node.h>
#include <lambda_p_script/closure/single.h>
#include <lambda_p_llvm/constant_int/create.h>
#include <lambda_p_llvm/apint/node.h>
#include <lambda_p_llvm/context/node.h>
#include <lambda_p_io/analyzer/extensions/global.h>

#include <sstream>

#include <boost/make_shared.hpp>

lambda_p_llvm::constant_int::extension::extension (boost::shared_ptr <lambda_p_llvm::context::node> context_a)
	: context (context_a)
{
}

void lambda_p_llvm::constant_int::extension::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p_io::analyzer::expression & expression_a)
{
	auto bits_position (expression_a.position + 1);
	auto number_position (expression_a.position + 2);
	if (expression_a.expression_m->values.size () > number_position)
	{
		expression_a.position = number_position;
		auto bits_identifier (boost::dynamic_pointer_cast <lambda_p_io::ast::identifier> (expression_a.expression_m->values [bits_position]));
		if (bits_identifier.get () != nullptr)
		{
			auto number_identifier (boost::dynamic_pointer_cast <lambda_p_io::ast::identifier> (expression_a.expression_m->values [number_position]));
			if (number_identifier.get () != nullptr)
			{
				auto characters (bits_identifier->string.c_str ());
				auto bits (lambda_p_script::integer::core_d (errors_a, characters));
				if (bits.get () != nullptr)
				{
					auto number (lambda_p_script::integer::core (errors_a, number_identifier->string));
					if (number.get () != nullptr)
					{
						auto result (boost::make_shared <lambda_p_script::closure::single> (boost::make_shared <lambda_p_llvm::constant_int::create> ()));
						result->closed.push_back (context);
						result->closed.push_back (boost::make_shared <lambda_p_llvm::apint::node> (new llvm::APInt (64, bits->value)));
						result->closed.push_back (boost::make_shared <lambda_p_llvm::apint::node> (new llvm::APInt (bits->value, number->value)));
						expression_a.self->dependencies.push_back (result);
					}
					else
					{
						std::wstringstream message;
						message << L"Unable to parse number: ";
						message << number;
					}
				}
				else
				{
					std::wstringstream message;
					message << L"Unable to parse bits number: ";
					message << characters;
					(*errors_a) (message.str ());
				}
			}
			else
			{
				(*errors_a) (L"Constant_int requires the second argument to be an identifier");
			}
		}
		else
		{
			(*errors_a) (L"Constant_int requires the first argument to be an identifier");
		}
	}
	else
	{
		(*errors_a) (L"Constant_int extension requires two arguments");
	}
}