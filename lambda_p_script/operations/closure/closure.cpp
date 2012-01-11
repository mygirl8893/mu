#include "closure.h"

#include <lambda_p_script/nodes/hole.h>

lambda_p_script::operations::closure::closure::closure (size_t count_a, boost::shared_ptr <lambda_p_script::nodes::operation> operation_a)
	: closed (count_a + 1),
	open (count_a)
{
	closed [0] = operation_a;
	for (size_t i (0); i != count_a; ++i)
	{
		open [i] = i + 1;
	}
}

lambda_p_script::operations::closure::closure::closure (std::vector <size_t> & open_a, std::vector <boost::shared_ptr <lambda_p::node>> & closed_a)
	: open (open_a),
	closed (closed_a)
{
}

void lambda_p_script::operations::closure::closure::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, std::vector <boost::shared_ptr <lambda_p::node>> & parameters, std::vector <boost::shared_ptr <lambda_p::node>> & results)
{
	std::vector <size_t> open_l;
	for (size_t position (0), end (parameters.size ()); position != end; ++position)
	{
		auto val (parameters [position]);
		auto hole (boost::dynamic_pointer_cast <lambda_p_script::nodes::hole> (val));
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
		results.push_back (boost::shared_ptr <lambda_p_script::operations::closure::closure> (new lambda_p_script::operations::closure::closure (open_l, closed)));
	}
	else
	{
		assert (closed.size () > 0);
		auto operation (boost::static_pointer_cast <lambda_p_script::nodes::operation> (closed [0]));
		operation->perform (errors_a, closed, results);
	}
}

size_t lambda_p_script::operations::closure::closure::count ()
{
	return open.size ();
}

std::wstring lambda_p_script::operations::closure::closure::name ()
{
	return std::wstring (L"lambda_p_script::operations::closure::closure");
}