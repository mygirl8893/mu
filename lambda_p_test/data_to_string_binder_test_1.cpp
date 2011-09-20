//
//  data_to_string_binder_test_1.cpp
//  lambda_p_test
//
//  Created by Colin LeMahieu on 9/19/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#define __STDC_LIMIT_MACROS

#include <lambda_p_test/data_to_string_binder_test_1.h>
#include <lambda_p_llvm/data_to_string_binder.h>
#include <lambda_p/core/routine.h>
#include <lambda_p_llvm/llvm_generation_context.h>
#include <lambda_p_llvm/llvm_value.h>

#include <llvm/LLVMContext.h>
#include <llvm/Module.h>

#include <boost/shared_ptr.hpp>

#include <sstream>

void lambda_p_test::data_to_string_binder_test_1::run ()
{
    ::lambda_p::core::routine routine;
    ::lambda_p::core::statement * parameters = routine.add_statement ();
    ::lambda_p::core::declaration * p1 = routine.add_declaration (0, 0);
    parameters->add_argument (p1);
    ::lambda_p::core::statement * statement = routine.add_statement ();
    ::lambda_p::core::reference * reference = routine.add_reference (0, 0, 1, 0);
    statement->add_argument (reference);
    ::lambda_p::core::declaration * declaration = routine.add_declaration (1, 1);
    statement->add_argument (declaration);
    ::lambda_p::core::data * data = routine.add_data (::std::wstring (L"Test string"), 1, 2);
    statement->add_argument (data);
    ::llvm::LLVMContext llvm_context;
    ::lambda_p_llvm::llvm_generation_context context (llvm_context);
    ::std::string module_string ("test");
    ::llvm::StringRef module_name (module_string);
    ::llvm::Module * module = new ::llvm::Module (module_name, llvm_context);
    context.module = module;
    ::lambda_p_llvm::data_to_string_binder binder (context);
    ::std::map < ::lambda_p::core::node *, ::boost::shared_ptr < ::lambda_p::binder::node_instance> > instances;
    ::std::wstringstream problems;
    binder.bind (statement, instances, problems);
    assert (problems.str ().size () == 0);
    assert (module->getGlobalList ().size () == 1);
    assert (instances.size () == 1);
    assert (instances [declaration].get () != NULL);
    assert (::boost::dynamic_pointer_cast < ::lambda_p_llvm::llvm_value> (instances [declaration]).get () != NULL);
}