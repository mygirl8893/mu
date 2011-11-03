#include "binder.h"

#include <lambda_p/core/statement.h>
#include <lambda_p/core/association.h>
#include <lambda_p/errors/unexpected_result_count.h>
#include <lambda_p/errors/unexpected_argument_count.h>
#include <lambda_p/errors/unexpected_binder_type.h>
#include <lambda_p/errors/binder_string_error.h>
#include <lambda_p/errors/error_list.h>

#include <sstream>

void lambda_p::binder::binder::check_count (size_t result_count, size_t argument_count, lambda_p::core::statement * statement, lambda_p::errors::error_list & problems)
{
	if (statement->association->declarations.size () != result_count)
	{
		boost::shared_ptr <lambda_p::errors::unexpected_result_count> problem (new lambda_p::errors::unexpected_result_count (binder_name (), statement->association->declarations.size (), result_count));
		problems (problem);
	}
	if (statement->association->references.size () != argument_count)
	{
		boost::shared_ptr <lambda_p::errors::unexpected_argument_count> problem (new lambda_p::errors::unexpected_argument_count (binder_name (), statement->association->references.size (), argument_count));
		problems (problem);
	}
}

void lambda_p::binder::binder::add_error (wchar_t * message, lambda_p::errors::error_list & problems)
{
	add_error (std::wstring (message), problems);
}

void lambda_p::binder::binder::add_error (std::wstring message, lambda_p::errors::error_list & problems)
{
	problems (new lambda_p::errors::binder_string_error (binder_name (), message));
}

void lambda_p::binder::binder::unexpected_binder_type_error (size_t position, wchar_t * expected, lambda_p::errors::error_list & problems)
{
	problems (new lambda_p::errors::unexpected_binder_type (binder_name (), position, std::wstring (expected)));
}

void lambda_p::binder::binder::check_binder (boost::shared_ptr <lambda_p::binder::node> binder_a, size_t position, wchar_t * expected, lambda_p::errors::error_list & problems)
{
	if (binder_a.get () == nullptr)
	{
		unexpected_binder_type_error (position, expected, problems);
	}
}