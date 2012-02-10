#include "node.h"

#include <lambda_p/errors/error_target.h>
#include <lambda_p_script/string/node.h>
#include <lambda_p_script/integer/node.h>
#include <lambda_p_script/routine.h>
#include <lambda_p_script/remapping.h>

#include <sstream>

lambda_p_script::cluster::node::node ()
	: remapping (new lambda_p_script::remapping)
{
}

lambda_p_script::cluster::node::node (std::vector <boost::shared_ptr <lambda_p_script::routine>> routines_a, boost::shared_ptr <lambda_p_script::remapping> remapping_a)
	: routines (routines_a),
	remapping (remapping_a)
{
}

lambda_p_script::cluster::node::node (std::map <std::wstring, boost::shared_ptr <lambda_p::routine>> names_a, std::vector <boost::shared_ptr <lambda_p_script::routine>> routines_a, boost::shared_ptr <lambda_p_script::remapping> remapping_a)
	: names (names_a),
	routines (routines_a),
	remapping (remapping_a)
{
}

void lambda_p_script::cluster::node::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters, std::vector <boost::shared_ptr <lambda_p::node>> & results)
{
	auto one (boost::dynamic_pointer_cast <lambda_p_script::string::node> (parameters [0]));
	if (one.get () != nullptr)
	{
		auto existing (names.find (one->string));
		if (existing != names.end ())
		{
			results.push_back (remapping->generated [existing->second]);
		}
		else
		{
			std::wstringstream message;
			message << L"Cluster does not contain routine named: ";
			message << one->string;
			(*errors_a) (message.str ());
		}
	}
	else
	{
		auto one (boost::dynamic_pointer_cast <lambda_p_script::integer::node> (parameters [0]));
		if (one.get () != nullptr)
		{
			if (routines.size () > one->value)
			{
				results.push_back (routines [one->value]);
			}
			else
			{
				std::wstringstream message;
				message << L"Cluster does not have a rounite numbered: ";
				message << one->value;
				(*errors_a) (message.str ());
			}
		}
		else
		{
			invalid_type (errors_a, parameters [0], 0);
		}
	}
}

size_t lambda_p_script::cluster::node::count ()
{
	return 1;
}