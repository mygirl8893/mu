#include <mu/test_entry/io/analyzer_result.h>
#include <mu/core/errors/error_list.h>
#include <mu/io/parser/parser.h>
#include <mu/io/lexer/lexer.h>
#include <mu/io/analyzer/analyzer.h>
#include <mu/io/source.h>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (io_test, routine1)
{
	mu::io_test::analyzer_result result;
	auto errors (new (GC) mu::core::errors::error_list);
	mu::io::analyzer::analyzer analyzer (boost::bind (&mu::io_test::analyzer_result::operator(), &result, _1), errors);
	mu::io::parser::parser parser (errors, boost::bind (&mu::io::analyzer::analyzer::input, &analyzer, _1));
	mu::io::lexer::lexer lexer (errors, boost::bind (&mu::io::parser::parser::operator (), &parser, _1, _2));
	lexer (L"[unresolved ;; 1]");
	lexer ();
	EXPECT_EQ (result.clusters.empty (), true);
	EXPECT_EQ (!errors->errors.empty (), true);
}