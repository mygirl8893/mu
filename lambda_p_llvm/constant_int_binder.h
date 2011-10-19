#pragma once

#include <lambda_p/binder/binder.h>

namespace lambda_p
{
	namespace binder
	{
		class data;
		class routine_instances;
	}
}
namespace lambda_p_llvm
{
	class generation_context;
	class constant_int_binder : public ::lambda_p::binder::binder
	{
	public:
		constant_int_binder (::lambda_p_llvm::generation_context & context_a);
		~constant_int_binder (void);
		void bind (::lambda_p::core::statement * statement, lambda_p::binder::routine_instances & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems);
		::std::wstring binder_name ();
	private:
		void parse_number (unsigned long base, unsigned long bits, ::std::wstring & number_wstring, size_t declaration_node, lambda_p::binder::routine_instances & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems);
		void parse_nodes (::boost::shared_ptr < ::lambda_p::binder::data> base_data, ::boost::shared_ptr < ::lambda_p::binder::data> number_data, ::boost::shared_ptr < ::lambda_p::binder::data> bits_data, size_t declaration_node, lambda_p::binder::routine_instances & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems);
		::lambda_p_llvm::generation_context & context;
	};
}

