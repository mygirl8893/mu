#include <mu/core/errors/error_list.h>
#include <mu/script/loadb/operation.h>
#include <mu/script/string/node.h>
#include <mu/script/extensions/node.h>
#include <mu/io/analyzer/extensions/extensions.h>
#include <mu/io/analyzer/extensions/global.h>
#include <mu/script/identity/operation.h>
#include <mu/script/context.h>

#include <boost/filesystem.hpp>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (script_test, loadb1)
{
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::script::loadb::operation);
	auto valid (ctx ());
	EXPECT_EQ (!valid, true);
}

TEST (script_test, loadb2)
{
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
    auto windows_name (mu::string (U"mu/binary_test/Debug/mu_binary_test.dll"));
    auto unix_name (mu::string (U"mu/binary_test/Debug/libmu_binary_test.so"));
    auto windows_path (boost::filesystem::initial_path() /= std::string (windows_name.begin (), windows_name.end ()));
    auto unix_path (boost::filesystem::initial_path () /= std::string (unix_name.begin (), unix_name.end ()));
    auto windows_exists (boost::filesystem::exists (windows_path));
    auto unix_exists (boost::filesystem::exists (unix_path));
    mu::script::string::node * file;
    if (windows_exists)
    {
        file = new (GC) mu::script::string::node (windows_name);
    }
    else if (unix_exists)
    {
        file = new (GC) mu::script::string::node (unix_name);
    }
    else
    {
        ASSERT_EQ (false, true);
    }
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::script::loadb::operation);
    ctx.push (file);
	auto valid (ctx ());
	ASSERT_EQ (valid, true);
	EXPECT_EQ (ctx.working_size (), 1);
    auto ext (ctx.working (0));
    std::type_info const & t1 (typeid (mu::script::extensions::node));
    std::type_info const & t2 (typeid (*ext));
	auto extensions (dynamic_cast <mu::script::extensions::node *> (ext));
	ASSERT_NE (extensions, nullptr);
	auto existing ((*extensions->extensions) (mu::string (U"identity")));
	EXPECT_NE (existing, nullptr);
	auto value (dynamic_cast <mu::io::analyzer::extensions::global *> (existing));
	EXPECT_NE (value, nullptr);
	auto identity (dynamic_cast <mu::script::identity::operation *> (value->node));
	EXPECT_NE (identity, nullptr);
}