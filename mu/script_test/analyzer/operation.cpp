#include "operation.h"

#include <mu/core/errors/error_list.h>
#include <mu/script/analyzer/operation.h>
#include <mu/io/ast/cluster.h>
#include <mu/script/extensions/node.h>
#include <mu/script/cluster/node.h>

#include <boost/make_shared.hpp>

void mu::script_test::analyzer::operation::run ()
{
	run_1 ();
}

void mu::script_test::analyzer::operation::run_1 ()
{
	auto errors (boost::make_shared <mu::core::errors::error_list> ());
	mu::script::analyzer::operation analyzer;
	auto cluster (analyzer.core (errors, boost::make_shared <mu::script::extensions::node> (), boost::make_shared <mu::io::ast::cluster> ()));
	assert (cluster.get () != nullptr);
}