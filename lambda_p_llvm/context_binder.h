#pragma once

#include <lambda_p/binder/node_binder.h>

namespace lambda_p_llvm
{
	class context_binder : public ::lambda_p::binder::node_binder
	{
	public:
		context_binder(void);
		~context_binder(void);
		void bind (::lambda_p::core::statement * statement, ::lambda_p::binder::routine_instances & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems);
		::std::wstring binder_name ();
	};
}
