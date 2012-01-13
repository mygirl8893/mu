#pragma once

#include <lambda_p/visitor.h>

#include <boost/shared_ptr.hpp>

#include <map>

namespace lambda_p
{
	class expression;
	class node;
}
namespace lambda_p_script
{
	class call;
}
namespace lambda_p_script_io
{
	class generator;
	class call : public lambda_p::visitor
	{
	public:
		call (std::map <boost::shared_ptr <lambda_p::expression>, size_t> & reservations_a, boost::shared_ptr <lambda_p_script::call> call_a, boost::shared_ptr <lambda_p::node> node_a);
		void operator () (lambda_p::call * call_a) override;
		void operator () (lambda_p::set * set_a) override;
		void operator () (lambda_p::reference * reference_a) override;
		void operator () (lambda_p::node * node_a) override;
		boost::shared_ptr <lambda_p::node> node;
		boost::shared_ptr <lambda_p_script::call> call_m;
		std::map <boost::shared_ptr <lambda_p::expression>, size_t> & reservations;
	};
}

