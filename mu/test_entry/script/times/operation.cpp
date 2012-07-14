#include <mu/script/times/operation.h>
#include <mu/core/errors/error_list.h>
#include <mu/script/integer/subtract.h>
#include <mu/script/integer/node.h>
#include <mu/script/builder.h>
#include <mu/io/source.h>
#include <mu/script/context.h>
#include <mu/core/routine.h>
#include <mu/script/api.h>
#include <mu/script/extensions/node.h>
#include <mu/core/expression.h>
#include <mu/core/parameters.h>
#include <mu/script/identity/operation.h>
#include <mu/script/cluster/node.h>
#include <mu/script/runtime/routine.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (script_test, times_operation1)
{
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::script::times::operation);
	auto n1 (new (GC) mu::script::integer::node (0));
	ctx.push (n1);
	auto n2 (new (GC) mu::script::integer::subtract);
	ctx.push (n2);
	auto n3 (new (GC) mu::script::integer::node (5));
	ctx.push (n3);
	auto n4 (new (GC) mu::script::integer::node (1));
	ctx.push (n4);
	auto valid (ctx ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (ctx.working_size (), 2);
	EXPECT_EQ (ctx.working (0), n3);
	EXPECT_EQ (ctx.working (1), n4);
}

TEST (script_test, times_operation2)
{	
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	mu::script::context ctx (errors);
	auto expression (new (GC) mu::core::expression);
	expression->dependencies.push_back (new (GC) mu::core::parameters);
	auto routine (new (GC) mu::core::routine (expression));
	ctx.push (new (GC) mu::script::times::operation);
	ctx.push (new (GC) mu::script::integer::node (10));
	ctx.push (new (GC) mu::script::identity::operation);
	auto n1 (new (GC) mu::core::node);
	ctx.push (n1);
	auto n2 (new (GC) mu::core::node);
	ctx.push (n2);
	auto valid (ctx ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (ctx.working_size (), 2);
	EXPECT_EQ (ctx.working (0), n1);
	EXPECT_EQ (ctx.working (1), n2);
}

TEST (script_test, times_operation3)
{	
	mu::script::builder builder (boost::shared_ptr <mu::script::extensions::node> (mu::script::api::core ())->extensions);
	builder (L"[[~ :~; subtract number amount] ~ subtract [subtract number amount] amount]");
	builder ();
	EXPECT_EQ (builder.errors->errors.empty (), true);
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::script::times::operation);
	auto cluster (builder.cluster);
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 1);
	auto n1 (new (GC) mu::script::integer::node (2));
	ctx.push (n1);
	auto n2 (cluster->routines [0]);
	ctx.push (n2);
	auto n3 (new (GC) mu::script::integer::subtract);
	ctx.push (n3);
	auto n4 (new (GC) mu::script::integer::node (5));
	ctx.push (n4);
	auto n5 (new (GC) mu::script::integer::node (1));
	ctx.push (n5);
	auto valid (ctx ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (ctx.working_size (), 3);
	auto r1 (dynamic_cast <mu::script::integer::subtract *> (ctx.working (0)));
	EXPECT_NE (r1, nullptr);
	auto r2 (dynamic_cast <mu::script::integer::node *> (ctx.working (1)));
	EXPECT_NE (r2, nullptr);
	EXPECT_EQ (r2->value, 3);
	auto r3 (dynamic_cast <mu::script::integer::node *> (ctx.working (2)));
	EXPECT_NE (r3, nullptr);
	EXPECT_EQ (r3->value, 1);
}