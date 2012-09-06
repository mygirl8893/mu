#include <mu/script/parser_state.h>

#include <mu/io/tokens_token.h>
#include <mu/core/errors/error_target.h>
#include <mu/script/parser_error.h>
#include <mu/script/parser_parser.h>

#include <gc_cpp.h>

void mu::script::parser::state::unexpected_token (mu::script::parser::parser & parser_a, mu::io::tokens::token const & token_a)
{
    mu::stringstream message;
    message << U"Unexpected token: ";
    message << token_a.token_name ();
    message << U" at: ";
    message << token_a.context.string ();
    parser_a.errors (message.str ());
    parser_a.state.push (new (GC) mu::script::parser::error);
}