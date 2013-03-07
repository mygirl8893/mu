#include <mu/llvmc/analyzer.hpp>

#include <mu/core/error_string.hpp>
#include <mu/llvmc/ast.hpp>
#include <mu/llvmc/skeleton.hpp>

#include <gc_cpp.h>

mu::llvmc::module_result mu::llvmc::analyzer::analyze (mu::llvmc::ast::node * module_a)
{
    mu::llvmc::analyzer_module analyzer_l;
    auto result (analyzer_l.analyze (module_a));
    return result;
}

mu::llvmc::analyzer_module::analyzer_module () :
result_m ({nullptr, nullptr})
{
}

mu::llvmc::analyzer_function::analyzer_function (mu::llvmc::analyzer_module & module_a) :
result_m ({nullptr, nullptr}),
module (module_a)
{    
}

void mu::llvmc::analyzer_function::process_parameters (mu::llvmc::ast::function * function_a, mu::llvmc::skeleton::function * function_s)
{
    for (auto k (function_a->parameters.begin ()), l (function_a->parameters.end ()); k != l && result_m.error == nullptr; ++k)
    {
        auto type_l (process_type ((*k)->type));
        if (type_l != nullptr)
        {
            auto parameter_s (new (GC) mu::llvmc::skeleton::parameter (&function_s->entry, type_l));
            already_generated [*k] = parameter_s;
            function_s->parameters.push_back (parameter_s);
        }
        else
        {
            result_m.error = new (GC) mu::core::error_string (U"Expecting a type");
        }
    }
}

bool mu::llvmc::analyzer_function::process_node (mu::llvmc::ast::node * node_a)
{
    bool result (false);
    auto existing (already_generated.find (node_a));
    if (existing == already_generated.end ())
    {
        auto existing_multi (already_generated_multi.find (node_a));
        if (existing_multi == already_generated_multi.end ())
        {
            auto existing_function (module.functions.find (node_a));
            if (existing_function == module.functions.end ())
            {
                auto definite_expression (dynamic_cast <mu::llvmc::ast::definite_expression *> (node_a));
                if (definite_expression != nullptr)
                {
                    result = process_definite_expression (definite_expression);
                }
                else
                {
                    auto value_node (dynamic_cast <mu::llvmc::ast::value *> (node_a));
                    if (value_node != nullptr)
                    {
                        already_generated [node_a] = value_node->node_m;
                    }
                    else
                    {
                        result_m.error = new (GC) mu::core::error_string (U"Unknown expression subclass");
                    }
                }
            }
            else
            {
                already_generated [node_a] = existing_function->second;
            }
        }
        else
        {
            result = true;
        }
    }
    return result;
}

void mu::llvmc::analyzer_function::process_element (mu::llvmc::ast::element * element_a)
{
    auto multi (process_node (element_a->node));
    if (result_m.error == nullptr)
    {
        if (multi)
        {
            auto existing (already_generated_multi.find (element_a->node));
            if (existing->second.size () < element_a->index)
            {
                already_generated [element_a] = existing->second [element_a->index];
            }
            else
            {
                result_m.error = new (GC) mu::core::error_string (U"No value at index");
            }
        }
        else
        {
            if (element_a->index == 0)
            {
                already_generated [element_a] = already_generated [element_a->node];
            }
            else
            {
                result_m.error = new (GC) mu::core::error_string (U"Value has only one element");
            }
        }
    }
}

void mu::llvmc::analyzer_function::process_single_node (mu::llvmc::ast::node * node_a)
{
    auto multi (process_node (node_a));
    if (multi)
    {
        result_m.error = new (GC) mu::core::error_string (U"Expecting 1 value");
    }
}

mu::llvmc::skeleton::value * mu::llvmc::analyzer_function::process_value (mu::llvmc::ast::node * node_a)
{
    mu::llvmc::skeleton::value * result (nullptr);
    process_single_node (node_a);
    if (result_m.error == nullptr)
    {
        result = dynamic_cast <mu::llvmc::skeleton::value *> (already_generated [node_a]);
    }
    return result;
}

mu::llvmc::skeleton::type * mu::llvmc::analyzer_function::process_type (mu::llvmc::ast::node * node_a)
{
    mu::llvmc::skeleton::type * result (nullptr);
    process_single_node (node_a);
    if (result_m.error == nullptr)
    {
        auto node_l (already_generated [node_a]);
        result = dynamic_cast <mu::llvmc::skeleton::type *> (node_l);
    }
    return result;
}

void mu::llvmc::analyzer_function::process_results(mu::llvmc::ast::function * function_a, mu::llvmc::skeleton::function * function_s)
{
    for (auto k (function_a->results.begin ()), l (function_a->results.end ()); k != l && result_m.error == nullptr; ++k)
    {
        auto & ast_result (*k);
        function_s->results.push_back (decltype (function_s->results)::value_type ());
        auto & new_result (function_s->results [function_s->results.size () - 1]);
        for (auto m (ast_result.begin ()), n (ast_result.end ()); m != n && result_m.error == nullptr; ++m)
        {
            auto single_result (*m);
            auto type (process_type (single_result->written_type));
            if (type != nullptr)
            {
                auto value (process_value (single_result->value));
                if (value != nullptr)
                {
                    new_result.push_back (new (GC) mu::llvmc::skeleton::result (type, value));
                }
                else
                {
                    result_m.error = new (GC) mu::core::error_string (U"Expecting a value");
                }
            }
            else
            {
                result_m.error = new (GC) mu::core::error_string (U"Expecting a type");
            }
        }
    }
}

mu::llvmc::function_result mu::llvmc::analyzer_function::analyze (mu::llvmc::ast::node * function_a)
{
    auto function_l (dynamic_cast <mu::llvmc::ast::function *> (function_a));
    if (function_l != nullptr)
    {
        auto function_s (new (GC) mu::llvmc::skeleton::function);
        process_parameters (function_l, function_s);
        process_results (function_l, function_s);
        result_m.function = function_s;
    }
    else
    {
        result_m.error = new (GC) mu::core::error_string (U"Expecting a function");
    }
    return result_m;
}

mu::llvmc::module_result mu::llvmc::analyzer_module::analyze (mu::llvmc::ast::node * module_a)
{
    auto module_s (new (GC) mu::llvmc::skeleton::module);
    auto module_l (dynamic_cast <mu::llvmc::ast::module *> (module_a));
    if (module_l != nullptr)
    {
        for (auto i (module_l->functions.begin ()), j (module_l->functions.end ()); i != j && result_m.error == nullptr; ++i)
        {
            analyzer_function analyzer (*this);
            analyzer.analyze (*i);
            module_s->functions.push_back (analyzer.result_m.function);
            result_m.error = analyzer.result_m.error;
        }
    }
    else
    {
        result_m.error = new (GC) mu::core::error_string (U"Expecting a module");
    }
    if (result_m.error == nullptr)
    {
        result_m.module = module_s;
    }
    return result_m;
}

bool mu::llvmc::analyzer_function::process_definite_expression (mu::llvmc::ast::definite_expression * expression_a)
{
    bool result (false);
    auto existing (current_expression_generation.find (expression_a));
    if (existing == current_expression_generation.end ())
    {
        current_expression_generation.insert (expression_a);
        mu::vector <mu::llvmc::skeleton::node *> arguments;
        for (auto i (expression_a->arguments.begin ()), j (expression_a->arguments.end ()); i != j && result_m.error == nullptr; ++i)
        {
            result = (process_node (*i));
            if (result_m.error == nullptr)
            {
                if (result)
                {
                    auto & nodes (already_generated_multi [*i]);
                    for (auto k (nodes.begin ()), l (nodes.end ()); k != l && result_m.error == nullptr; ++k)
                    {
                        arguments.push_back (*k);
                    }
                }
                else
                {
                    arguments.push_back (already_generated [*i]);
                }
            }
        }
        if (result_m.error == nullptr)
        {
            if (!arguments.empty ())
            {
                auto target (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [0]));
                if (target != nullptr)
                {
                    result = process_value_call (arguments, expression_a);
                }
                else
                {
                    auto marker (dynamic_cast <mu::llvmc::skeleton::marker *> (arguments [0]));
                    if (marker != nullptr)
                    {
                        result = process_marker (arguments, expression_a);
                    }
                    else
                    {
                        result_m.error = new (GC) mu::core::error_string (U"Expecting first argument to be call target");
                    }
                }
            }
            else
            {
                result_m.error = new (GC) mu::core::error_string (U"Expecting a call target");
            }
        }
        current_expression_generation.erase (expression_a);
    }
    else
    {
        result_m.error = new (GC) mu::core::error_string (U"Cycle in expressions");
    }
    return result;
}

bool mu::llvmc::analyzer_function::process_value_call (mu::vector <mu::llvmc::skeleton::node *> & arguments_a, mu::llvmc::ast::node * expression_a)
{
    assert (! arguments_a.empty ());
    assert (dynamic_cast <mu::llvmc::skeleton::value *> (arguments_a [0]) != nullptr);
    auto target (static_cast <mu::llvmc::skeleton::value *> (arguments_a [0]));
    auto type_l (target->type ());
    auto pointer_type (dynamic_cast <mu::llvmc::skeleton::pointer_type *> (type_l));
    if (pointer_type != nullptr)
    {
        auto function_type (dynamic_cast <mu::llvmc::skeleton::function_type *> (pointer_type->pointed_type));
        if (function_type != nullptr)
        {
            if (arguments_a.size () == function_type->function.parameters.size ())
            {
                auto k (arguments_a.begin ());
                for (size_t i (0), j (function_type->function.parameters.size ()); i != j && result_m.error
                     == nullptr; ++i, ++k)
                {
                    auto argument_value (dynamic_cast <mu::llvmc::skeleton::value *> (*k));
                    if (argument_value != nullptr)
                    {
                        if ((*argument_value->type ()) != *function_type->function.parameters [i]->type ())
                        {
                            result_m.error = new (GC) mu::core::error_string (U"Argument type does not match parameter type");
                        }
                    }
                    else
                    {
                        result_m.error = new (GC) mu::core::error_string (U"Argument to function is not a value");
                    }
                }
                if (result_m.error == nullptr)
                {
                    mu::llvmc::skeleton::branch * most_specific_branch (nullptr);
                    if (!arguments_a.empty ())
                    {
                        auto value (dynamic_cast <mu::llvmc::skeleton::value *> (arguments_a [0]));
                        assert (value != nullptr);
                        most_specific_branch = value->branch;
                        for (auto i (arguments_a.begin () + 1), j (arguments_a.end ()); i != j && result_m.error == nullptr; ++i)
                        {
                            auto value (dynamic_cast <mu::llvmc::skeleton::value *> (*i));
                            assert (value != nullptr);
                            auto testing (value->branch);
                            while (testing != nullptr && testing != most_specific_branch)
                            {
                                testing = testing->parent;
                            }
                            if (testing == nullptr)
                            {
                                // Previous most specific branch was not above or equal to the current one
                                // Either current one must be most specific branch or these arguments are disjoint which is an error
                                testing = most_specific_branch;
                                most_specific_branch = value->branch;
                                while (testing != nullptr && testing != most_specific_branch)
                                {
                                    testing = testing->parent;
                                }
                                if (testing == nullptr)
                                {
                                    result_m.error = new (GC) mu::core::error_string (U"Arguments are disjoint");
                                }
                            }
                        }
                        if (result_m.error == nullptr)
                        {
                            auto call (new (GC) mu::llvmc::skeleton::function_call (function_type->function, most_specific_branch, arguments_a));
                            if (function_type->function.results.size () == 1)
                            {
                                if (function_type->function.results [0].size () == 1)
                                {
                                    already_generated [expression_a] = new (GC) mu::llvmc::skeleton::element (most_specific_branch, call, 0, 0);
                                }
                                else
                                {
                                    auto & target (already_generated_multi [expression_a]);
                                    for (size_t i (0), j (function_type->function.results [0].size ()); i != j && result_m.error == nullptr; ++i)
                                    {
                                        target.push_back (new (GC) mu::llvmc::skeleton::element (most_specific_branch, call, 0, i));
                                    }
                                }
                            }
                            else
                            {
                                for (size_t i (0), j (function_type->function.results.size ()); i != j && result_m.error == nullptr; ++i)
                                {
                                    auto branch (new (GC) mu::llvmc::skeleton::branch (most_specific_branch));
                                    if (function_type->function.results [0].size () == 1)
                                    {
                                        already_generated [expression_a] = new (GC) mu::llvmc::skeleton::element (branch, call, i, 0);
                                    }
                                    else
                                    {
                                        auto & target (already_generated_multi [expression_a]);
                                        for (size_t k (0), l (function_type->function.results [i].size ()); k != l && result_m.error == nullptr; ++k)
                                        {
                                            target.push_back (new (GC) mu::llvmc::skeleton::element (branch, call, i, k));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                result_m.error = new (GC) mu::core::error_string (U"Number of arguments does not match number of parameters");
            }
        }
        else
        {
            result_m.error = new (GC) mu::core::error_string (U"Pointer does not point to a function");
        }
    }
    else
    {
        result_m.error = new (GC) mu::core::error_string (U"Only function pointers can be the target of a call");
    }
    return result;
}

bool mu::llvmc::analyzer_function::process_marker (mu::vector <mu::llvmc::skeleton::node *> & arguments_a, mu::llvmc::ast::node * expression_a)
{
    assert (!arguments_a.empty ());
    assert (dynamic_cast <mu::llvmc::skeleton::marker *> (arguments_a [0]) != nullptr);
    auto marker (static_cast <mu::llvmc::skeleton::marker *> (arguments_a [0]));
    bool result (false);
    switch (marker->type)
    {
        default:
            result_m.error = new (GC) mu::core::error_string (U"Unknown instruction marker");
            break;
    }
    return result;
}