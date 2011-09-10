#include <lambda_p/parser/reference_position.h>

lambda_p::parser::reference_position::reference_position ()
	: statement (-1),
	argument (-1)
{
}

lambda_p::parser::reference_position::reference_position (size_t statement_a, size_t argument_a)
	:statement (statement_a),
	argument (argument_a)
{
}

lambda_p::parser::reference_position::~reference_position(void)
{
}