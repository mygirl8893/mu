#include <mu/llvm_/instructions.xor_c.h>

#include <mu/core/errors.error_target.h>
#include <mu/llvm_/instruction.node.h>

#include <llvm/Value.h>
#include <llvm/DerivedTypes.h>
#include <llvm/BasicBlock.h>
#include <llvm/Instructions.h>

#include <sstream>

#include <boost/make_shared.hpp>

void mu::llvm_::instructions::xor_c::operator () (boost::shared_ptr <mu::core::errors::error_target> errors_a, mu::core::segment <boost::shared_ptr <mu::core::node>> parameters_a, std::vector <boost::shared_ptr <mu::core::node>> & results_a)
{
	auto one (boost::dynamic_pointer_cast <mu::llvm_::value::node> (parameters_a [0]));
	auto two (boost::dynamic_pointer_cast <mu::llvm_::value::node> (parameters_a [1]));
	if (one.get () != nullptr)
	{
		if (two.get () != nullptr)
		{
			bool one_int (one->value ()->getType ()->isIntegerTy ());
			bool two_int (two->value ()->getType ()->isIntegerTy ());
			if (one_int && two_int)
			{
				size_t one_bits (one->value ()->getType ()->getPrimitiveSizeInBits ());
				size_t two_bits (two->value ()->getType ()->getPrimitiveSizeInBits ());
				if (one_bits == two_bits)
				{
					auto instruction (llvm::BinaryOperator::CreateXor (one->value (), two->value ()));
					results_a.push_back (boost::make_shared <mu::llvm_::instruction::node> (instruction, one->type));
				}
				else
				{
					std::wstringstream message;
					message << L"Bit widths don't match: ";
					message << one_bits;
					message << L" ";
					message << two_bits;
					(*errors_a) (message.str ());
				}
			}
			else
			{
				std::wstringstream message;
				message << L"Arguments are not integers: ";
				message << one_int;
				message << L" ";
				message << two_int;
				(*errors_a) (message.str ());
			}
		}
		else
		{
			invalid_type (errors_a, parameters_a [1], 1);
		}
	}
	else
	{
		invalid_type (errors_a, parameters_a [0], 0);
	}
}

size_t mu::llvm_::instructions::xor_c::count ()
{
	return 2;
}
