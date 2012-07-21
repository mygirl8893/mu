#include <mu/core/errors/error_list.h>
#include <mu/llvm_/constant_string/create.h>
#include <mu/script/string/node.h>
#include <mu/llvm_/context/node.h>
#include <mu/llvm_/module/node.h>
#include <mu/llvm_/global_variable/node.h>
#include <mu/llvm_/pointer_type/node.h>
#include <mu/llvm_/integer_type/node.h>
#include <mu/script/context.h>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Constants.h>
#include <llvm/DerivedTypes.h>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (llvm_test, constant_string_create1)
{
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	llvm::LLVMContext context;
	auto module (new llvm::Module (llvm::StringRef (), context));
	mu::script::context ctx;
	ctx.push (new (GC) mu::llvm_::constant_string::create);
	ctx.push (new (GC) mu::llvm_::context::node (&context));
	ctx.push (new (GC) mu::llvm_::module::node (module));
	ctx.push (new (GC) mu::script::string::node (mu::string (U"test string")));
	auto valid (ctx ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (ctx.working_size (), 1);
	auto result (dynamic_cast <mu::llvm_::value::node *> (ctx.working (0)));
	EXPECT_NE (result, nullptr);
	//assert (result->global_variable ()->isConstant ());
	//assert (result->global_variable ()->hasInitializer ());
	//auto initializer (result->global_variable ()->getInitializer ());
	//assert (initializer != nullptr);
	//auto array (llvm::cast <llvm::ConstantArray> (initializer));
	//assert (array != nullptr);
	//auto elements (array->getType ()->getNumElements ());
	//assert (elements == 11);
	auto type (dynamic_cast <mu::llvm_::pointer_type::node *> (result->type));
	EXPECT_NE (type, nullptr);
	auto element_type (dynamic_cast <mu::llvm_::integer_type::node *> (type->element));
	EXPECT_NE (element_type, nullptr);
}