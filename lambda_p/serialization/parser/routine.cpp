#include <lambda_p/serialization/parser/routine.h>

lambda_p::serialization::parser::routine::routine(void)
{
}

lambda_p::serialization::parser::routine::~routine(void)
{
}

::lambda_p::serialization::parser::state_id lambda_p::serialization::parser::routine::state_type ()
{
	return ::lambda_p::serialization::parser::state_routine;
}