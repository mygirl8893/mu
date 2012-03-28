#include <mu/io_test/hex_code.h>

#include <mu/io_test/lexer_result.h>
#include <mu/core/errors/error_list.h>
#include <mu/io/lexer/lexer.h>
#include <mu/io/source.h>
#include <mu/io/debugging/error.h>
#include <mu/io/debugging/stream.h>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

void mu::io_test::hex_code::run ()
{
	run_1 ();
	run_2 ();
}

void mu::io_test::hex_code::run_1 ()
{
	mu::io_test::lexer_result result;
	auto errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	mu::io::lexer::lexer lexer (errors, boost::bind (&mu::io_test::lexer_result::operator (), &result, _1, _2));
	lexer (L":aq");
	lexer ();
	assert (result.results.empty ());
	assert (!errors->errors.empty ());
	auto e1 (boost::dynamic_pointer_cast <mu::io::debugging::error> (errors->errors [0]));
	assert (e1.get () != nullptr);
	assert (e1->context == mu::io::debugging::context (1, 3, 2, 1, 3, 2));
}

void mu::io_test::hex_code::run_2 ()
{
	mu::io_test::lexer_result result;
	auto errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	mu::io::lexer::lexer lexer (errors, boost::bind (&mu::io_test::lexer_result::operator (), &result, _1, _2));
	lexer (L":uq");
	lexer ();
	assert (result.results.empty ());
	assert (!errors->errors.empty ());
	auto e1 (boost::dynamic_pointer_cast <mu::io::debugging::error> (errors->errors [0]));
	assert (e1.get () != nullptr);
	assert (e1->context == mu::io::debugging::context (1, 3, 2, 1, 3, 2));
}