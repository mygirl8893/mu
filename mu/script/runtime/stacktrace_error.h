#include <mu/core/errors/error.h>

#include <vector>

#include <boost/shared_ptr.hpp>

namespace mu
{
	namespace script
	{
		class operation;
		class context;
		namespace debugging
		{
			class call_info;
		}
		namespace runtime
		{
			class stacktrace_error : public mu::core::errors::error
			{
			public:
				stacktrace_error (mu::script::context & context_a, boost::shared_ptr <mu::core::errors::error> error_a);
				void string (std::wostream & stream) override;
				std::vector <boost::shared_ptr <mu::script::debugging::call_info>> stack;
				boost::shared_ptr <mu::core::errors::error> error;
			};
		}
	}
}
