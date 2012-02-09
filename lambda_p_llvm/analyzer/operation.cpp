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
#include <lambda_p_llvm/instructions/call.h>
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
#include <lambda_p_llvm/apint/extension.h>
#include <lambda_p_script/string/extension.h>
#include <lambda_p_io/ast/end.h>
#include <lambda_p_io/ast/cluster.h>
#include <lambda_p_llvm/value/get_type.h>
#include <lambda_p_script_io/builder.h>
#include <lambda_p_llvm/basic_block/insert.h>
#include <lambda_p_llvm/basic_block/node.h>
#include <lambda_p_script/cluster/node.h>
#include <lambda_p_script/routine.h>
#include <lambda_p_llvm/basic_block/instruction_insert.h>
#include <lambda_p_script/values/create.h>
#include <lambda_p_llvm/function_type/create.h>
#include <lambda_p_llvm/integer_type/create.h>
#include <lambda_p_script/closure/single.h>
#include <lambda_p_llvm/context/node.h>
#include <lambda_p_llvm/constant_int/create.h>
#include <lambda_p_script/identity/operation.h>
#include <lambda_p_llvm/predicate/node.h>
#include <lambda_p_llvm/pointer_type/create.h>

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

lambda_p_llvm::analyzer::operation::operation ()
	: extensions (new lambda_p_io::analyzer::extensions::extensions)
{
	context.context_m = boost::make_shared <lambda_p_llvm::context::node> (nullptr);
	context.block = boost::make_shared <lambda_p_llvm::basic_block::node> (nullptr);
	extensions->extensions_m [std::wstring (L"#")] = boost::make_shared <lambda_p_llvm::apint::extension> ();
	extensions->extensions_m [std::wstring (L"{")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_script::values::create> ());
	extensions->extensions_m [std::wstring (L"~")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_script::identity::operation> ());
	auto function_type (boost::make_shared <lambda_p_script::closure::single> (boost::make_shared <lambda_p_llvm::function_type::create> ()));
	function_type->closed.push_back (context.context_m);
	extensions->extensions_m [std::wstring (L"fun-t")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (function_type);
	auto integer_type (boost::make_shared <lambda_p_script::closure::single> (boost::make_shared <lambda_p_llvm::integer_type::create> ()));
	integer_type->closed.push_back (context.context_m);
	extensions->extensions_m [std::wstring (L"int-t")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (integer_type);
	auto constant_int (boost::make_shared <lambda_p_script::closure::single> (boost::make_shared <lambda_p_llvm::constant_int::create> ()));
	constant_int->closed.push_back (context.context_m);
	extensions->extensions_m [std::wstring (L"int-c")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (constant_int);	
	extensions->extensions_m [std::wstring (L"add")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::add> ()));
	extensions->extensions_m [std::wstring (L"and")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::and> ()));
	extensions->extensions_m [std::wstring (L"ashr")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::ashr> ()));
	extensions->extensions_m [std::wstring (L"bitcast")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::bitcast> ()));
	extensions->extensions_m [std::wstring (L"call")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::call> ()));
	extensions->extensions_m [std::wstring (L"eq")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::predicate::node> (llvm::CmpInst::Predicate::ICMP_EQ));
	//extensions->extensions_m [std::wstring (L"cmpxchg")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::cmpxchg> ()));
	extensions->extensions_m [std::wstring (L"icmp")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::icmp> ()));
	extensions->extensions_m [std::wstring (L"inttoptr")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::inttoptr> ()));
	extensions->extensions_m [std::wstring (L"load")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::load> ()));
	extensions->extensions_m [std::wstring (L"lshr")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::lshr> ()));
	extensions->extensions_m [std::wstring (L"mul")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::mul> ()));
	extensions->extensions_m [std::wstring (L"ne")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::predicate::node> (llvm::CmpInst::Predicate::ICMP_NE));
	extensions->extensions_m [std::wstring (L"or")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::or> ()));
	extensions->extensions_m [std::wstring (L"ptr")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::pointer_type::create> ());
	extensions->extensions_m [std::wstring (L"ptrtoint")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::ptrtoint> ()));
	extensions->extensions_m [std::wstring (L"sdiv")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::sdiv> ()));
	extensions->extensions_m [std::wstring (L"sext")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::sext> ()));
	extensions->extensions_m [std::wstring (L"sge")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::predicate::node> (llvm::CmpInst::Predicate::ICMP_SGE));
	extensions->extensions_m [std::wstring (L"sgt")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::predicate::node> (llvm::CmpInst::Predicate::ICMP_SGT));
	extensions->extensions_m [std::wstring (L"shl")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::shl> ()));
	extensions->extensions_m [std::wstring (L"sle")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::predicate::node> (llvm::CmpInst::Predicate::ICMP_SLE));
	extensions->extensions_m [std::wstring (L"slt")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::predicate::node> (llvm::CmpInst::Predicate::ICMP_SLT));
	extensions->extensions_m [std::wstring (L"srem")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::srem> ()));
	extensions->extensions_m [std::wstring (L"store")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::store> ()));
	//extensions->extensions_m [std::wstring (L"structure")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::types::structure> ());
	extensions->extensions_m [std::wstring (L"sub")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::sub> ()));
	extensions->extensions_m [std::wstring (L"trunc")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::trunc> ()));
	extensions->extensions_m [std::wstring (L"udiv")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::udiv> ()));
	extensions->extensions_m [std::wstring (L"uge")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::predicate::node> (llvm::CmpInst::Predicate::ICMP_UGE));
	extensions->extensions_m [std::wstring (L"ugt")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::predicate::node> (llvm::CmpInst::Predicate::ICMP_UGT));
	extensions->extensions_m [std::wstring (L"ule")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::predicate::node> (llvm::CmpInst::Predicate::ICMP_ULE));
	extensions->extensions_m [std::wstring (L"ult")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::predicate::node> (llvm::CmpInst::Predicate::ICMP_ULT));
	extensions->extensions_m [std::wstring (L"urem")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::urem> ()));
	//extensions->extensions_m [std::wstring (L"void")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::types::void_c> ());
	extensions->extensions_m [std::wstring (L"xor")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::xor> ()));
	extensions->extensions_m [std::wstring (L"zext")] = boost::make_shared <lambda_p_io::analyzer::extensions::global> (boost::make_shared <lambda_p_llvm::basic_block::instruction_insert> (context.block, boost::make_shared <lambda_p_llvm::instructions::zext> ()));
}

void lambda_p_llvm::analyzer::operation::operator () (boost::shared_ptr <lambda_p::errors::error_target> errors_a, lambda_p::segment <boost::shared_ptr <lambda_p::node>> parameters, std::vector <boost::shared_ptr <lambda_p::node>> & results)
{
	auto one (boost::dynamic_pointer_cast <lambda_p_io::ast::cluster> (parameters [0]));
	if (one.get () != nullptr)
	{
		auto function (boost::bind (&lambda_p_llvm::analyzer::operation::add, this, &results, _1));
		lambda_p_io::analyzer::analyzer analyzer (function, errors_a, extensions);
		for (auto i (one->expressions.begin ()), j (one->expressions.end ()); i != j; ++i)
		{
			analyzer.input (*i);
		}
		lambda_p::context context;
		if (one->expressions.size () > 0)
		{
			context = lambda_p::context (one->expressions [one->expressions.size () - 1]->context.last, one->expressions [one->expressions.size () - 1]->context.last);
		}
		analyzer.input (boost::make_shared <lambda_p_io::ast::end> (context));
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
	results->push_back (cluster_a);
}