#pragma once

#include <vector>
#include <map>

#include <lambda_p/core/position.h>

namespace lambda_p_test
{
	class simple_parser_test_1;
}
namespace lambda_p_repl
{
	class entry_environment;
}
namespace lambda_p
{
	namespace binder
	{
		class routine_binder;
	}
	namespace serialization
	{
		template <typename> class simple;
	}
	namespace parser
	{
		class simple_parser;
		class statement;
		class routine;
	}
	namespace errors
	{
		class error;
	}
	namespace core
	{
		class association;
		class data;
		class statement;
		class declaration;
		class node;
		class routine
		{
			friend class ::lambda_p::core::statement;
			friend class ::lambda_p::core::data;
			friend class ::lambda_p::core::declaration;
			friend class ::lambda_p::core::node;
            template <typename> friend class ::lambda_p::serialization::simple;
			friend class ::lambda_p::parser::simple_parser;
			friend class ::lambda_p::binder::routine_binder;
			friend class ::lambda_p::parser::statement;
			friend class ::lambda_p_repl::entry_environment;
			friend class ::lambda_p::parser::routine;
			friend class ::lambda_p_test::simple_parser_test_1;
		private:
		public:
			routine ();
			~routine (void);
			::lambda_p::core::statement * add_statement (::lambda_p::core::declaration * target_a);
			::lambda_p::core::data * add_data (::std::wstring string);
			::lambda_p::core::declaration * add_declaration ();
            void placement (::std::map < ::lambda_p::core::node const *, ::lambda_p::core::position> & argument_positions, ::std::map < ::lambda_p::core::statement const *, size_t> & statement_positions) const;
			void validate (::std::vector < ::lambda_p::errors::error *> & problems) const;
			::lambda_p::core::association * surface;
		private:
			void validate_node (::lambda_p::core::node * node, size_t current_statement, size_t current_argument, ::std::vector < ::lambda_p::errors::error *> & problems) const;
			::std::vector < ::lambda_p::core::statement *> statements;
			::std::vector < ::lambda_p::core::declaration *> declarations;
			::std::vector < ::lambda_p::core::data *> data;
		};
	}
}
