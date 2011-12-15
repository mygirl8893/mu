#include "tests.h"

#include <lambda_p_serialization_test/simple_compile_test_1.h>
#include <lambda_p_serialization_test/wistream_input_test.h>
#include <lambda_p_serialization_test/istream_input_test.h>
#include <lambda_p_serialization_test/routine_builder_test.h>

void lambda_p_serialization_test::tests::run ()
{
	{
		simple_compile_test_1 test;
		test.run ();
	}
	{
		wistream_input_test test;
		test.run ();
	}
	{
		istream_input_test test;
		test.run ();
	}
}