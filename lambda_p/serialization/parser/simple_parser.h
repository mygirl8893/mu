#include <lambda_p/tokens/token.h>
#include <lambda_p/tokens/identifier.h>
#include <lambda_p/tokens/complex_identifier.h>
#include <lambda_p/serialization/parser/parse_result.h>
#include <lambda_p/tokens/statement_end.h>
#include <lambda_p/tokens/declaration.h>
#include <lambda_p/tokens/hex_data_token.h>
#include <lambda_p/tokens/routine_end.h>
#include <lambda_p/tokens/data_token.h>
#include <lambda_p/serialization/parser/result_reference.h>
#include <lambda_p/serialization/parser/result_position.h>
#include <lambda_p/core/reference.h>
#include <lambda_p/core/statement.h>
#include <lambda_p/core/routine.h>
#include <lambda_p/serialization/parser/state.h>
#include <lambda_p/serialization/parser/begin.h>
#include <lambda_p/serialization/parser/error.h>
#include <lambda_p/serialization/parser/routine.h>
#include <lambda_p/serialization/parser/routine_parameter.h>
#include <lambda_p/serialization/parser/body.h>
#include <lambda_p/serialization/parser/statement.h>
#include <lambda_p/serialization/parser/reference.h>
#include <lambda_p/serialization/parser/data.h>
#include <lambda_p/serialization/parser/declaration.h>
#include <lambda_p/serialization/parser/hex_data.h>

#include <boost/function.hpp>

#include <map>
#include <stack>

namespace lambda_p
{
	namespace serialization
	{
		namespace parser
		{
			class simple_parser
			{
			public:
				simple_parser (::boost::function <void (::boost::shared_ptr < ::lambda_p::core::routine>)> target_a);
				void operator () (::lambda_p::tokens::token * token);
				void reset ();
				bool error ();
				::lambda_p::serialization::parser::state_id current_state ();
				void error_message (::std::wstring & target);
			private:
				void parse_internal (::lambda_p::tokens::token * token);
				void parse_error (::lambda_p::tokens::token * token);
				void parse_begin (::lambda_p::tokens::token * token);
				void parse_routine (::lambda_p::tokens::token * token);
				void parse_routine_parameters (::lambda_p::tokens::token * token);
				void parse_routine_body (::lambda_p::tokens::token * token);
				void parse_statement (::lambda_p::tokens::token * token);
				void parse_reference (::lambda_p::tokens::token * token);
				void parse_data (::lambda_p::tokens::token * token);
				void parse_declaration (::lambda_p::tokens::token * token);
				void parse_hex_data (::lambda_p::tokens::token * token);
				::std::wstring token_type_name (::lambda_p::tokens::token * token);
				::boost::function <void (::boost::shared_ptr < ::lambda_p::core::routine>)> target;
			public:
				::boost::shared_ptr < ::lambda_p::serialization::parser::state> last_state;
				::std::stack < ::boost::shared_ptr < ::lambda_p::serialization::parser::state> > state;
			};
		}
	}
}