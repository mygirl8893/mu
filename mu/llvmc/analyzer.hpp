#pragma once

#include <mu/core/types.hpp>

namespace mu
{
    namespace core
    {
        class error;
    }
    namespace llvmc
    {
        namespace skeleton
        {
            class value;
            class node;
            class module;
            class expression;
            class definite_expression;
            class function;
            class type;
        }
        namespace ast
        {
            class node;
            class expression;
            class definite_expression;
            class function;
            class instruction;
        }
        class module_result
        {
        public:
            mu::llvmc::skeleton::module * module;
            mu::core::error * error;
        };
        class analyzer_module
        {
        public:
            analyzer_module ();
            module_result analyze (mu::llvmc::ast::node * module_a);
            mu::map <mu::llvmc::ast::node *, mu::llvmc::skeleton::function *> functions;
            mu::llvmc::module_result result_m;
        };
        class function_result
        {
        public:
            mu::llvmc::skeleton::function * function;
            mu::core::error * error;
        };
        class analyzer_function
        {
        public:
            analyzer_function (mu::llvmc::analyzer_module & module_a);
            void process_parameters (mu::llvmc::ast::function * function_a, mu::llvmc::skeleton::function * function_s);
            void process_results (mu::llvmc::ast::function * function_a, mu::llvmc::skeleton::function * function_s);
            bool process_node (mu::llvmc::ast::node * node_a);
            void process_instruction (mu::llvmc::ast::instruction * node_a);
            mu::llvmc::skeleton::value * process_value (mu::llvmc::ast::node * node_a);
            mu::llvmc::skeleton::type * process_type (mu::llvmc::ast::node * node_a);
            bool process_definite_expression (mu::llvmc::ast::definite_expression * node_a);
            void process_single_node (mu::llvmc::ast::node * node_a);
            mu::map <mu::llvmc::ast::node *, mu::llvmc::skeleton::node *> already_generated;
            mu::map <mu::llvmc::ast::node *, mu::vector <mu::llvmc::skeleton::node *>> already_generated_multi;
            mu::set <mu::llvmc::ast::expression *> current_expression_generation;
            function_result analyze (mu::llvmc::ast::node * function_a);
            mu::llvmc::function_result result_m;
            mu::llvmc::analyzer_module & module;
        };
        class analyzer
        {
        public:
            module_result analyze (mu::llvmc::ast::node * module_a);
        };
    }
}