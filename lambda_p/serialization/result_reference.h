#pragma once

#include <string>

namespace lambda_p
{
	namespace serialization
	{
		class result_reference
		{
		public:
			result_reference (::std::wstring & statement_a, ::std::wstring & argument_a);
			~result_reference (void);
			bool operator < (::lambda_p::serialization::result_reference const & other) const;
			::std::wstring statement;
			::std::wstring argument;
		};
	}
}

