//
//  echo_binder_test.cpp
//  lambda_p_test
//
//  Created by Colin LeMahieu on 9/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <lambda_p_test/echo_binder_test.h>

#include <lambda_p_repl/echo_binder.h>
#include <lambda_p_llvm/generation_context.h>
#include <lambda_p_llvm/wprintf_function.h>
#include <lambda_p/core/routine.h>
#include <lambda_p/core/statement.h>
#include <lambda_p_llvm/constant_wstring.h>
#include <lambda_p_llvm/llvm_value.h>

#include <llvm/LLVMContext.h>
#include <llvm/Function.h>
#include <llvm/Module.h>
#include <llvm/DerivedTypes.h>

#include <sstream>

void lambda_p_test::echo_binder_test::run ()
{
    ::llvm::LLVMContext llvm_context;
    ::std::string module_string ("test");
    ::llvm::StringRef module_name (module_string);
    ::llvm::Module * module = new ::llvm::Module (module_name, llvm_context);
    ::lambda_p_llvm::generation_context context (llvm_context, module, NULL);
    ::lambda_p_llvm::wprintf_function wprintf (context);
    ::lambda_p::core::routine routine;
    ::lambda_p::core::statement * parameters = routine.add_statement ();
    ::lambda_p::core::declaration * p1 = routine.add_declaration (0, 0);
    parameters->add_argument (p1);
    ::lambda_p::core::declaration * p2 = routine.add_declaration (0, 1);
    parameters->add_argument (p2);
    ::lambda_p::core::statement * statement = routine.add_statement ();
    ::lambda_p::core::reference * reference = routine.add_reference (0, 0, 1, 0);
    statement->add_argument (reference);
    ::lambda_p::core::reference * str = routine.add_reference (0, 1, 1, 1);
    statement->add_argument (str);
    ::llvm::Function * start (::llvm::Function::Create (::llvm::FunctionType::get (::llvm::Type::getVoidTy (llvm_context), false), ::llvm::GlobalValue::ExternalLinkage));
    module->getFunctionList ().push_back (start);
    ::llvm::BasicBlock * block (::llvm::BasicBlock::Create (llvm_context));
    start->getBasicBlockList ().push_back (block);
    context.block = block;
    ::lambda_p_repl::echo_binder binder (wprintf.wprintf, context);
    ::std::map < ::lambda_p::core::node *, ::boost::shared_ptr < ::lambda_p::binder::node_instance> > instances;
    ::std::wstring raw_string (L"test_echo");
    ::lambda_p_llvm::constant_wstring string (context, raw_string);
    ::boost::shared_ptr < ::lambda_p_llvm::llvm_value> value (new ::lambda_p_llvm::llvm_value (string.value));
    instances [str] = value;
    ::std::wstringstream problems;
    binder.bind (statement, instances, problems);
    assert (problems.str ().size () == 0);
    assert (block->getInstList ().size () == 1);
}