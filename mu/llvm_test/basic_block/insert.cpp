#include "insert.h"

#include <mu/llvm_/basic_block/node.h>
#include <mu/llvm_/instruction/node.h>
#include <mu/llvm_/basic_block/insert.h>
#include <mu/core/errors/error_list.h>
#include <mu/llvm_/pointer_type/node.h>
#include <mu/llvm_/integer_type/node.h>
#include <mu/script/context.h>

#include <boost/make_shared.hpp>

#include <llvm/LLVMContext.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>
#include <llvm/Function.h>
#include <llvm/Module.h>

void mu::llvm_test::basic_block::insert::run ()
{
	run_1 ();
}

void mu::llvm_test::basic_block::insert::run_1 ()
{
	llvm::LLVMContext context;
	auto module (new llvm::Module (llvm::StringRef (), context));
	auto function (llvm::Function::Create (llvm::FunctionType::get (llvm::Type::getVoidTy (context), false), llvm::GlobalValue::ExternalLinkage));
	module->getFunctionList ().push_back (function);
	auto block (boost::make_shared <mu::llvm_::basic_block::node> (llvm::BasicBlock::Create (context)));
	function->getBasicBlockList ().push_back (block->block);
	auto instruction (boost::make_shared <mu::llvm_::instruction::node> (new llvm::AllocaInst (llvm::Type::getInt1Ty (context)), boost::make_shared <mu::llvm_::pointer_type::node> (boost::make_shared <mu::llvm_::integer_type::node> (llvm::Type::getInt1Ty (context)))));
	mu::llvm_::basic_block::insert insert;
	auto errors (boost::make_shared <mu::core::errors::error_list> ());
	std::vector <boost::shared_ptr <mu::core::node>> arguments;
	std::vector <boost::shared_ptr <mu::core::node>> results;
	arguments.push_back (block);
	arguments.push_back (instruction);
	std::vector <std::type_info const *> stack;
	std::vector <mu::core::context> context_stack;
    auto ctx (mu::script::context (errors, arguments, results, stack, context_stack));
	insert (ctx);
	assert (errors->errors.empty ());
	assert (block->block->getInstList ().size () == 1);
}