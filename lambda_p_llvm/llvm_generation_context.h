//
//  llvm_generation_context.h
//  lambda_p_llvm
//
//  Created by Colin LeMahieu on 9/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <llvm/LLVMContext.h>

namespace llvm
{
    class BasicBlock;
    class Module;
    class Type;
}
namespace lambda_p_llvm
{
    class llvm_generation_context
    {
    public:
        llvm_generation_context (::llvm::LLVMContext & context_a);
        ::llvm::LLVMContext & context;
        ::llvm::Type const * wchar_t_type;
        ::llvm::Module * module;
        ::llvm::BasicBlock * block;
    };
}