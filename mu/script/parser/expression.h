#pragma once

#include <boost/function.hpp>

#include <mu/script/parser/state.h>
#include <mu/io/tokens/visitor.h>

namespace mu
{
    namespace core
    {
        class node_list;
    }
    namespace script
    {
        namespace ast
        {
            class expression;
        }
        namespace parser
        {
            enum class expression_state
            {
                values,
                name,
                have_name,
                elements
            };
            class routine;
            class expression : public mu::script::parser::state, public mu::io::tokens::visitor
            {
            public:
                expression (mu::script::parser::routine & routine_a, mu::script::ast::expression * expression_a);
                mu::script::ast::expression * expression_m;
                mu::io::debugging::context context;
                mu::script::parser::expression_state state;
                mu::script::parser::routine & routine;
                size_t element;
                void operator () (mu::io::tokens::token * token_a, mu::io::debugging::context context_a) override;
				void operator () (mu::io::tokens::divider * token) override;
				void operator () (mu::io::tokens::identifier * token) override;
				void operator () (mu::io::tokens::left_square * token) override;
				void operator () (mu::io::tokens::right_square * token) override;
				void operator () (mu::io::tokens::stream_end * token) override;
				void operator () (mu::io::tokens::parameters * token) override;
                void operator () (mu::io::tokens::value * token) override;
            };
        }
    }
}