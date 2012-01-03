#include "routine.h"

#include <lambda_p/core/pipe.h>
#include <lambda_p/core/entry.h>
#include <lambda_p/errors/error_list.h>

lambda_p::core::routine::routine ()
	: output (new lambda_p::core::pipe),
	errors (new lambda_p::errors::error_list),
	input (new lambda_p::core::entry)
{
}

lambda_p::core::routine::routine (boost::shared_ptr <lambda_p::core::entry> input_a, boost::shared_ptr <lambda_p::core::pipe> output_a, boost::shared_ptr <lambda_p::errors::error_list> errors_a)
	: output (output_a),
	errors (errors_a),
	input (input_a)
{
	assert (output.get () != nullptr);
	assert (errors.get () != nullptr);
	assert (input.get () != nullptr);
}
