//
//  load_binder.cpp
//  lambda_p_llvm
//
//  Created by Colin LeMahieu on 9/24/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <lambda_p_llvm/load_inst_binder.h>
#include <lambda_p/core/statement.h>
#include <lambda_p_llvm/value.h>
#include <lambda_p_llvm/generation_context.h>
#include <lambda_p/core/association.h>
#include <lambda_p/binder/routine_instances.h>

#include <llvm/Type.h>
#include <llvm/Instructions.h>
#include <llvm/BasicBlock.h>

#include <sstream>

lambda_p_llvm::load_inst_binder::load_inst_binder (::lambda_p_llvm::generation_context & context_a)
: context (context_a)
{
}

void lambda_p_llvm::load_inst_binder::bind (::lambda_p::core::statement * statement, ::lambda_p::binder::routine_instances & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems)
{
	check_count_only_references (1, 1, statement, problems);
	if (problems.empty ())
	{
		size_t pointer_node (statement->association->parameters [0]);
        ::boost::shared_ptr < ::lambda_p::binder::node_instance> pointer_instance (instances [pointer_node]);
        ::boost::shared_ptr < ::lambda_p_llvm::value> pointer (::boost::dynamic_pointer_cast < ::lambda_p_llvm::value> (pointer_instance));
        if (pointer.get () != NULL)
        {
            if (pointer->value_m->getType ()->isPointerTy ())
            {
                ::llvm::LoadInst * load (new ::llvm::LoadInst (pointer->value_m));
                context.block->getInstList ().push_back (load);
                ::boost::shared_ptr < ::lambda_p_llvm::value> value (new ::lambda_p_llvm::value (load));
				instances [statement->association->results [0]] = value;
            }
            else
            {
                add_error (::std::wstring (L"load_inst_binder expects argument 1 to be a pointer"), problems);
            }
        }
        else
        {
            add_error (::std::wstring (L"load_inst_binder expects argument 1 to be an llvm value"), problems);
        }
    }
}

::std::wstring lambda_p_llvm::load_inst_binder::binder_name ()
{
	return ::std::wstring (L"load_inst_binder");
}