#include <typeinfo>

namespace mu
{
	namespace script
	{
		namespace runtime
		{
			class context;
			class trace_registration
			{
			public:
				trace_registration (mu::script::runtime::context & context_a, std::type_info const * info_a);
				~trace_registration ();
				mu::script::runtime::context & context;
			};
		}
	}
}