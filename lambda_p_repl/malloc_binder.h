//
//  malloc_binder.h
//  lambda_p_repl
//
//  Created by Colin LeMahieu on 9/24/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <lambda_p/binder/node_binder.h>

namespace llvm
{
    class Function;
}
namespace lambda_p_llvm
{
    class generation_context;
}
namespace lambda_p_repl
{
    class malloc_binder : public ::lambda_p::binder::node_binder
    {
    public:
        malloc_binder (::lambda_p_llvm::generation_context & context_a, ::llvm::Function * malloc_a);
        void bind (::lambda_p::core::statement * statement, ::std::map < ::lambda_p::core::node *, ::boost::shared_ptr < ::lambda_p::binder::node_instance> > & instances, ::std::vector < ::boost::shared_ptr < ::lambda_p::errors::error> > & problems);
		::std::wstring binder_name ();
    private:
        ::llvm::Function * malloc;
        ::lambda_p_llvm::generation_context & context;
    };
}