#pragma once

#include <mu/core/node.h>

namespace llvm
{
	class Type;
}
namespace mu
{
	namespace llvm_
	{
		namespace type
		{
			class node : public mu::core::node
			{
			public:
				mu::string name () override;
				virtual llvm::Type * type () = 0;
			};
		}
	}
}