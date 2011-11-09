#include "eq_binder.h"

#include <lambda_p/binder/binder_implementor.h>
#include <lambda_p_llvm/module.h>
#include <lambda_p_llvm/type.h>
#include <lambda_p_llvm/value.h>

#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Module.h>
#include <llvm/Instructions.h>

void lambda_p_llvm::eq_binder::bind (lambda_p::core::statement * statement, lambda_p::binder::list & nodes, lambda_p::errors::error_list & problems)
{
	check_count (1, 2, statement, problems);
	if (problems.errors.empty ())
	{
		auto module (boost::dynamic_pointer_cast <lambda_p_llvm::module> (nodes [statement->association->references [0]]));
		check_binder (module, 0, L"module", problems);
		auto type (boost::dynamic_pointer_cast <lambda_p_llvm::type> (nodes [statement->association->references [1]]));
		check_binder (type, 1, L"type", problems);
		if (problems.errors.empty ())
		{
			auto int_type (llvm::dyn_cast <llvm::IntegerType> (type->type_m));
			if (int_type != nullptr)
			{
				std::vector <llvm::Type const *> arguments;
				arguments.push_back (type->type_m);
				arguments.push_back (type->type_m);
				auto function (llvm::Function::Create (llvm::FunctionType::get (llvm::Type::getInt1Ty (module->module_m->getContext ()), arguments, false), llvm::GlobalValue::ExternalLinkage));
				module->module_m->getFunctionList ().push_back (function);
				auto block (llvm::BasicBlock::Create (module->module_m->getContext ()));
				function->getBasicBlockList ().push_back (block);
				auto arg (function->arg_begin ());
				auto arg1 (arg);
				++arg;
				auto arg2 (arg);
				auto op (new llvm::ICmpInst (llvm::CmpInst::Predicate::ICMP_EQ, arg1, arg2));
				block->getInstList ().push_back (op);
				block->getInstList ().push_back (llvm::ReturnInst::Create (module->module_m->getContext (), op));
				boost::shared_ptr <lambda_p_llvm::value> value (new lambda_p_llvm::value (function));
				nodes [statement->association->declarations [0]] = value;
			}
			else
			{
				add_error (L"Type must be an IntegerType", problems);
			}
		}
	}
}

std::wstring lambda_p_llvm::eq_binder::binder_name ()
{
	return std::wstring (L"eq_binder");
}