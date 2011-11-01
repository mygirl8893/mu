#pragma once

#include <lambda_p/binder/node.h>

#include <boost/shared_ptr.hpp>

#include <string>

namespace lambda_p
{
	namespace core
	{
		class statement;
	}
	namespace errors
	{
		class error_list;
	}
	namespace binder
	{
		class node_list;
		class binder : virtual public lambda_p::binder::node
		{
		public:
			virtual void bind (lambda_p::core::statement * statement, lambda_p::binder::node_list & nodes, lambda_p::errors::error_list & problems) = 0;
			virtual std::wstring binder_name () = 0;
		protected:
			void add_error (wchar_t * message, lambda_p::errors::error_list & problems);
			void add_error (std::wstring message, lambda_p::errors::error_list & problems);
			void unexpected_binder_type_error (size_t position, wchar_t * expected, lambda_p::errors::error_list & problems);
			// Checks for the specified number of results and arguments
			void check_count (size_t result_count, size_t argument_count, lambda_p::core::statement * statement, lambda_p::errors::error_list & problems);
		};
	}
}

