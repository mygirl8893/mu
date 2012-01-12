#include "operation.h"

#include <lambda_p_script/closure/hole.h>

lambda_p_script::closure::operation::operation (size_t count_a, boost::shared_ptr <lambda_p_script::operation> operation_a)
	: closed (count_a),
	open (count_a),
	operation_m (operation_a)
{
	for (size_t i (0); i != count_a; ++i)
	{
		open [i] = i;
	}
}

lambda_p_script::closure::operation::operation (boost::shared_ptr <lambda_p_script::operation> operation_a, std::vector <size_t> & open_a, std::vector <boost::shared_ptr <lambda_p::node>> & closed_a)
	: open (open_a),
	closed (closed_a),
	operation_m (operation_a)
{
}

void lambda_p_script::closure::operation::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p_script::segment <boost::shared_ptr <lambda_p::node>> parameters, std::vector <boost::shared_ptr <lambda_p::node>> & results)
{
	std::vector <size_t> open_l;
	for (size_t position (0), end (parameters.size ()); position != end; ++position)
	{
		auto val (parameters [position]);
		auto hole (boost::dynamic_pointer_cast <lambda_p_script::closure::hole> (val));
		if (hole.get () == nullptr)
		{
			closed [open [position]] = val;
		}
		else
		{
			open_l.push_back (open [position]);
		}
	}
	if (open_l.size () != 0)
	{
		results.push_back (boost::shared_ptr <lambda_p_script::closure::operation> (new lambda_p_script::closure::operation (operation_m, open_l, closed)));
	}
	else
	{
		operation_m->perform (errors_a, closed, results);
	}
}

size_t lambda_p_script::closure::operation::count ()
{
	return open.size ();
}

std::wstring lambda_p_script::closure::operation::name ()
{
	return std::wstring (L"lambda_p_script::closure::operation");
}