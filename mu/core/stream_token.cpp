#include <mu/core/stream_token.hpp>

#include <mu/core/tokens.hpp>
#include <mu/core/lexer.hpp>

mu::io::stream_token::stream_token (mu::io::lexer & lexer_a, size_t size_a):
size_m (size_a),
lexer (lexer_a)
{
	refill ();
}

void mu::io::stream_token::consume (size_t size_a)
{
	assert (size_a <= size ());
	buffer.erase (buffer.begin (), buffer.begin () + size_a);
	refill ();
}

mu::io::token * & mu::io::stream_token::operator [] (size_t size_a)
{
	assert (size_a <= size ());
	auto & result (buffer [size_a]);
	return result;
}

size_t mu::io::stream_token::size ()
{
	return size_m;
}

void mu::io::stream_token::refill ()
{
	auto needed (size () - buffer.size ());
	auto & lexer_l (lexer);
	for (size_t i (0); i < needed; ++i)
	{
		auto token (lexer_l.lex ());
		if (token.error != nullptr)
		{
			buffer.push_back (new mu::io::end (mu::core::region (lexer.position, lexer.position)));
		}
		else
		{
			buffer.push_back (token.token);
		}
	}
}
