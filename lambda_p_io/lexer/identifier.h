#pragma once

#include <lambda_p_io/lexer/state.h>

#include <string>

namespace lambda_p_io
{
	namespace lexer
	{
		class lexer;
		class identifier : public lambda_p_io::lexer::state
		{
		public:
			identifier (lambda_p_io::lexer::lexer & lexer_a);
			void lex (wchar_t character) override;
			std::wstring string;
			lambda_p_io::lexer::lexer & lexer;
		};
	}
}

