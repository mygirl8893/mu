#include "tests.h"

#include <lambda_p_script_test\segment.h>
#include <lambda_p_script_test/routine.h>
#include <lambda_p_script_test/call.h>
#include <lambda_p_script_test/constant.h>
#include <lambda_p_script_test/expression.h>
#include <lambda_p_script_test/reference.h>
#include <lambda_p_script_test/integer/equal.h>
#include <lambda_p_script_test/closure/operation.h>
#include <lambda_p_script_test/package/create.h>
#include <lambda_p_script_test/package/get.h>
#include <lambda_p_script_test/closure/apply.h>
#include <lambda_p_script_test/times/operation.h>

void lambda_p_script_test::tests::run ()
{
	{
		segment test;
		test.run ();
	}
	{
		reference test;
		test.run ();
	}
	{
		expression test;
		test.run ();
	}
	{
		constant test;
		test.run ();
	}
	{
		call test;
		test.run ();
	}
	{
		routine test;
		test.run ();
	}
	{
		lambda_p_script_test::integer::equal test;
		test.run ();
	}
	{
		lambda_p_script_test::closure::operation test;
		test.run ();
	}
	{
		lambda_p_script_test::package::create test;
		test.run ();
	}
	{
		lambda_p_script_test::package::get test;
		test.run ();
	}
	{
		lambda_p_script_test::closure::apply test;
		test.run ();
	}
	{
		lambda_p_script_test::times::operation test;
		test.run ();
	}
}