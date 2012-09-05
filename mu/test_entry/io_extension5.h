#pragma once

#include <mu/io/analyzer_state.h>

namespace mu
{
    namespace io
    {
        namespace keywording
        {
            class keywording;
        }
    }
	namespace io_test
	{
		class extension5 : public mu::io::keywording::state
		{
		public:
            extension5 (mu::io::keywording::keywording & keywording_a);
            void operator () (mu::io::tokens::token * token_a) override;
            static bool const dominating = true;
            mu::io::keywording::keywording & keywording;
		};
	}
}
