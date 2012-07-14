#include <mu/io/source.h>
#include <mu/io/builder.h>
#include <mu/llvm_/synthesizer/operation.h>
#include <mu/llvm_/module/node.h>
#include <mu/io/ast/cluster.h>
#include <mu/llvm_/module/print.h>
#include <mu/llvm_/module/verify.h>
#include <mu/io/builder.h>
#include <mu/llvm_/cluster/node.h>
#include <mu/llvm_/function/node.h>
#include <mu/llvm_/type/node.h>
#include <mu/llvm_/context/node.h>
#include <mu/script/context.h>
#include <mu/script/api.h>
#include <mu/script/extensions/node.h>
#include <mu/core/errors/error_list.h>
#include <mu/script/api.h>
#include <mu/script/values/operation.h>
#include <mu/llvm_/api.h>

#include <boost/bind.hpp>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Function.h>

#include <boost/make_shared.hpp>

#include <sstream>

#include <gtest/gtest.h>

#include <gc_cpp.h>

TEST (llvm_test, synthesizer_operation1)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	mu::io::builder builder (extensions->extensions);
	builder (L"");
	builder ();
	EXPECT_EQ (builder.errors->errors.empty (), true);
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_NE (ast, nullptr);
	auto module (new (GC) mu::llvm_::module::node (new llvm::Module (llvm::StringRef (), context_l)));	
	context->values [1] = module;
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (ctx.working_size (), 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 0);
	EXPECT_EQ (cluster->names.size (), 0);
	EXPECT_EQ (module->module->getFunctionList ().size (), 0);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module);
	auto valid2 (ctx ());
	EXPECT_EQ (valid2, true);
	EXPECT_EQ (ctx.working_size (), 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module);
	auto valid3 (ctx ());
	EXPECT_EQ (valid3, true);
}

TEST (llvm_test, synthesizer_operation2)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	builder (L"[fun-t .. i32] [add #i 32 d1 #i 32 d1]");
	builder ();
	EXPECT_EQ (builder.errors->errors.empty (), true);
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_NE (ast, nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_EQ (valid, true);
	EXPECT_EQ (ctx.working_size (), 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_NE (cluster, nullptr);
	EXPECT_EQ (cluster->routines.size (), 1);
	EXPECT_EQ (cluster->names.size (), 0);
	auto routine (cluster->routines [0]);
	EXPECT_EQ (routine->value()->getType ()->isPointerTy (), true);
	auto ptr (llvm::dyn_cast <llvm::PointerType> (routine->value ()->getType ()));
	EXPECT_NE (ptr, nullptr);
	EXPECT_EQ (ptr->getElementType ()->isFunctionTy (), true);
	EXPECT_EQ (module->getFunctionList ().size (), 1);
	mu::llvm_::module::print print;
	std::vector <mu::core::node *> a3;
	std::vector <mu::core::node *> r3;
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (context->values [1]);
	auto valid2 (ctx ());
	EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (context->values [1]);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation3)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	builder (L"[fun-t i32 .. i32] [~ :~]");
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;		
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 1);
	EXPECT_TRUE (cluster->names.size () == 0);
	EXPECT_TRUE (module->getFunctionList ().size () == 1);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
	EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation4)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	builder (L"[fun-t i32 i32 .. i32] [add :~]");
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;		
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 1);
	EXPECT_TRUE (cluster->names.size () == 0);
	EXPECT_TRUE (module->getFunctionList ().size () == 1);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
	EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation5)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));	
	mu::io::builder builder (extensions->extensions);
	builder (L"[fun-t] [~ :~]");
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;			
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 1);
	EXPECT_TRUE (cluster->names.size () == 0);
	EXPECT_TRUE (module->getFunctionList ().size () == 1);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
	EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation6)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	builder (L"[fun-t i32 i16 .. i32 i16] [~ :~]");
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;					
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 1);
	EXPECT_TRUE (cluster->names.size () == 0);
	EXPECT_TRUE (module->getFunctionList ().size () == 1);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
	EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation7)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	builder (L"[fun-t i32 [ptr i32]] [store :~]");
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;						
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 1);
	EXPECT_TRUE (cluster->names.size () == 0);
	EXPECT_TRUE (module->getFunctionList ().size () == 1);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
    EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation8)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	builder (L"[fun-t i32 [ptr i32]] [~ [store :~] [store :~]]");
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;						
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 1);
	EXPECT_TRUE (cluster->names.size () == 0);
	EXPECT_TRUE (module->getFunctionList ().size () == 1);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
    EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation9)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	std::wstringstream code;
	code << L"[fun-t i32 i32 .. i32] [add :~]";
	code << L"[fun-t i32 i32 .. i32] [and :~]";
	code << L"[fun-t i32 .. i32] [bitcast :~ i32]";
	code << L"[fun-t i32 i32 .. i32] [ashr :~]";
	code << L"[fun-t i32 i32 .. i1] [icmp eq :~]";
	code << L"[fun-t i32 i32 .. i1] [icmp ne :~]";
	code << L"[fun-t i32 i32 .. i1] [icmp sge :~]";
	code << L"[fun-t i32 i32 .. i1] [icmp sgt :~]";
	code << L"[fun-t i32 i32 .. i1] [icmp sle :~]";
	code << L"[fun-t i32 i32 .. i1] [icmp slt :~]";
	code << L"[fun-t i32 i32 .. i1] [icmp uge :~]";
	code << L"[fun-t i32 i32 .. i1] [icmp ugt :~]";
	code << L"[fun-t i32 i32 .. i1] [icmp ule :~]";
	code << L"[fun-t i32 i32 .. i1] [icmp ult :~]";
	code << L"[fun-t i64 .. [ptr i8]] [inttoptr :~ [ptr i8]]";
	code << L"[fun-t [ptr i32] .. i32] [load :~]";
	code << L"[fun-t i32 i32 .. i32] [lshr :~]";
	code << L"[fun-t i32 i32 .. i32] [mul :~]";
	code << L"[fun-t i32 i32 .. i32] [or :~]";
	code << L"[fun-t [ptr i8] .. i64] [ptrtoint :~ i64]";
	code << L"[fun-t i32 i32 .. i32] [sdiv :~]";
	code << L"[fun-t i32 .. i64] [sext :~ i64]";
	code << L"[fun-t i32 i32 .. i32] [shl :~]";
	code << L"[fun-t i32 i32 .. i32] [srem :~]";
	code << L"[fun-t i32 [ptr i32]] [store :~]";
	code << L"[fun-t i32 i32 .. i32] [sub :~]";
	code << L"[fun-t i32 .. i16] [trunc :~ i16]";
	code << L"[fun-t i32 i32 .. i32] [udiv :~]";
	code << L"[fun-t i32 i32 .. i32] [urem :~]";
	code << L"[fun-t i32 i32 .. i32] [xor :~]";
	code << L"[fun-t i32 .. i64] [zext :~ i64]";
	builder (code.str ());
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;					
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 31);
	EXPECT_TRUE (cluster->names.size () == 0);
	EXPECT_TRUE (module->getFunctionList ().size () == 31);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
    EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation10)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	std::wstringstream code;
	code << L"[fun-t .. [ptr [fun-t]]] [~ 2 ;; 1]";
	code << L"[fun-t] [~ ;; 2]";
	builder (code.str ());
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;				
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 2);
	EXPECT_TRUE (cluster->names.size () == 2);
	auto rout1 (cluster->routines [0]);
	auto rout2 (cluster->routines [1]);
	auto n1 (cluster->names.find (std::wstring (L"1")));
	auto n2 (cluster->names.find (std::wstring (L"2")));
	EXPECT_TRUE (n1 != cluster->names.end ());
	EXPECT_TRUE (n2 != cluster->names.end ());
	EXPECT_TRUE (n1->second == rout1);
	EXPECT_TRUE (n2->second == rout2);
	EXPECT_TRUE (module->getFunctionList ().size () == 2);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
    EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation11)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	std::wstringstream code;
	code << L"[fun-t .. i32] [call 2 ;; 1]";
	code << L"[fun-t .. i32] [~ #i 32 d42 ;; 2]";
	builder (code.str ());
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;		
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 2);
	EXPECT_TRUE (cluster->names.size () == 2);
	auto rout1 (cluster->routines [0]);
	auto rout2 (cluster->routines [1]);
	auto n1 (cluster->names.find (std::wstring (L"1")));
	auto n2 (cluster->names.find (std::wstring (L"2")));
	EXPECT_TRUE (n1 != cluster->names.end ());
	EXPECT_TRUE (n2 != cluster->names.end ());
	EXPECT_TRUE (n1->second == rout1);
	EXPECT_TRUE (n2->second == rout2);
	EXPECT_TRUE (module->getFunctionList ().size () == 2);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
    EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation12)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	llvm::LLVMContext context_l;
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;		
	std::wstringstream code;
	code << L"[fun-t .. [ptr i16]] [~ ` test_string]";
	builder (code.str ());
	builder ();
	auto ast (builder.cluster);
	EXPECT_TRUE (builder.errors->errors.empty ());
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
}

TEST (llvm_test, synthesizer_operation13)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	std::wstringstream code;
	code << L"[fun-t] [call 2 [call 2];; 1]";
	code << L"[fun-t] [~ ;; 2]";
	builder (code.str ());
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;			
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 2);
	EXPECT_TRUE (cluster->names.size () == 2);
	auto rout1 (cluster->routines [0]);
	auto rout2 (cluster->routines [1]);
	auto n1 (cluster->names.find (std::wstring (L"1")));
	auto n2 (cluster->names.find (std::wstring (L"2")));
	EXPECT_TRUE (n1 != cluster->names.end ());
	EXPECT_TRUE (n2 != cluster->names.end ());
	EXPECT_TRUE (n1->second == rout1);
	EXPECT_TRUE (n2->second == rout2);
	EXPECT_TRUE (module->getFunctionList ().size () == 2);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
    EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation14)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	auto context_p (static_cast <mu::llvm_::context::node *> (context->values [0]));
	auto module_p (static_cast <mu::llvm_::module::node *> (context->values [1]));
	mu::io::builder builder (extensions->extensions);
	std::wstringstream code;
	code << L"[fun-t] [call 3 [call 2];; 1]";
	code << L"[fun-t .. i16 i16] [~ #i 16 d0 #i 16 d0;; 2]";
	code << L"[fun-t i16 i16] [~ ;; 3]";
	builder (code.str ());
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new llvm::Module (llvm::StringRef (), context_l));	
	context_p->context = &context_l;
	module_p->module = module;			
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (valid);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto cluster (dynamic_cast <mu::llvm_::cluster::node *> (ctx.working (0)));
	EXPECT_TRUE (cluster != nullptr);
	EXPECT_TRUE (cluster->routines.size () == 3);
	EXPECT_TRUE (cluster->names.size () == 3);
	auto rout1 (cluster->routines [0]);
	auto rout2 (cluster->routines [1]);
	auto rout3 (cluster->routines [2]);
	auto n1 (cluster->names.find (std::wstring (L"1")));
	auto n2 (cluster->names.find (std::wstring (L"2")));
	auto n3 (cluster->names.find (std::wstring (L"3")));
	EXPECT_TRUE (n1 != cluster->names.end ());
	EXPECT_TRUE (n2 != cluster->names.end ());
	EXPECT_TRUE (n3 != cluster->names.end ());
	EXPECT_TRUE (n1->second == rout1);
	EXPECT_TRUE (n2->second == rout2);
	EXPECT_TRUE (n3->second == rout3);
	EXPECT_TRUE (module->getFunctionList ().size () == 3);
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::print);
	ctx.push (module_p);
	auto valid2 (ctx ());
    EXPECT_TRUE (valid2);
	EXPECT_TRUE (ctx.working_size () == 1);
	auto text (ctx.working (0));
	ctx.drop ();
	ctx.push (new (GC) mu::llvm_::module::verify);
	ctx.push (module_p);
	auto valid3 (ctx ());
	EXPECT_TRUE (valid3);
}

TEST (llvm_test, synthesizer_operation15)
{
	mu::script::extensions::node * extensions_ptr;
	mu::script::values::operation * context_ptr;
	mu::llvm_::api::binding (extensions_ptr, context_ptr);
	auto extensions (extensions_ptr);
	auto context (context_ptr);
	mu::io::builder builder (extensions->extensions);
	builder (L"[fun-t add] [~]");
	builder ();
	EXPECT_TRUE (builder.errors->errors.empty ());
	llvm::LLVMContext context_l;
	auto ast (builder.cluster);
	EXPECT_TRUE (ast != nullptr);
	auto module (new (GC) mu::llvm_::module::node (new llvm::Module (llvm::StringRef (), context_l)));	
	context->values [1] = module;
	mu::core::errors::errors errors (builder.errors);
	mu::script::context ctx (errors);
	ctx.push (new (GC) mu::llvm_::synthesizer::operation);
	ctx.push (ast);
	ctx.push (context);
	auto valid (ctx ());
	EXPECT_TRUE (!valid);
}