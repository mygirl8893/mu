#pragma once

#include <lambda_p_io/analyzer/extension.h>

namespace lambda_p_kernel
{
	namespace extensions
	{
		class adata : public lambda_p_io::analyzer::extension
		{
		public:
			std::pair <size_t, std::vector <boost::shared_ptr <lambda_p::core::node>>> operator () (std::vector <boost::shared_ptr <lambda_p_io::ast::node>> & values_a, size_t position_a, boost::shared_ptr <lambda_p::errors::error_target> errors_a) override;
		};
	}
}

