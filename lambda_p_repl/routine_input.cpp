#include "routine_input.h"

#include <lambda_p/lexer/character_stream.h>
#include <lambda_p/binder/list_binder.h>

#include <boost/bind.hpp>

lambda_p_repl::routine_input::routine_input (std::vector <std::pair <std::wstring, boost::shared_ptr <lambda_p::binder::node>>> injected_parameters_a)
	: parser (routines, injected_parameters_a),
	lexer (boost::bind (&lambda_p::parser::parser::operator(), &parser, _1))
{
}

void lambda_p_repl::routine_input::operator () (boost::shared_ptr <lambda_p::lexer::character_stream> input_stream)
{	
	wchar_t last_char (L' ');
	while (routines.routines->empty () && last_char != L'\uffff' && !lexer.error () && !parser.error ())
	{
		std::wstring line;
		while (last_char != L'\uffff' && last_char != L'\n')
		{
			last_char = input_stream->operator() ();
			line.push_back (last_char);
		}
		operator () (line);
		if (last_char == L'\n')
		{
			last_char = L' ';
		}
	}
}

void lambda_p_repl::routine_input::operator () (wchar_t character)
{
	lexer (character);
}

void lambda_p_repl::routine_input::operator() (std::wstring & string)
{
	for (std::wstring::const_iterator i = string.begin (); i != string.end (); ++i)
	{
		operator () (*i);
	}
}

void lambda_p_repl::routine_input::error_message (std::wstring & message)
{
	if (lexer.error ())
	{
		lexer.error_message (message);
	}
	if (parser.error ())
	{
		parser.error_message (message);
	}
}

bool lambda_p_repl::routine_input::error ()
{
	bool result (lexer.error () || parser.error ());
	return result;
}