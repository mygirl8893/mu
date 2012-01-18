#include <lambda_p_io/lexer/identifier.h>

#include <lambda_p_io/lexer/lexer.h>
#include <lambda_p_io/tokens/identifier.h>
#include <lambda_p_io/lexer/hex_code.h>

lambda_p_io::lexer::identifier::identifier (lambda_p_io::lexer::lexer & lexer_a)
	: lexer (lexer_a),
	lookahead (false)
{
}

void lambda_p_io::lexer::identifier::lex (wchar_t character)
{				
	if (lookahead)
	{
		lookahead = false;
		switch (character)
		{
			case L'u':
				lexer.state.push (boost::shared_ptr <lambda_p_io::lexer::state> (new lambda_p_io::lexer::hex_code (8, *this)));
				break;
			case L'a':
				lexer.state.push (boost::shared_ptr <lambda_p_io::lexer::state> (new lambda_p_io::lexer::hex_code (2, *this)));
				break;
			default:
				lexer.state.pop ();
				lexer (L':');
				lexer (character);
				break;
		}
	}
	else
	{
		switch (character)
		{
			case L' ':
			case L'\t':
			case L'\n':
			case L'\f':
			case L'\0':
			case L';':
			case L'|':
			case L'[':
			case L']':
			case L'\uffff':
				{
					lambda_p_io::tokens::identifier * identifier = new lambda_p_io::tokens::identifier (string);
					lexer.target (identifier);
					lexer.state.pop ();
					lexer (character);
				}
				break;
			case L':':
				lookahead = true;
				break;
			default:
				add (character);
				break;
		}
	}
}

void lambda_p_io::lexer::identifier::add (wchar_t character)
{
	string.push_back (character);
}