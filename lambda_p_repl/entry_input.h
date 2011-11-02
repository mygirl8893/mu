//
//  entry_input.h
//  lambda_p_repl
//
//  Created by Colin LeMahieu on 9/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <lambda_p_repl/routine_input.h>

#include <boost/shared_ptr.hpp>

#include <vector>

namespace lambda_p
{
    namespace core
    {
        class routine;
    }
	namespace binder
	{
		class node;
	}
}
namespace lambda_p_repl
{
    class entry_input
    {
    public:  
		entry_input (std::vector <std::pair <std::wstring, boost::shared_ptr <lambda_p::binder::node>>> injected_parameters);
		void operator () (boost::shared_ptr <lambda_p::lexer::character_stream> in_a, std::wostream & out_a);
        boost::shared_ptr <lambda_p::core::routine> routine;
		lambda_p_repl::routine_input input;
    };
}
