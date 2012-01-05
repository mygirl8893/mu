#include "lexer.h"

#include <lambda_p_io_test/lexer_result.h>
#include <lambda_p_io/lexer/lexer.h>
#include <lambda_p_io/source.h>
#include <lambda_p_io/tokens/token.h>
#include <lambda_p_io/tokens/left_square.h>
#include <lambda_p_io/tokens/right_square.h>
#include <lambda_p_io/tokens/divider.h>
#include <lambda_p_io/tokens/identifier.h>
#include <lambda_p_io/tokens/stream_end.h>

#include <boost/bind.hpp>

void lambda_p_io_test::lexer::run ()
{
	run_1 ();
	run_2 ();
	run_3 ();
	run_4 ();
	run_5 ();
	run_6 ();
	run_7 ();
	run_8 ();
	run_9 ();
	run_10 ();
}

void lambda_p_io_test::lexer::run_1 ()
{
	lambda_p_io_test::lexer_result result;
	lambda_p_io::lexer::lexer lexer (boost::bind (&lambda_p_io_test::lexer_result::operator (), &result, _1));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L"");
	assert (result.results.empty ());
}

void lambda_p_io_test::lexer::run_2 ()
{
	lambda_p_io_test::lexer_result result;
	lambda_p_io::lexer::lexer lexer (boost::bind (&lambda_p_io_test::lexer_result::operator (), &result, _1));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L"");
	source ();
	assert (result.results.size () == 1);
	auto t1 (result.results [0]);
	assert (dynamic_cast <lambda_p_io::tokens::stream_end *> (t1) != nullptr);
}

void lambda_p_io_test::lexer::run_3 ()
{
	lambda_p_io_test::lexer_result result;
	lambda_p_io::lexer::lexer lexer (boost::bind (&lambda_p_io_test::lexer_result::operator (), &result, _1));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L"a[];");
	source ();
	assert (result.results.size () == 5);
	auto t1 (result.results [0]);
	auto t2 (result.results [1]);
	auto t3 (result.results [2]);
	auto t4 (result.results [3]);
	auto t5 (result.results [4]);
	auto t1i (dynamic_cast <lambda_p_io::tokens::identifier *> (t1));
	assert (t1i != nullptr);
	assert (t1i->string.size () == 1);
	assert (t1i->string [0] == L'a');
	assert (dynamic_cast <lambda_p_io::tokens::left_square *> (t2) != nullptr);
	assert (dynamic_cast <lambda_p_io::tokens::right_square *> (t3) != nullptr);
	assert (dynamic_cast <lambda_p_io::tokens::divider *> (t4) != nullptr);
	assert (dynamic_cast <lambda_p_io::tokens::stream_end *> (t5) != nullptr);
}

void lambda_p_io_test::lexer::run_4 ()
{
	lambda_p_io_test::lexer_result result;
	lambda_p_io::lexer::lexer lexer (boost::bind (&lambda_p_io_test::lexer_result::operator (), &result, _1));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L"||");
	source ();
	assert (result.results.size () == 2);
	auto t1 (result.results [0]);
	auto t2 (result.results [1]);
	auto t1i (dynamic_cast <lambda_p_io::tokens::identifier *> (t1));
	assert (t1i != nullptr);
	assert (t1i->string.size () == 0);
	assert (dynamic_cast <lambda_p_io::tokens::stream_end *> (t2) != nullptr);
}

void lambda_p_io_test::lexer::run_5 ()
{
	lambda_p_io_test::lexer_result result;
	lambda_p_io::lexer::lexer lexer (boost::bind (&lambda_p_io_test::lexer_result::operator (), &result, _1));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L"|a|a");
	source ();
	assert (result.results.size () == 2);
	auto t1 (result.results [0]);
	auto t2 (result.results [1]);
	auto t1i (dynamic_cast <lambda_p_io::tokens::identifier *> (t1));
	assert (t1i != nullptr);
	assert (t1i->string.size () == 0);
	assert (dynamic_cast <lambda_p_io::tokens::stream_end *> (t2) != nullptr);
}

void lambda_p_io_test::lexer::run_6 ()
{
	lambda_p_io_test::lexer_result result;
	lambda_p_io::lexer::lexer lexer (boost::bind (&lambda_p_io_test::lexer_result::operator (), &result, _1));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L"|a||;[]:a");
	source ();
	assert (result.results.size () == 2);
	auto t1 (result.results [0]);
	auto t2 (result.results [1]);
	auto t1i (dynamic_cast <lambda_p_io::tokens::identifier *> (t1));
	assert (t1i != nullptr);
	assert (t1i->string == std::wstring (L"|;[]:"));
	assert (dynamic_cast <lambda_p_io::tokens::stream_end *> (t2) != nullptr);
}

void lambda_p_io_test::lexer::run_7 ()
{
	lambda_p_io_test::lexer_result result;
	lambda_p_io::lexer::lexer lexer (boost::bind (&lambda_p_io_test::lexer_result::operator (), &result, _1));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L"|:a||;[]:a");
	source ();
	assert (result.results.size () == 2);
	auto t1 (result.results [0]);
	auto t2 (result.results [1]);
	auto t1i (dynamic_cast <lambda_p_io::tokens::identifier *> (t1));
	assert (t1i != nullptr);
	assert (t1i->string == std::wstring (L"|;[]"));
	assert (dynamic_cast <lambda_p_io::tokens::stream_end *> (t2) != nullptr);
}

void lambda_p_io_test::lexer::run_8 ()
{
	lambda_p_io_test::lexer_result result;
	lambda_p_io::lexer::lexer lexer (boost::bind (&lambda_p_io_test::lexer_result::operator (), &result, _1));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L":- a\nb");
	source ();
	assert (result.results.size () == 2);
	auto t1 (dynamic_cast <lambda_p_io::tokens::identifier *> (result.results [0]));
	assert (t1->string == std::wstring (L"b"));
	assert (dynamic_cast <lambda_p_io::tokens::stream_end *> (result.results [1]) != nullptr);
}

void lambda_p_io_test::lexer::run_9 ()
{
	lambda_p_io_test::lexer_result result;
	lambda_p_io::lexer::lexer lexer (boost::bind (&lambda_p_io_test::lexer_result::operator (), &result, _1));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L":[ a :] b");
	source ();
	assert (result.results.size () == 2);
	auto t1 (dynamic_cast <lambda_p_io::tokens::identifier *> (result.results [0]));
	assert (t1->string == std::wstring (L"b"));
	assert (dynamic_cast <lambda_p_io::tokens::stream_end *> (result.results [1]) != nullptr);
}

void lambda_p_io_test::lexer::run_10 ()
{
	lambda_p_io_test::lexer_result result;
	lambda_p_io::lexer::lexer lexer (boost::bind (&lambda_p_io_test::lexer_result::operator (), &result, _1));
	lambda_p_io::source source (boost::bind (&lambda_p_io::lexer::lexer::operator (), &lexer, _1));
	source (L":[:[ a :]:] b");
	source ();
	assert (result.results.size () == 2);
	auto t1 (dynamic_cast <lambda_p_io::tokens::identifier *> (result.results [0]));
	assert (t1->string == std::wstring (L"b"));
	assert (dynamic_cast <lambda_p_io::tokens::stream_end *> (result.results [1]) != nullptr);
}