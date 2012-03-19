#include <mu/script_test/closure/operation.h>

#include <mu/script/closure/operation.h>
#include <mu/script/identity/operation.h>
#include <mu/core/errors/error_list.h>
#include <mu/script/closure/hole.h>
#include <mu/script_runtime/context.h>

#include <boost/make_shared.hpp>

void mu::script_test::closure::operation::run ()
{
	run_1 ();
	run_2 ();
	run_3 ();
	run_4 ();
	run_5 ();
}

void mu::script_test::closure::operation::run_1 ()
{
	mu::core::errors::errors errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	mu::script_runtime::context ctx (errors);
	ctx.push (boost::make_shared <mu::script::closure::operation> (0, boost::shared_ptr <mu::script_runtime::operation> (new mu::script::identity::operation)));
	auto valid (ctx ());
	assert (valid);
	assert (ctx.working_size () == 0);
}

void mu::script_test::closure::operation::run_2 ()
{
	mu::core::errors::errors errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	mu::script_runtime::context ctx (errors);
	ctx.push (boost::make_shared <mu::script::closure::operation> (1, boost::shared_ptr <mu::script_runtime::operation> (new mu::script::identity::operation)));
	auto node (boost::shared_ptr <mu::core::node> (new mu::core::node));
	ctx.push (node);
	auto valid (ctx ());
	assert (valid);
	assert (ctx.working_size () == 1);
	assert (ctx.working (0) == node);
}

void mu::script_test::closure::operation::run_3 ()
{
	mu::core::errors::errors errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	mu::script_runtime::context ctx (errors);
	ctx.push (boost::make_shared <mu::script::closure::operation> (2, boost::shared_ptr <mu::script_runtime::operation> (new mu::script::identity::operation)));
	auto n1 (boost::shared_ptr <mu::core::node> (new mu::core::node));
	ctx.push (n1);
	auto n2 (boost::shared_ptr <mu::core::node> (new mu::core::node));
	ctx.push (n2);
	auto valid (ctx ());
	assert (valid);
	assert (ctx.working_size () == 2);
	assert (ctx.working (0) == n1);
	assert (ctx.working (1) == n2);
}

void mu::script_test::closure::operation::run_4 ()
{
	mu::core::errors::errors errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	mu::script_runtime::context ctx (errors);
	ctx.push (boost::make_shared <mu::script::closure::operation> (2, boost::shared_ptr <mu::script_runtime::operation> (new mu::script::identity::operation)));
	auto n1 (boost::shared_ptr <mu::core::node> (new mu::core::node));
	ctx.push (n1);
	auto n2 (boost::shared_ptr <mu::core::node> (new mu::script::closure::hole));
	ctx.push (n2);
	auto valid (ctx ());
	assert (valid);
	assert (ctx.working_size () == 1);
	auto result (boost::dynamic_pointer_cast <mu::script::closure::operation> (ctx.working (0)));
	assert (result.get () != nullptr);
	ctx.drop ();
	ctx.push (result);
	auto n3 (boost::shared_ptr <mu::core::node> (new mu::core::node));
	ctx.push (n3);
	auto valid2 (ctx ());
	assert (valid2);
	assert (ctx.working_size () == 2);
	assert (ctx.working (0) == n1);
	assert (ctx.working (1) == n3);
}

void mu::script_test::closure::operation::run_5 ()
{
	auto errors (boost::shared_ptr <mu::core::errors::error_list> (new mu::core::errors::error_list));
	mu::script::closure::operation closure (2, boost::shared_ptr <mu::script_runtime::operation> (new mu::script::identity::operation));
	std::vector <boost::shared_ptr <mu::core::node>> arguments;
	auto n1 (boost::shared_ptr <mu::core::node> (new mu::script::closure::hole));
	arguments.push_back (n1);
	auto n2 (boost::shared_ptr <mu::core::node> (new mu::core::node));
	arguments.push_back (n2);
	std::vector <boost::shared_ptr <mu::core::node>> results;
	std::vector <boost::shared_ptr <mu::script::debugging::call_info>> stack;
    auto ctx (mu::script::context (errors, arguments, results, stack));
	closure (ctx);
	assert (errors->errors.empty ());
	assert (results.size () == 1);
	auto result (boost::dynamic_pointer_cast <mu::script::closure::operation> (results [0]));
	assert (result.get () != nullptr);
	auto n3 (boost::shared_ptr <mu::core::node> (new mu::core::node));
	std::vector <boost::shared_ptr <mu::core::node>> args2;
	args2.push_back (n3);
	std::vector <boost::shared_ptr <mu::core::node>> res2;
    auto ctx2 (mu::script::context (errors, args2, res2, stack));
	(*result) (ctx2);
	assert (res2.size () == 2);
	assert (res2 [0] == n3);
	assert (res2 [1] == n2);
}