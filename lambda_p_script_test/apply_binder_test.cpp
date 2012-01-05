#include "apply_binder_test.h"

#include <lambda_p_kernel/apply.h>
#include <lambda_p/core/routine.h>
#include <lambda_p/core/statement.h>
#include <lambda_p/core/association.h>
#include <lambda_p_kernel/nodes/data.h>
#include <lambda_p/errors/error_list.h>

void lambda_p_test::apply_binder_test::run ()
{
	run_1 ();
}

// Makes sure apply correctly handles putting nodes in to correct node_list location with respect to the routine's surface declarations
// The routine surface declarations won't necessarily match their index location i.e. injected declarations come first
void lambda_p_test::apply_binder_test::run_1 ()
{
	boost::shared_ptr <lambda_p::core::routine> routine (new lambda_p::core::routine);
	size_t data (routine->inject_declaration (boost::shared_ptr <lambda_p::binder::node> (new lambda_p_kernel::nodes::data (std::wstring (L"~")))));
	size_t declaration (routine->add_declaration ());
	routine->surface->declarations.push_back (declaration);
	boost::shared_ptr <lambda_p::binder::list> list (new lambda_p::binder::list);
	list->operator[] (0) = boost::shared_ptr <lambda_p::binder::node> (new lambda_p_kernel::nodes::data (std::wstring (L"test")));
	boost::shared_ptr <lambda_p_kernel::apply> apply (new lambda_p_kernel::apply);
	boost::shared_ptr <lambda_p::core::routine> rout (routine);
	lambda_p::errors::error_list problems;
	lambda_p::binder::list declarations;
	apply->core (rout, *list, problems, declarations);
	assert (problems.errors.empty ());
}