#include "single.h"

#include <lambda_p_io/tokens/divider.h>
#include <lambda_p_io/tokens/identifier.h>
#include <lambda_p_io/parser/parser.h>
#include <lambda_p_io/parser/error.h>
#include <lambda_p_io/ast/expression.h>
#include <lambda_p_io/tokens/left_square.h>
#include <lambda_p_io/tokens/stream_end.h>
#include <lambda_p_io/parser/full.h>

#include <sstream>

lambda_p_io::parser::single::single (lambda_p_io::parser::parser & parser_a, boost::function <void (boost::shared_ptr <lambda_p_io::ast::expression>)> target_a, std::vector <boost::shared_ptr <lambda_p_io::ast::node>> values_a)
	: parser (parser_a),
	target (target_a),
	values (values_a)
{
}

void lambda_p_io::parser::single::operator () (lambda_p_io::tokens::divider * token)
{
	parser.state.pop ();
	parser.state.push (boost::shared_ptr <lambda_p_io::tokens::visitor> (new lambda_p_io::parser::full (parser, target, values, names)));
}

void lambda_p_io::parser::single::operator () (lambda_p_io::tokens::identifier * token)
{
	names.push_back (token->string);
}

void lambda_p_io::parser::single::operator () (lambda_p_io::tokens::left_square * token)
{
	unexpected_token (token);
}

void lambda_p_io::parser::single::operator () (lambda_p_io::tokens::right_square * token)
{
	parser.state.pop ();
	target (boost::shared_ptr <lambda_p_io::ast::expression> (new lambda_p_io::ast::expression (values, names)));
}

void lambda_p_io::parser::single::operator () (lambda_p_io::tokens::stream_end * token)
{
	unexpected_token (token);
}

void lambda_p_io::parser::single::unexpected_token (lambda_p_io::tokens::token * token)
{
    std::wstringstream message;
	message << L"Unexpected token while parsing individual names: ";
	message << token->token_name ();
    parser.state.push (boost::shared_ptr <lambda_p_io::tokens::visitor> (new lambda_p_io::parser::error (message.str ())));
}