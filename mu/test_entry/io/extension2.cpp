#include "extension2.h"

#include <mu/io/analyzer/expression.h>
#include <mu/core/expression.h>

void mu::io_test::extension2::operator () (mu::core::errors::error_target * errors_a, mu::io::analyzer::expression & expression_a)
{
	expression_a.self->dependencies.push_back (nullptr);
}