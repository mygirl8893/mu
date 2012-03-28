#include <mu/script_test/ast/extension.h>

#include <mu/core/expression.h>
#include <mu/io/ast/expression.h>
#include <mu/io/ast/identifier.h>
#include <mu/io/ast/cluster.h>
#include <mu/script/builder.h>
#include <mu/core/errors/error_list.h>
#include <mu/script/context.h>
#include <mu/script/runtime/routine.h>
#include <mu/script/api.h>
#include <mu/script/extensions/node.h>
#include <mu/io/source.h>
#include <mu/script/cluster/node.h>
#include <mu/io/debugging/stream.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

void mu::script_test::ast::extension::run ()
{
	run_1 ();
	run_2 ();
	run_3 ();
}

void mu::script_test::ast::extension::run_1 ()
{
	mu::script::builder builder (boost::make_shared <mu::io::debugging::stream> (), boost::shared_ptr <mu::script::extensions::node> (mu::script::api::core ())->extensions);
	builder (L"[~ .ast []]");
	builder ();
	assert (builder.errors->errors.empty ());
	auto cluster (builder.cluster);
	assert (cluster != nullptr);
	assert (cluster->routines.size () == 1);
	auto routine (cluster->routines [0]);
	mu::script::context ctx (mu::core::errors::errors (builder.errors));
	ctx.push (routine);
	auto valid (ctx ());
	assert (valid);
	assert (ctx.working_size () == 1);
	auto cl (boost::dynamic_pointer_cast <mu::io::ast::cluster> (ctx.working (0)));
	assert (cl.get () != nullptr);
	assert (cl->expressions.size () == 0);
}

void mu::script_test::ast::extension::run_2 ()
{
	mu::script::builder builder (boost::make_shared <mu::io::debugging::stream> (), boost::shared_ptr <mu::script::extensions::node> (mu::script::api::core ())->extensions);
	builder (L"[~ .ast thing]");
	builder ();
	assert (!builder.errors->errors.empty ());
	assert (builder.cluster == nullptr);
}

void mu::script_test::ast::extension::run_3 ()
{
	mu::script::builder builder (boost::make_shared <mu::io::debugging::stream> (), boost::shared_ptr <mu::script::extensions::node> (mu::script::api::core ())->extensions);
	builder (L"[~ .ast [[]]]");
	builder ();
	assert (builder.errors->errors.empty ());
	auto cluster (builder.cluster);
	assert (cluster != nullptr);
	assert (cluster->routines.size () == 1);
	auto routine (cluster->routines [0]);
	mu::script::context ctx (mu::core::errors::errors (builder.errors));
	ctx.push (routine);
	auto valid (ctx ());
	assert (valid);
	assert (ctx.working_size () == 1);
	auto cl (boost::dynamic_pointer_cast <mu::io::ast::cluster> (ctx.working (0)));
	assert (cl.get () != nullptr);
	assert (cl->expressions.size () == 1);
	auto expression (cl->expressions [0]);
}