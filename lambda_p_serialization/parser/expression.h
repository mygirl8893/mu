#pragma once

#include <lambda_p_serialization/parser/state.h>
#include <lambda_p_serialization/parser/expression_state.h>

#include <boost/shared_ptr.hpp>

#include <map>
#include <vector>

namespace lambda_p
{
	namespace core
	{
		class expression;
		class tee;
		class gather;
		class target;
		class scatter;
	}
}
namespace lambda_p_serialization
{
	namespace parser
	{
		class parser;
		class routine;
		class expression : public lambda_p_serialization::parser::state
		{
		public:
			expression (lambda_p_serialization::parser::parser & parser_a, lambda_p_serialization::parser::routine & routine_a, boost::shared_ptr <lambda_p::core::target> target_a);
			boost::shared_ptr <lambda_p::core::tee> tee;
			boost::shared_ptr <lambda_p::core::scatter> scatter;
			boost::shared_ptr <lambda_p::core::gather> gather;
			boost::shared_ptr <lambda_p::core::target> target;
			lambda_p_serialization::parser::routine & routine;
			lambda_p_serialization::parser::parser & parser;
			lambda_p_serialization::parser::expression_state::expression_state state;
			size_t position;
			std::vector <std::wstring> local_names;
			std::wstring full_name;
			void resolve ();
			void back_resolve (std::wstring identifier, boost::shared_ptr <lambda_p::core::expression> expression);
			void parse (lambda_p_serialization::tokens::token * token) override;
			void parse_expression (lambda_p_serialization::tokens::token * token);
			void parse_local_name (lambda_p_serialization::tokens::token * token);
			void parse_full_name (lambda_p_serialization::tokens::token * token);
			void parse_nested (lambda_p_serialization::tokens::token * token);
		};
	}
}

