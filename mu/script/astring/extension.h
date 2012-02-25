#pragma once

#include <mu/io/analyzer/extensions/extension.h>

namespace mu
{
	namespace script
	{
		namespace astring
		{
			class extension : public mu::io::analyzer::extensions::extension
			{
			public:
				void operator () (boost::shared_ptr <mu::core::errors::error_target> errors_a, mu::io::analyzer::expression & expression_a) override;
			};
		}
	}
}