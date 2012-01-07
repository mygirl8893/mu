#pragma once

#include <lambda_p/expression.h>

namespace lambda_p
{
	class parameters : public lambda_p::expression
	{
	public:
		parameters ();
		void operator () (lambda_p::visitor * visitor_a) override;
		size_t count;
	};
}

