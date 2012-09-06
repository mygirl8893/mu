#pragma once

#include <mu/io/tokens_token.h>

#include <string>

namespace mu
{
	namespace io
	{
		namespace tokens
		{
			class right_square : public mu::io::tokens::token
			{
			public:
                right_square (mu::io::context const & context_a);
				mu::string token_name () const override;
				void operator () (mu::io::tokens::visitor * visitor_a) override;
			};
		}
	}
}
