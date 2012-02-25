#pragma once

#include <mu/core/node.h>

#include <boost/shared_ptr.hpp>

namespace llvm
{
	class Value;
}
namespace mu
{
	namespace llvm_
	{
		namespace type
		{
			class node;
		}
		namespace value
		{
			class node : public mu::core::node
			{
			public:
				node (llvm::Value * value_a, boost::shared_ptr <mu::llvm_::type::node> type_a);
				llvm::Value * value ();
				std::wstring name () override;
				boost::shared_ptr <mu::llvm_::type::node> type;
			protected:
				llvm::Value * value_m;
			};
		}
	}
}