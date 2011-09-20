#define __STDC_LIMIT_MACROS

#include "data_to_string_binder.h"

#include <lambda_p/core/statement.h>
#include <lambda_p/core/node.h>
#include <lambda_p/core/data.h>
#include <lambda_p_llvm/llvm_generation_context.h>
#include <lambda_p_llvm/llvm_value.h>

#include <llvm/DerivedTypes.h>
#include <llvm/Constants.h>
#include <llvm/GlobalValue.h>
#include <llvm/GlobalVariable.h>
#include <llvm/Module.h>

#include <sstream>

lambda_p_llvm::data_to_string_binder::data_to_string_binder (::lambda_p_llvm::llvm_generation_context context_a)
: context (context_a)
{
}

lambda_p_llvm::data_to_string_binder::~data_to_string_binder (void)
{
}

void lambda_p_llvm::data_to_string_binder::bind (::lambda_p::core::statement * statement, ::std::map < ::lambda_p::core::node *, ::boost::shared_ptr < ::lambda_p::binder::node_instance> > & instances, ::std::wstringstream & problems)
{
	size_t argument_count (statement->arguments.size ());
	if (argument_count == 3)
	{
		::lambda_p::core::node_id arg1_type (statement->arguments [1]->node_type ());
		switch (arg1_type)
		{
		case ::lambda_p::core::node_declaration:
			{
				::lambda_p::core::node_id arg2_type (statement->arguments [2]->node_type ());
				switch (arg2_type)
				{
				case ::lambda_p::core::node_data:
					{
						::lambda_p::core::data * data (static_cast < ::lambda_p::core::data *> (statement->arguments [2]));
                        ::std::wstring string (data->string ());
                        ::llvm::ArrayType * string_type (::llvm::ArrayType::get (context.wchar_t_type, string.size () + 1));
                        ::std::vector < ::llvm::Constant *> string_initializer;
                        for (::std::wstring::iterator i = string.begin (); i != string.end (); ++i)
                        {
                            string_initializer.push_back (::llvm::ConstantInt::get (context.wchar_t_type, *i));
                        }
                        string_initializer.push_back (::llvm::ConstantInt::get (context.wchar_t_type, 0));
                        ::llvm::Constant * string_array (::llvm::ConstantArray::get (string_type, string_initializer));
                        ::llvm::GlobalVariable * string_global (new ::llvm::GlobalVariable (string_type, true, ::llvm::GlobalValue::ExternalLinkage, string_array));
                        context.module->getGlobalList ().push_back (string_global);
                        ::boost::shared_ptr < ::lambda_p_llvm::llvm_value> value (new ::lambda_p_llvm::llvm_value (string_global));
						instances [statement->arguments [1]] = value;
					}
					break;
				default:
					problems << L"data_to_string_binder expecting argument 2 to be data, have: ";
					problems << statement->arguments [2]->node_type_name ();
					problems << '\n';
					break;
				}
			}
			break;
		default:
			problems << L"data_to_string_binder expecting argument 1 to be a declaration, have: ";
			problems << statement->arguments [1]->node_type_name ();
			problems << '\n';
			break;
		}
	}
	else
	{
		problems << L"data_to_string_binder expecting 2 arguments, have: ";
		problems << argument_count - 1;
		problems << '\n';
	}
}

void lambda_p_llvm::data_to_string_binder::operator () ()
{
}