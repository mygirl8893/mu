#pragma once

#include <mu/llvm_/analyzer/context.h>
#include <mu/script/operation.h>

#include <vector>

namespace llvm
{
	class Function;
}
namespace mu
{
	namespace core
	{
		class cluster;
        namespace errors
        {
            class error_target;
        }
	}
	namespace io
	{
		namespace analyzer
		{
			namespace extensions
			{
				class extensions;
			}
		}
	}
	namespace llvm_
	{
		namespace context
		{
			class node;
		}
		namespace instruction
		{
			class node;
		}
		namespace function_address
		{
			class operation;
		}
		namespace function
		{
			class node;
		}
		namespace function_type
		{
			class node;
		}
		namespace analyzer
		{
			class reference;
			class operation : public mu::script::operation
			{
			public:
				operation ();
				void operator () (mu::script::context & context_a) override;
				void finish_types (std::vector <std::type_info const *> * stack_a, boost::shared_ptr <mu::core::errors::error_target> errors_a, std::vector <std::pair <boost::shared_ptr <mu::llvm_::function::node>, boost::shared_ptr <mu::llvm_::function_type::node>>> * functions, std::vector <boost::shared_ptr <mu::llvm_::function::node>> * types, boost::shared_ptr <mu::core::cluster> cluster_a);
				void finish_bodies (std::vector <std::type_info const *> * stack_a, boost::shared_ptr <mu::core::errors::error_target> errors_a, std::vector <boost::shared_ptr <mu::core::node>> * results, std::vector <std::pair <boost::shared_ptr <mu::llvm_::function::node>, boost::shared_ptr <mu::llvm_::function_type::node>>> * functions, boost::shared_ptr <mu::core::cluster> cluster_a);
				mu::llvm_::analyzer::context context;
				boost::shared_ptr <mu::llvm_::analyzer::reference> reference_m;
				boost::shared_ptr <mu::io::analyzer::extensions::extensions> extensions;
			};
		}
	}
}

