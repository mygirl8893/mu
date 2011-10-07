#pragma once

#include <boost/shared_ptr.hpp>

namespace lambda_p
{
	namespace binder
	{
		class package;
	}
}
namespace lambda_p_llvm
{
    class generation_context;
	class api
	{
	public:
		api (::lambda_p_llvm::generation_context & context_a);
		~api (void);
		::boost::shared_ptr < ::lambda_p::binder::package> package;
        ::lambda_p_llvm::generation_context & context;
	};
}
