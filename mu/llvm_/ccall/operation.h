#pragma once

#include <mu/script/operation.h>

namespace mu
{
	namespace llvm_
	{
		namespace basic_block
		{
			class node;
		}
		namespace ccall
		{
			class operation : public mu::script::operation
			{
			public:
				operation (mu::llvm_::basic_block::node * block_a, mu::script::operation * call_a);
				bool operator () (mu::script::context & context_a) override;
				mu::llvm_::basic_block::node * block;
				mu::script::operation * call;
			};
		}
	}
}
