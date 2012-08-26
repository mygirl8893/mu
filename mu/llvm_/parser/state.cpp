#include <mu/llvm_/parser/state.h>

#include <mu/io/tokens/token.h>
#include <mu/core/errors/error_target.h>
#include <mu/llvm_/parser/error.h>
#include <mu/llvm_/parser/parser.h>

#include <gc_cpp.h>

void mu::llvm_::parser::state::unexpected_token (mu::llvm_::parser::parser & parser_a, mu::io::tokens::token * token_a)
{
    mu::stringstream message;
    message << U"Unexpected token: ";
    message << token_a->token_name ();
    message << U" at: ";
    message << token_a->context.string ();
    parser_a.errors (message.str ());
    parser_a.state.push (new (GC) mu::llvm_::parser::error);
}