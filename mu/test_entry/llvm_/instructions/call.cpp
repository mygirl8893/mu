#include <mu/core/errors/error_list.h>
#include <mu/llvm_/instructions/call.h>
#include <mu/llvm_/value/node.h>
#include <mu/llvm_/pointer_type/node.h>
#include <mu/llvm_/context/node.h>
#include <mu/llvm_/function_type/node.h>
#include <mu/llvm_/integer_type/node.h>
#include <mu/llvm_/constant_int/node.h>
#include <mu/llvm_/void_type/node.h>
#include <mu/llvm_/set_type/node.h>
#include <mu/llvm_/instruction/node.h>
#include <mu/script/context.h>

#include <llvm/Constants.h>
#include <llvm/LLVMContext.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Module.h>

#include <gtest/gtest.h>

#include <gc_cpp.h>
#include <gc_allocator.h>

TEST (llvm_test, instructions_call1)
{
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	llvm::LLVMContext context;
	auto ctx (new (GC) mu::llvm_::context::node (&context));
	std::vector <mu::llvm_::type::node *, gc_allocator <mu::llvm_::type::node *>> arguments;
	arguments.push_back (new (GC) mu::llvm_::integer_type::node (llvm::Type::getInt1Ty (context)));
	auto type (new (GC) mu::llvm_::pointer_type::node (new (GC) mu::llvm_::function_type::node (ctx, arguments, new (GC) mu::llvm_::integer_type::node (llvm::Type::getInt1Ty (context)))));
	mu::script::context ctx2 (errors);
	ctx2.push (new (GC) mu::llvm_::instructions::call);
	ctx2.push (new (GC) mu::llvm_::value::node (llvm::ConstantPointerNull::get (type->pointer_type ()), type));
	auto valid (ctx2 ());
	EXPECT_EQ (!valid, true);
}

TEST (llvm_test, instructions_call2)
{
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	llvm::LLVMContext context;
	auto ctx (new (GC) mu::llvm_::context::node (&context));
	std::vector <mu::llvm_::type::node *, gc_allocator <mu::llvm_::type::node *>> arguments;
	arguments.push_back (new (GC) mu::llvm_::integer_type::node (llvm::Type::getInt1Ty (context)));
	auto type (new (GC) mu::llvm_::pointer_type::node (new (GC) mu::llvm_::function_type::node (ctx, arguments, new (GC) mu::llvm_::integer_type::node (llvm::Type::getInt1Ty (context)))));
	mu::script::context ctx2 (errors);
	ctx2.push (new (GC) mu::llvm_::instructions::call);
	ctx2.push (new (GC) mu::llvm_::value::node (llvm::ConstantPointerNull::get (type->pointer_type ()), type));
	ctx2.push (new (GC) mu::llvm_::constant_int::node (llvm::ConstantInt::get (llvm::Type::getInt16Ty (context), 0, false), new (GC) mu::llvm_::integer_type::node (llvm::Type::getInt16Ty (context))));
	auto valid (ctx2 ());
	EXPECT_EQ (!valid, true);
}

TEST (llvm_test, instructions_call3)
{
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	llvm::LLVMContext context;
	auto ctx (new (GC) mu::llvm_::context::node (&context));
	auto type (new (GC) mu::llvm_::pointer_type::node (new (GC) mu::llvm_::function_type::node (ctx, std::vector <mu::llvm_::type::node *, gc_allocator <mu::llvm_::type::node *>> (), new (GC) mu::llvm_::integer_type::node (llvm::Type::getInt1Ty (context)))));
	mu::script::context ctx2;
	ctx2.push (new (GC) mu::llvm_::instructions::call);
	ctx2.push (new (GC) mu::llvm_::value::node (llvm::ConstantPointerNull::get (type->pointer_type ()), type));
	auto valid (ctx2 ());
	assert (valid);
	assert (ctx2.working_size () == 1);
	llvm::Module module (llvm::StringRef (), context);
	auto function (llvm::Function::Create (llvm::FunctionType::get (llvm::Type::getVoidTy (context), false), llvm::GlobalValue::ExternalLinkage, llvm::Twine (), &module));
	auto block (llvm::BasicBlock::Create (context, llvm::Twine (), function));
	block->getInstList ().push_back (static_cast <mu::llvm_::instruction::node *> (ctx2.working (0))->instruction ());
}

TEST (llvm_test, instructions_call4)
{
	mu::core::errors::errors errors (new (GC) mu::core::errors::error_list);
	llvm::LLVMContext context;
	auto ctx (new (GC) mu::llvm_::context::node (&context));
	auto type (new (GC) mu::llvm_::pointer_type::node (new (GC) mu::llvm_::function_type::node (ctx, std::vector <mu::llvm_::type::node *, gc_allocator <mu::llvm_::type::node *>> (), new (GC) mu::llvm_::void_type::node (ctx))));
	mu::script::context ctx2;
	ctx2.push (new (GC) mu::llvm_::instructions::call);
	auto valid (ctx2 ());
	EXPECT_EQ (!valid, true);
}