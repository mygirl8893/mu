#include "synthesizer.h"

#include <lambda_p_script_io/synthesizer.h>
#include <lambda_p/errors/error_list.h>
#include <lambda_p_script_io_test/synthesizer_results.h>
#include <lambda_p/routine.h>
#include <lambda_p/call.h>

#include <boost/bind.hpp>

void lambda_p_script_io_test::synthesizer::run ()
{
	run_1 ();
}

void lambda_p_script_io_test::synthesizer::run_1 ()
{
	auto errors (boost::shared_ptr <lambda_p::errors::error_list> (new lambda_p::errors::error_list));
	lambda_p_script_test::synthesizer_results results;
	lambda_p_script_io::synthesizer synthesizer (boost::bind (&lambda_p_script_test::synthesizer_results::operator(), &results, _1), errors);
	auto routine (boost::shared_ptr <lambda_p::routine> (new lambda_p::routine));
	routine->call = boost::shared_ptr <lambda_p::call> (new lambda_p::call);
	synthesizer (routine);
	assert (errors->errors.empty ());
	assert (results.routines.size () == 1);
}