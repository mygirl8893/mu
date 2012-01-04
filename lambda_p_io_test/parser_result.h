#pragma once

#include <boost/shared_ptr.hpp>

#include <vector>

namespace lambda_p_io
{
	namespace ast
	{
		class expression;
	}
}
namespace lambda_p_io_test
{
	class parser_result
	{
	public:
		void operator () (boost::shared_ptr <lambda_p_io::ast::expression> result_a);
		std::vector <boost::shared_ptr <lambda_p_io::ast::expression>> results;
	};
}

