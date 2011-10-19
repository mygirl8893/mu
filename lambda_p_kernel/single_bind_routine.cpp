#include "single_bind_routine.h"

#include <lambda_p/core/routine.h>
#include <lambda_p/core/association.h>
#include <lambda_p/core/statement.h>
#include <lambda_p_kernel/routine_binder.h>
#include <lambda_p_kernel/routine.h>

lambda_p_kernel::single_bind_routine::single_bind_routine (boost::shared_ptr < lambda_p_kernel::routine> inner_routine_a, boost::shared_ptr < lambda_p::binder::routine_instances> inner_instances_a)
	: binder_m (new lambda_p_kernel::routine_binder),
	outer_routine (new lambda_p::core::routine),
	outer_instances (new lambda_p::binder::routine_instances)
{
	size_t binder_parameter (outer_routine->add_declaration ());
	outer_routine->surface->results.push_back (binder_parameter);
	size_t routine_parameter (outer_routine->add_declaration ());
	outer_routine->surface->results.push_back (routine_parameter);
	size_t instances_parameter (outer_routine->add_declaration ());
	outer_routine->surface->results.push_back (instances_parameter);
	statement = outer_routine->add_statement (binder_parameter);
	statement->association->parameters.push_back (routine_parameter);
	statement->association->parameters.push_back (instances_parameter);
	(*outer_instances) [0] = binder_m;
	(*outer_instances) [1] = inner_routine_a;
	(*outer_instances) [2] = inner_instances_a;
}


void lambda_p_kernel::single_bind_routine::operator () (std::vector < boost::shared_ptr < lambda_p::errors::error> > & problems)
{
	binder_m->bind (statement, *outer_instances, problems);
}