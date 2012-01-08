#pragma once

#include <lambda_p_script/runtime/node.h>

namespace lambda_p_script
{
	namespace runtime
	{
		class reference : public lambda_p_script::runtime::node
		{
		public:
			reference (size_t expression_a, size_t index_a);
			void operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, std::vector <boost::shared_ptr <lambda_p::node>> & target) override;
			size_t expression;
			size_t index;
		};
	}
}

