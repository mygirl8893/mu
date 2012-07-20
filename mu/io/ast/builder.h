#pragma once

#include <mu/io/lexer/lexer.h>
#include <mu/io/parser/parser.h>
#include <mu/io/analyzer/analyzer.h>
#include <mu/io/ast/visitor.h>

#include <vector>

namespace mu
{
	namespace core
	{
		namespace errors
		{
			class error_list;
		}
	}
	namespace io
	{
		namespace ast
		{
			class node;
			class cluster;
			class builder
			{
			public:
				builder ();
				void operator () (char32_t char_a);
				mu::core::errors::error_list * errors;
				mu::io::parser::parser parser;
				mu::io::lexer::lexer lexer;
				void add (mu::io::ast::cluster * node_a);
				mu::io::ast::cluster * cluster;
			};
		}
	}
}
