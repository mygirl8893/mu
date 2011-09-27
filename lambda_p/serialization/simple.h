#pragma once

#include <boost/iostreams/stream.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <lambda_p/core/routine.h>
#include <lambda_p/core/statement.h>
#include <lambda_p/core/node.h>
#include <lambda_p/core/data.h>
#include <lambda_p/core/reference.h>
#include <lambda_p/core/declaration.h>

#include <vector>

namespace lambda_p
{
	namespace serialization
	{
		template <typename stream_type>
		class simple
		{
		public:
			simple (stream_type & target_a)
				: target (target_a)
			{
			}
			~simple (void)
			{
			}
			void routine (::lambda_p::core::routine const * routine_a)
			{
                ::std::map < ::lambda_p::core::node const *, ::lambda_p::core::position> argument_positions;
                ::std::map < ::lambda_p::core::statement const *, size_t> statement_positions;
                routine_a->placement (argument_positions, statement_positions);
                for (::std::vector < ::lambda_p::core::statement *>::const_iterator i = routine_a->statements.begin (); i != routine_a->statements.end (); ++i)
                {
                    target << " ";
                    statement (argument_positions, statement_positions, *i);
                    target << "\n";
                }
				target << "#;";
			}
			void statement (::std::map < ::lambda_p::core::node const *, ::lambda_p::core::position> & argument_positions, ::std::map < ::lambda_p::core::statement const *, size_t> & statement_positions, ::lambda_p::core::statement const * statement_a)
			{
                target << "statement";
                target << statement_positions [statement_a];
                for (::std::vector < ::lambda_p::core::node *>::const_iterator i = statement_a->arguments.begin (); i != statement_a->arguments.end (); ++i)
                {
                    target << "\n  ";
                    node (argument_positions, *i);
                }
                target << "\n ;";
			}
			void node (::std::map < ::lambda_p::core::node const *, ::lambda_p::core::position> & argument_positions, ::lambda_p::core::node const * node_a)
			{
				::lambda_p::core::node_id node_id (node_a->node_type ());
				switch (node_id)
				{
				case ::lambda_p::core::node_reference:
					reference (argument_positions, static_cast < ::lambda_p::core::reference const *> (node_a));
					break;
				case ::lambda_p::core::node_data:
					data (static_cast < ::lambda_p::core::data const *> (node_a));
					break;
				case ::lambda_p::core::node_declaration:
					declaration (argument_positions, static_cast < ::lambda_p::core::declaration const *> (node_a));
					break;
				default:
					assert (false);
					break;
				}
			}
			void data (::lambda_p::core::data const * data_a)
			{
                target << "' ";
                target << data_a->string ();
			}
            void reference (::std::map < ::lambda_p::core::node const *, ::lambda_p::core::position> & argument_positions, ::lambda_p::core::reference const * result_ref_a)
            {
                ::lambda_p::core::position position (argument_positions [result_ref_a->declaration]);
                target << "statement";
                target << position.statement;
                target << " ";
                target << "declaration";
                target << position.argument;
            }
			void declaration (::std::map < ::lambda_p::core::node const *, ::lambda_p::core::position> & argument_positions, ::lambda_p::core::declaration const * result_a)
			{
                ::lambda_p::core::position position (argument_positions [result_a]);
				target << ";! ";
                target << "declaration";
                target << position.argument;
			}
			stream_type & target;
		};
	}
}
