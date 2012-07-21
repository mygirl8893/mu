#include <mu/core/errors/error_list.h>
#include <mu/llvm_/module/node.h>
#include <mu/llvm_/linker/link_modules.h>
#include <mu/script/context.h>

#include <llvm/LLVMContext.h>
#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Module.h>

#include <vector>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (llvm_test, link_modules1)
{
	llvm::LLVMContext context;
	std::vector <llvm::Type *> types;
	auto module (new (GC) mu::llvm_::module::node (new llvm::Module (llvm::StringRef ("test1"), context)));	
	auto module1 (new (GC) mu::llvm_::module::node (new llvm::Module (llvm::StringRef ("test1"), context)));	
	auto function1 (llvm::Function::Create (llvm::FunctionType::get (llvm::Type::getVoidTy (context), types, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "a", module1->module));
	auto module2 (new (GC) mu::llvm_::module::node (new llvm::Module (llvm::StringRef ("test2"), context)));	
	auto function2 (llvm::Function::Create (llvm::FunctionType::get (llvm::Type::getVoidTy (context), types, false), llvm::GlobalValue::LinkageTypes::ExternalLinkage, "b", module2->module));
	mu::llvm_::linker::link_modules link_modules;
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	mu::script::context ctx;
	ctx.push (new (GC) mu::llvm_::linker::link_modules);
	ctx.push (module);
	ctx.push (module1);
	auto valid (ctx ());
	EXPECT_EQ (valid, true);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::linker::link_modules);
	ctx.push (module);
	ctx.push (module2);
	auto valid2 (ctx ());
	EXPECT_EQ (valid2, true);
	EXPECT_NE (module->module->getFunction ("a"), nullptr);
	EXPECT_NE (module->module->getFunction ("b"), nullptr);
}