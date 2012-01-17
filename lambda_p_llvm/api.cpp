#include "api.h"

#include <lambda_p_io/analyzer/global.h>
#include <lambda_p_llvm/context/create.h>
#include <lambda_p_llvm/module/add_package.h>
#include <lambda_p_llvm/module/assemble.h>
#include <lambda_p_llvm/module/create.h>
#include <lambda_p_llvm/module/get_function.h>
#include <lambda_p_llvm/execution_engine/create_jit.h>
#include <lambda_p_llvm/execution_engine/run_function.h>
#include <lambda_p_llvm/module/get_package.h>
#include <lambda_p_llvm/linker/link_modules.h>

lambda_p_llvm::api::api ()
{
	extensions.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extension>>::value_type (std::wstring (L"llvm/context/create"), boost::shared_ptr <lambda_p_io::analyzer::extension> (new lambda_p_io::analyzer::global (boost::shared_ptr <lambda_p::node> (new lambda_p_llvm::context::create)))));
	extensions.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extension>>::value_type (std::wstring (L"llvm/execution_engine/create_jit"), boost::shared_ptr <lambda_p_io::analyzer::extension> (new lambda_p_io::analyzer::global (boost::shared_ptr <lambda_p::node> (new lambda_p_llvm::execution_engine::create_jit)))));
	extensions.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extension>>::value_type (std::wstring (L"llvm/execution_engine/run_function"), boost::shared_ptr <lambda_p_io::analyzer::extension> (new lambda_p_io::analyzer::global (boost::shared_ptr <lambda_p::node> (new lambda_p_llvm::execution_engine::run_function)))));
	extensions.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extension>>::value_type (std::wstring (L"llvm/linker/link_modules"), boost::shared_ptr <lambda_p_io::analyzer::extension> (new lambda_p_io::analyzer::global (boost::shared_ptr <lambda_p::node> (new lambda_p_llvm::linker::link_modules)))));
	extensions.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extension>>::value_type (std::wstring (L"llvm/module/add_package"), boost::shared_ptr <lambda_p_io::analyzer::extension> (new lambda_p_io::analyzer::global (boost::shared_ptr <lambda_p::node> (new lambda_p_llvm::module::add_package)))));
	extensions.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extension>>::value_type (std::wstring (L"llvm/module/assemble"), boost::shared_ptr <lambda_p_io::analyzer::extension> (new lambda_p_io::analyzer::global (boost::shared_ptr <lambda_p::node> (new lambda_p_llvm::module::assemble)))));
	extensions.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extension>>::value_type (std::wstring (L"llvm/module/create"), boost::shared_ptr <lambda_p_io::analyzer::extension> (new lambda_p_io::analyzer::global (boost::shared_ptr <lambda_p::node> (new lambda_p_llvm::module::create)))));
	extensions.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extension>>::value_type (std::wstring (L"llvm/module/get_function"), boost::shared_ptr <lambda_p_io::analyzer::extension> (new lambda_p_io::analyzer::global (boost::shared_ptr <lambda_p::node> (new lambda_p_llvm::module::get_function)))));
	extensions.insert (std::map <std::wstring, boost::shared_ptr <lambda_p_io::analyzer::extension>>::value_type (std::wstring (L"llvm/module/get_package"), boost::shared_ptr <lambda_p_io::analyzer::extension> (new lambda_p_io::analyzer::global (boost::shared_ptr <lambda_p::node> (new lambda_p_llvm::module::get_package)))));
}
