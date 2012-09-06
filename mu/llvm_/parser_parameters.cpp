#include <mu/llvm_/parser_parameters.h>

#include <mu/io/tokens_implementor.h>
#include <mu/llvm_/parser_routine.h>
#include <mu/llvm_/parser_parser.h>
#include <mu/llvm_/parser_cluster.h>
#include <mu/llvm_/type_node.h>
#include <mu/core/errors/error_target.h>
#include <mu/llvm_/parser_error.h>
#include <mu/io/tokens_right_square.h>
#include <mu/llvm_/function_node.h>
#include <mu/llvm_/function_type_node.h>
#include <mu/llvm_/context_node.h>
#include <mu/llvm_/set_type_node.h>
#include <mu/llvm_/void_type_node.h>
#include <mu/llvm_/ast_routine.h>
#include <mu/llvm_/ast_parameter.h>

#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>
#include <llvm/ADT/ArrayRef.h>

#include <gc_cpp.h>

mu::llvm_::parser::parameters::parameters (mu::llvm_::parser::routine & routine_a):
routine (routine_a),
state (mu::llvm_::parser::parameters_state::name),
position (0)
{
}

void mu::llvm_::parser::parameters::operator () (mu::io::tokens::token * token_a)
{
    (*token_a) (this);
}

void mu::llvm_::parser::parameters::operator () (mu::io::tokens::divider const & token)
{
    switch (state)
    {
        case mu::llvm_::parser::parameters_state::results:
            break;
        case mu::llvm_::parser::parameters_state::parameters:
            state = mu::llvm_::parser::parameters_state::results;
        case mu::llvm_::parser::parameters_state::name:
            unexpected_token (routine.cluster.parser, token);
            break;
        default:
            assert (false);
            break;
    }
}

void mu::llvm_::parser::parameters::operator () (mu::io::tokens::identifier const & token)
{
    switch (state)
    {
        case mu::llvm_::parser::parameters_state::results:
        case mu::llvm_::parser::parameters_state::parameters:
            unexpected_token (routine.cluster.parser, token);
            break;
        case mu::llvm_::parser::parameters_state::name:
        {
            state = mu::llvm_::parser::parameters_state::parameters;
            auto position_l (position);
            routine.cluster.map.insert_routine_scope (routine.cluster.parser.errors, token.string, new (GC) mu::llvm_::ast::parameter (position_l), token.context);
            position = position_l + 1;
        }
            break;
        default:
            assert (false);
            break;
    }
}

void mu::llvm_::parser::parameters::operator () (mu::io::tokens::left_square const & token)
{
    unexpected_token (routine.cluster.parser, token);
}

void mu::llvm_::parser::parameters::operator () (mu::io::tokens::right_square const & token)
{
    switch (state)
    {
        case mu::llvm_::parser::parameters_state::results:
        case mu::llvm_::parser::parameters_state::name:
            routine.cluster.parser.state.pop ();
            break;
        case mu::llvm_::parser::parameters_state::parameters:
            unexpected_token (routine.cluster.parser, token);
            break;
        default:
            assert (false);
            break;
    }
}

void mu::llvm_::parser::parameters::operator () (mu::io::tokens::stream_end const & token)
{
    unexpected_token (routine.cluster.parser, token);
}

void mu::llvm_::parser::parameters::operator () (mu::io::tokens::value const & token)
{
    switch (state)
    {
        case mu::llvm_::parser::parameters_state::results:
        {
            auto type (dynamic_cast <mu::llvm_::type::node *> (token.node));
            if (type != nullptr)
            {
                routine.routine_m->results.push_back (type);
            }
            else                
            {
                mu::stringstream message;
                message << U"Unexpected value type: ";
                message << token.node->name ();
                message << U", expecting mu::llvm_::type::node";
                routine.cluster.parser.errors (message.str ());
                routine.cluster.parser.state.push (new (GC) mu::llvm_::parser::error);
            }
        }
            break;
        case mu::llvm_::parser::parameters_state::parameters:
        {
            auto type (dynamic_cast <mu::llvm_::type::node *> (token.node));
            if (type != nullptr)
            {
                state = mu::llvm_::parser::parameters_state::name;
                routine.routine_m->types.push_back (type);
            }
            else
            {
                mu::stringstream message;
                message << U"Unexpected value type: ";
                message << token.node->name ();
                message << U", expecting mu::llvm_::type::node";
                routine.cluster.parser.errors (message.str ());
                routine.cluster.parser.state.push (new (GC) mu::llvm_::parser::error);
            }
        }
            break;
        case mu::llvm_::parser::parameters_state::name:
            unexpected_token (routine.cluster.parser, token);
            break;
        default:
            assert (false);
            break;
    }
}