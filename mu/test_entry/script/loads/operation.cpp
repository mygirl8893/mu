#include <mu/core/errors/error_list.h>
#include <mu/script/loads/operation.h>
#include <mu/script/extensions/node.h>
#include <mu/script/string/node.h>
#include <mu/io/analyzer/extensions/extensions.h>
#include <mu/script/context.h>

#include <boost/make_shared.hpp>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (script_test, loads1)
{
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::script::loads::operation);
	ctx.push (new (GC) mu::script::extensions::node);
	ctx.push (new (GC) mu::script::string::node (std::wstring (L"source_test.mu")));
	auto valid (ctx ());
	errors.target->print (std::wcout);
    EXPECT_EQ (valid, true);
	EXPECT_EQ (ctx.working_size (), 1);
	auto extensions (dynamic_cast <mu::script::extensions::node *> (ctx.working (0)));
	EXPECT_NE (extensions, nullptr);
	EXPECT_EQ (extensions->extensions->extensions_m.size (), 2);
	auto a (extensions->extensions->extensions_m.find (std::wstring (L"a")));
	EXPECT_NE (a, extensions->extensions->extensions_m.end ());
	auto b (extensions->extensions->extensions_m.find (std::wstring (L"b")));
	EXPECT_EQ (b, extensions->extensions->extensions_m.end ());
}