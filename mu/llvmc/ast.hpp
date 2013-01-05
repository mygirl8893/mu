#pragma once

#include <mu/core/types.hpp>
#include <mu/llvmc/node_result.hpp>

namespace mu
{
    namespace io
    {
        template <typename T>
        class stream;
        class token;
    }
    namespace llvmc
    {
        namespace availability
        {
            class node;
            class function;
            class module;
        }
        namespace ast
        {
            class node
            {
            public:
                virtual ~node ();
            };
            class scoped : public mu::llvmc::ast::node
            {
            public:
                scoped (mu::llvmc::availability::node * availability_a);
                mu::llvmc::availability::node * availability_m;
            };
            class argument : public mu::llvmc::ast::scoped
            {
            public:
                argument (mu::llvmc::availability::node * availability_a);
            };
            class expression : public mu::llvmc::ast::scoped
            {
            public:
                expression (mu::llvmc::availability::node * availability_a);
                mu::vector <mu::llvmc::ast::node *> arguments;
            };
            class value : public mu::llvmc::ast::scoped
            {
            public:
                value (mu::llvmc::availability::node * availability_a);
            };
            class function : public mu::llvmc::ast::scoped
            {
            public:
                function (mu::llvmc::availability::module * availability_a);
                mu::llvmc::availability::function * entry;
                mu::string name;
                mu::vector <mu::llvmc::ast::node *> parameters;
                mu::vector <mu::vector <mu::llvmc::ast::node *>> results;
            };
            class module : public mu::llvmc::ast::scoped
            {
            public:
                module ();
                mu::llvmc::availability::module * availability ();
                mu::vector <mu::llvmc::ast::function *> functions;
            };
        }
    }
}