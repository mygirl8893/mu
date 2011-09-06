#pragma once

#include <lambda_p/serialization/lexer/state.h>

namespace lambda_p
{
	namespace serialization
	{
		namespace lexer
		{
			class begin : public ::lambda_p::serialization::lexer::state
			{
			public:
				begin(void);
				~begin(void);				
				state_id state_type ();
			};
		}
	}
}

