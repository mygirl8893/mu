#include "merge.h"

#include <lambda_p/errors/error_target.h>
#include <lambda_p_llvm/module/node.h>

#include <llvm/Linker.h>

#include <sstream>

void lambda_p_llvm::module::merge::perform (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters, std::vector <boost::shared_ptr <lambda_p::node>> & results)
{
	if (parameters.size () > 0)
	{
		auto one (boost::dynamic_pointer_cast <lambda_p_llvm::module::node> (parameters [0]));
		if (one.get () != nullptr)
		{
			size_t position (1);
			for (auto i (parameters.begin () + 1), j (parameters.end () + 0); i != j; ++i, ++position)
			{
				auto module (boost::dynamic_pointer_cast <lambda_p_llvm::module::node> (*i));
				if (module.get () != nullptr)
				{
					std::string errors;
					llvm::Linker::LinkModules (one->module, module->module, llvm::Linker::DestroySource, &errors);
					if (!errors.empty ())
					{
						std::wstring errors_l (errors.begin (), errors.end ());
						(*errors_a) (errors_l);
					}
				}
				else
				{
					invalid_type (errors_a, parameters [position], position);
				}
			}
		}
		else
		{
			invalid_type (errors_a, parameters [0], 0);
		}
	}
	else
	{
		std::wstringstream message;
		message << L"Operation: ";
		message << name ();
		message << L" requires at least 1 argument";
		(*errors_a) (message.str ());
	}
}