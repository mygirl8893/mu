#include "operation.h"

#include <lambda_p_io/ast/expression.h>
#include <lambda_p_io/analyzer/analyzer.h>
#include <lambda_p/routine.h>
#include <lambda_p_io/analyzer/extensions/extensions.h>
#include <lambda_p_io/analyzer/extensions/global.h>
#include <lambda_p_llvm/instructions/add.h>
#include <lambda_p_llvm/instructions/and.h>
#include <lambda_p_llvm/instructions/ashr.h>
#include <lambda_p_llvm/instructions/bitcast.h>
#include <lambda_p_llvm/instructions/cmpxchg.h>
#include <lambda_p_llvm/instructions/icmp.h>
#include <lambda_p_llvm/instructions/inttoptr.h>
#include <lambda_p_llvm/instructions/load.h>
#include <lambda_p_llvm/instructions/lshr.h>
#include <lambda_p_llvm/instructions/mul.h>
#include <lambda_p_llvm/instructions/or.h>
#include <lambda_p_llvm/instructions/ptrtoint.h>
#include <lambda_p_llvm/instructions/sdiv.h>
#include <lambda_p_llvm/instructions/sext.h>
#include <lambda_p_llvm/instructions/shl.h>
#include <lambda_p_llvm/instructions/srem.h>
#include <lambda_p_llvm/instructions/store.h>
#include <lambda_p_llvm/instructions/sub.h>
#include <lambda_p_llvm/instructions/trunc.h>
#include <lambda_p_llvm/instructions/udiv.h>
#include <lambda_p_llvm/instructions/urem.h>
#include <lambda_p_llvm/instructions/xor.h>
#include <lambda_p_llvm/instructions/zext.h>
#include <lambda_p_llvm/types/integer.h>
#include <lambda_p_llvm/types/pointer.h>
#include <lambda_p_llvm/types/structure.h>
#include <lambda_p_llvm/types/void_c.h>
#include <lambda_p_llvm/constants/integer.h>
#include <lambda_p_llvm/apint/extension.h>
#include <lambda_p_script/string/extension.h>
#include <lambda_p_io/ast/end.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

lambda_p_llvm::analyzer::operation::operation ()
	: extensions (new lambda_p_io::analyzer::extensions::extensions)
{
	extensions->extensions_m [std::wstring (L"#")] = boost::make_shared <lambda_p_llvm::apint::extension> ();
	extensions->extensions_m [std::wstring (L"`")] = boost::make_shared <lambda_p_script::string::extension> ();
	extensions->extensions_m [std::wstring (L"add")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::add> ());
	extensions->extensions_m [std::wstring (L"and")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::and> ());
	extensions->extensions_m [std::wstring (L"ashr")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::ashr> ());
	extensions->extensions_m [std::wstring (L"bitcast")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::bitcast> ());
	extensions->extensions_m [std::wstring (L"cmpxchg")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::cmpxchg> ());
	extensions->extensions_m [std::wstring (L"icmp")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::icmp> ());
	extensions->extensions_m [std::wstring (L"integer")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::types::integer> ());
	extensions->extensions_m [std::wstring (L"integerc")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::constants::integer> ());
	extensions->extensions_m [std::wstring (L"inttoptr")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::inttoptr> ());
	extensions->extensions_m [std::wstring (L"load")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::load> ());
	extensions->extensions_m [std::wstring (L"lshr")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::lshr> ());
	extensions->extensions_m [std::wstring (L"mul")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::mul> ());
	extensions->extensions_m [std::wstring (L"or")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::or> ());
	extensions->extensions_m [std::wstring (L"pointer")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::types::pointer> ());
	extensions->extensions_m [std::wstring (L"ptrtoint")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::ptrtoint> ());
	extensions->extensions_m [std::wstring (L"sdiv")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::sdiv> ());
	extensions->extensions_m [std::wstring (L"sext")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::sext> ());
	extensions->extensions_m [std::wstring (L"shl")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::shl> ());
	extensions->extensions_m [std::wstring (L"srem")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::srem> ());
	extensions->extensions_m [std::wstring (L"store")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::store> ());
	extensions->extensions_m [std::wstring (L"structure")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::types::structure> ());
	extensions->extensions_m [std::wstring (L"sub")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::sub> ());
	extensions->extensions_m [std::wstring (L"trunc")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::trunc> ());
	extensions->extensions_m [std::wstring (L"udiv")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::udiv> ());
	extensions->extensions_m [std::wstring (L"urem")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::urem> ());
	extensions->extensions_m [std::wstring (L"void")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::types::void_c> ());
	extensions->extensions_m [std::wstring (L"xor")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::xor> ());
	extensions->extensions_m [std::wstring (L"zext")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::instructions::zext> ());
}

void lambda_p_llvm::analyzer::operation::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters, std::vector <boost::shared_ptr <lambda_p::node>> & results)
{
	auto one (boost::dynamic_pointer_cast <lambda_p_io::ast::expression> (parameters [0]));
	if (one.get () != nullptr)
	{
		auto function (boost::bind (&lambda_p_llvm::analyzer::operation::add, this, &results, _1));
		lambda_p_io::analyzer::analyzer analyzer (function, errors_a, extensions);
		analyzer.input (one);
		analyzer.input (boost::make_shared <lambda_p_io::ast::end> (lambda_p::context (one->context.last, one->context.last)));
	}
	else
	{
		invalid_type (errors_a, parameters [0], 0);
	}
}

size_t lambda_p_llvm::analyzer::operation::count ()
{
	return 1;
}

void lambda_p_llvm::analyzer::operation::add (std::vector <boost::shared_ptr <lambda_p::node>> * results, boost::shared_ptr <lambda_p::cluster> cluster_a)
{
	assert (cluster_a->routines.size () == 1);
	results->push_back (cluster_a->routines [0]);
}