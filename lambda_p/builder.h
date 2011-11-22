#pragma once

#include <lambda_p/parser/routine_vector.h>
#include <lambda_p/lexer/lexer.h>
#include <lambda_p/parser/parser.h>

namespace lambda_p
{
	namespace lexer
	{
		class character_stream;
	}
	class builder
	{
	public:
		builder ();
		builder (std::map <std::wstring, boost::shared_ptr <lambda_p::parser::state_factory>> keywords_a, std::vector <std::pair <std::wstring, boost::shared_ptr <lambda_p::binder::node>>> & injected_parameters_a, std::vector <std::wstring> & injected_returns_a);
		void finish ();
		void operator () (std::wstring & string);
		void operator () (wchar_t const * string);
		void operator () (boost::shared_ptr <lambda_p::lexer::character_stream> source);
		void operator << (boost::shared_ptr <lambda_p::lexer::character_stream> source);
		virtual std::vector <std::pair <std::wstring, boost::shared_ptr <lambda_p::binder::node>>> injected_declarations ();
		virtual std::vector <std::wstring> injected_references ();
		virtual std::map <std::wstring, boost::shared_ptr <lambda_p::parser::state_factory>> keywords ();
		bool error ();
		void error_message (std::wostream & out);
		lambda_p::parser::routine_vector routines;
		lambda_p::parser::parser parser;
		lambda_p::lexer::lexer lexer;
	};
}

