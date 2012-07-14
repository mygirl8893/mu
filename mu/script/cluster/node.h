#pragma once

#include <mu/core/node.h>

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace mu
{
	namespace script
	{
		namespace runtime
		{
			class routine;
		}
		namespace cluster
		{
			class node : public mu::core::node
			{
			public:
				std::map <std::wstring, mu::script::runtime::routine *> names;
				std::vector <mu::script::runtime::routine *> routines;
			};
		}
	}
}
