#pragma once

#include <lambda_p/binder/node_binder.h>

namespace lambda_p
{
	namespace binder
	{
		class command_list;
	}
}
namespace lambda_p_repl
{
	class echo_binder : public ::lambda_p::binder::node_binder
	{
	public:
		echo_binder (::boost::shared_ptr < ::lambda_p::binder::command_list> commands_a);
		~echo_binder (void);
		void bind (::lambda_p::core::statement * statement, ::std::map < ::lambda_p::core::node *, ::boost::shared_ptr < ::lambda_p::binder::node_instance> > & instances, ::std::wstringstream & problems);
		void operator () ();
		::boost::shared_ptr < ::lambda_p::binder::command_list> commands;
	};
}
