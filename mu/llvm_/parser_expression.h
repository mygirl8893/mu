#pragma once

#include <boost/function.hpp>

#include <mu/llvm_/parser_state.h>
#include <mu/io/tokens_visitor.h>

namespace mu
{
    namespace llvm_
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
            class expression : public mu::llvm_::parser::state, public mu::io::tokens::visitor
            {
            public:
                expression (mu::llvm_::parser::routine & routine_a);
                mu::llvm_::parser::expression_state state;
                mu::llvm_::parser::routine & routine;
                size_t element;
                mu::llvm_::ast::expression * expression_m;
                void operator () (mu::io::tokens::token * token_a) override;
				void operator () (mu::io::tokens::divider const & token) override;
				void operator () (mu::io::tokens::identifier const & token) override;
				void operator () (mu::io::tokens::left_square const & token) override;
				void operator () (mu::io::tokens::right_square const & token) override;
				void operator () (mu::io::tokens::stream_end const & token) override;
                void operator () (mu::io::tokens::value const & token) override;
            };
        }
    }
}