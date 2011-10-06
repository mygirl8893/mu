#pragma once

#include <lambda_p/binder/node_binder.h>

namespace lambda_p
{
	namespace binder
	{
		class routine_instances;
		class null_binder : public node_binder
		{
		public:
			null_binder(void);
			~null_binder(void);
			virtual void bind (::lambda_p::core::statement * statement, ::lambda_p::binder::routine_instances & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems);
			::std::wstring binder_name ();
		};
	}
}

