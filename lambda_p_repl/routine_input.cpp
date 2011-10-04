#include "routine_input.h"

#include <boost/bind.hpp>

lambda_p_repl::routine_input::routine_input (void)
	: parser (routines),
	lexer (::boost::bind (&::lambda_p_repl::routine_input::token_sink, this, _1))
{
}

lambda_p_repl::routine_input::~routine_input (void)
{
}

void lambda_p_repl::routine_input::operator () (::boost::function <wchar_t ()> input_stream)
{	
	wchar_t last_char (L' ');
	while (routines.routines->empty () && last_char != '\uffff' && !lexer.error () && !parser.error ())
	{
		::std::wstring line;
		while (last_char != '\uffff' && last_char != L'\n')
		{
			last_char = input_stream ();
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

void lambda_p_repl::routine_input::operator() (::std::wstring & string)
{
	for (::std::wstring::const_iterator i = string.begin (); i != string.end (); ++i)
	{
		operator () (*i);
	}
}

void lambda_p_repl::routine_input::error_message (::std::wstring & message)
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

void lambda_p_repl::routine_input::token_sink (::lambda_p::tokens::token * token)
{
	parser (token);
}