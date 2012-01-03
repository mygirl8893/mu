#pragma once

#include <boost/shared_ptr.hpp>

namespace lambda_p
{
	namespace errors
	{
		class error_list;
	}
	namespace core
	{
		class entry;
		class pipe;
		class routine
		{
		public:
			routine ();
			routine (boost::shared_ptr <lambda_p::core::entry> input_a,	boost::shared_ptr <lambda_p::core::pipe> output_a, boost::shared_ptr <lambda_p::errors::error_list> errors_a);
			boost::shared_ptr <lambda_p::core::entry> input;
			boost::shared_ptr <lambda_p::core::pipe> output;
			boost::shared_ptr <lambda_p::errors::error_list> errors;
		};
	}
}

