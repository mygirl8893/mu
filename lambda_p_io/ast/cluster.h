#pragma once

#include <lambda_p/node.h>

#include <vector>

#include <boost/shared_ptr.hpp>

namespace lambda_p_io
{
	namespace ast
	{
		class expression;
		class cluster : public lambda_p::node
		{
		public:
			std::vector <boost::shared_ptr <lambda_p_io::ast::expression>> expressions;
		};
	}
}
