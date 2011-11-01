//
//  entry_input.cpp
//  lambda_p_repl
//
//  Created by Colin LeMahieu on 9/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <lambda_p_repl/entry_input.h>
#include <lambda_p_repl/routine_input.h>
#include <lambda_p/errors/error.h>
#include <lambda_p/core/routine.h>

#include <sstream>
#include <iostream>

lambda_p_repl::entry_input::entry_input (std::vector <std::pair <std::wstring, boost::shared_ptr <lambda_p::binder::node>>> injected_parameters)
	: input (injected_parameters)
{
}

void lambda_p_repl::entry_input::operator () (boost::shared_ptr <lambda_p_repl::character_stream> in, std::wostream & out)
{
	out << L"lp> \n";
	std::wstring environment (L";;\n");
	input (environment);
	out << environment;
	input (in);
	if (input.error ())
	{
		out << "Lexing error:\n";
		std::wstring message;
		input.error_message (message);
		out << message;
		out << '\n';
	}
	else if (input.routines.routines->size () < 1)
	{
		out << L"Input reached end of stream\n";
	}
	else
	{
		out << L">>\n";
		routine = (*input.routines.routines)[0];
        std::vector < lambda_p::errors::error *> errors;
        routine->validate (errors);
        if (!errors.empty ())
        {
            out << "Validation error:\n";
            for (std::vector <lambda_p::errors::error *>::iterator i = errors.begin (); i != errors.end (); ++i)
            {
                lambda_p::errors::error * error (*i);
                error->string (out);
                out << '\n';
            }
            routine.reset ();
        }
	}
}