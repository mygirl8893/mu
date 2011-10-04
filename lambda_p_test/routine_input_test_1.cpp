#include "routine_input_test_1.h"

#include <lambda_p_repl/routine_input.h>
#include <lambda_p_repl/wistream_input.h>

#include <sstream>

lambda_p_test::routine_input_test_1::routine_input_test_1(void)
{
}

lambda_p_test::routine_input_test_1::~routine_input_test_1(void)
{
}

void lambda_p_test::routine_input_test_1::run ()
{
	run_1 ();
}

void lambda_p_test::routine_input_test_1::run_1 ()
{
	::std::wstringstream input;
	input << "environment = ; environment constant_int = umber; #;";
	::lambda_p_repl::wistream_input input2 (input);
	::lambda_p_repl::routine_input routine;
	routine (input2);
}