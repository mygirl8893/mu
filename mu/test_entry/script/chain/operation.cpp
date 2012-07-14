#include <mu/core/errors/error_list.h>
#include <mu/script/chain/operation.h>
#include <mu/test_entry/script/chain/ten_count.h>
#include <mu/script/integer/node.h>
#include <mu/script/context.h>

#include <boost/make_shared.hpp>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (script_test, chain_operation1)
{
	auto operation (new (GC) mu::script_test::chain::ten_count);
	auto count (new (GC) mu::script::integer::node (50));
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::script::chain::operation);
	ctx.push (operation);
	ctx.push (count);
	auto valid (ctx ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (ctx.working_size (), 1);
	auto result (dynamic_cast <mu::script::integer::node *> (ctx.working (0)));
	EXPECT_EQ (result->value, 40);
}