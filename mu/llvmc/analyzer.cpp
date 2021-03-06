#include <mu/llvmc/analyzer.hpp>

#include <mu/core/error_string.hpp>
#include <mu/llvmc/ast.hpp>
#include <mu/llvmc/skeleton.hpp>
#include <mu/llvmc/ast_t.hpp>
#include <mu/llvmc/skeleton_t.hpp>

#include <typeinfo>

#include <inttypes.h>

static mu::llvmc::skeleton::factory b;

mu::llvmc::branch_analyzer::branch_analyzer (mu::llvmc::skeleton::branch * start_a, mu::core::error * & result_a) :
start (start_a),
most_specific (start_a),
result (result_a)
{
}

mu::llvmc::skeleton::branch * mu::llvmc::branch_analyzer::add_branch (mu::llvmc::skeleton::branch * branch_a, mu::core::region const & region_a)
{
	auto most_specific_l (most_specific->most_specific (branch_a));
	if (most_specific_l != nullptr)
	{
		most_specific = most_specific_l;
	}
	else
	{
		result = new mu::core::error_string (U"Branches are disjoint", mu::core::error_type::branch_analyzer_disjoint, region_a);
	}
    return most_specific_l;
}

void mu::llvmc::branch_analyzer::new_set ()
{
	auto existing_leaf (leaves.find (most_specific));
	if (existing_leaf == leaves.end ())
	{
		auto existing_ancestor (ancestors.find (most_specific));
		if (existing_ancestor == ancestors.end ())
		{
			mu::set <mu::llvmc::skeleton::branch *> ancestors_l;
			auto branch_l (most_specific->parent);
			while (branch_l != nullptr)
			{
				ancestors_l.insert (branch_l);
				branch_l = branch_l->parent;
			}
			mu::vector <mu::llvmc::skeleton::branch *> same (std::max (ancestors_l.size (), leaves.size ()));
			auto final (std::set_intersection (ancestors_l.begin (), ancestors_l.end (), leaves.begin (), leaves.end (), same.begin ()));
			if (final == same.begin ())
			{
				ancestors.insert (ancestors_l.begin (), ancestors_l.end ());
				leaves.insert (most_specific);
				most_specific = start;
			}
			else
			{
				result = new mu::core::error_string (U"Result branch is not unique", mu::core::error_type::branch_analyzer_intersection_exists);
			}
		}
		else
		{
			result = new mu::core::error_string (U"Result branch is not unique", mu::core::error_type::branch_analyzer_ancestor_exists);
		}
	}
	else
	{
		result = new mu::core::error_string (U"Result branch is not unique", mu::core::error_type::branch_analyzer_leaves_exist);
	}
}

class global_naming : public mu::llvmc::skeleton::visitor
{
public:
	global_naming (mu::llvmc::module_processor & module_a, mu::core::error * & error_a, mu::string const & name_a) :
	module (module_a),
	error (error_a),
	name (name_a)
	{
	}
	void node (mu::llvmc::skeleton::node * node_a) override
	{
		// Non-globals can be named anything
	}
	void global_value (mu::llvmc::skeleton::global_value * node_a) override
	{
		auto existing (module.named_globals.find (node_a));
		if (existing == module.named_globals.end ())
		{
			auto removed (module.unnamed_globals.erase (node_a));
			assert (removed == 1);
			module.named_globals.insert (node_a);
			module.module_m->globals.push_back (node_a);
			assert (node_a->name.empty ());
			node_a->name = name;
		}
		else
		{
			// Globals stick with their innermost name which is only used while debugging
		}
	}
    mu::llvmc::module_processor & module;
	mu::core::error * & error;
	mu::string const & name;
};

class module_analyzer : public mu::llvmc::skeleton::visitor
{
public:
    module_analyzer (mu::llvmc::module_processor & module_a, mu::core::error * & error_a) :
    module_m (module_a),
	error (error_a)
    {
    }
	void node (mu::llvmc::skeleton::node * node_a) override
	{
		error = new mu::core::error_string (U"Unexpected node type in module", mu::core::error_type::unexpected_node_type);
	}
    void named (mu::llvmc::skeleton::named * node_a) override
    {
        node_a->value_m->visit (this);
		if (error == nullptr)
		{
			global_naming naming (module_m, error, node_a->name);
			node_a->value_m->visit (&naming);
		}
    }
	void number (mu::llvmc::skeleton::number * node_a) override
	{
		written_but_not_generated (node_a);
	}
	void global_value (mu::llvmc::skeleton::global_value * node_a) override
	{
		written_but_not_generated (node_a);
	}
    void constant (mu::llvmc::skeleton::constant * node_a) override
    {
		written_but_not_generated (node_a);
    }
    void type (mu::llvmc::skeleton::type * node_a) override
    {
		written_but_not_generated (node_a);
    }
    void template_c (mu::llvmc::skeleton::template_c * node_a) override
    {
		written_but_not_generated (node_a);
    }
	void module (mu::llvmc::skeleton::module * node_a) override
	{
		written_but_not_generated (node_a);
	}
	void written_but_not_generated (mu::llvmc::skeleton::node * node_a)
	{
        // Nodes that can be written in module context but are not by themselves generated as part of the module
	}
    mu::llvmc::module_processor & module_m;
	mu::core::error * & error;
};
				
mu::llvmc::global_processor::global_processor (mu::core::error * & error_a) :
current_context (this),
error (error_a),
current_origin (&mu::llvmc::skeleton::branch::global)
{
}
				
void mu::llvmc::global_processor::node (mu::llvmc::ast::node * node_a)
{
	error = new mu::core::error_string (U"Expecting a module", mu::core::error_type::expecting_a_module, node_a->region);
}

mu::llvmc::module_processor::module_processor (mu::llvmc::global_processor & global_a) :
global_m (global_a),
module_m (b.module ()),
previous (global_a.current_context)
{
	global_a.current_context = this;
}

mu::llvmc::module_processor::~module_processor ()
{
	assert (global_m.current_context == this);
	global_m.current_context = previous;
}

void mu::llvmc::global_processor::module (mu::llvmc::ast::module * node_a)
{
	mu::llvmc::module_processor processor (*this);
	auto module_s (processor.module_m);
	module_analyzer analyzer (processor, error);
	for (auto i (node_a->globals.begin ()), j (node_a->globals.end ()); i != j && error == nullptr; ++i)
	{
		if (!(*i)->assigned)
		{
			process_node (*i);
		}
		if (error == nullptr)
		{
			assert ((*i)->assigned);
			auto & values ((*i)->generated);
			for (auto k (values.begin ()), l (values.end ()); k != l && error == nullptr; ++k)
			{
				auto value (*k);
				value->visit (&analyzer);
			}
		}
	}
    if (error == nullptr)
    {
        for (auto i: node_a->names)
        {
            assert (i.second->assigned);
            assert (module_s->names.find (i.first) == module_s->names.end ());
            assert (i.second->generated.size () == 1);
            module_s->names [i.first] = i.second->generated [0];
        }
		node_a->assigned = true;
		node_a->generated.push_back (module_s);
    }
}

class module_checker : public mu::llvmc::skeleton::visitor
{
public:
	module_checker (mu::core::error * & error_a, mu::core::region & region_a) :
	error (error_a),
	region (region_a)
	{
	}
	void node (mu::llvmc::skeleton::node * node_a)
	{
		error = new mu::core::error_string (U"Expecting a module", mu::core::error_type::expecting_a_module, region);
	}
	void module (mu::llvmc::skeleton::module * node_a)
	{
		//Expecting a module;
	}
	mu::core::error * & error;
	mu::core::region & region;
};

mu::llvmc::module_result mu::llvmc::analyzer::analyze (mu::llvmc::ast::node * module_a)
{
	mu::llvmc::module_result result ({nullptr, nullptr});
	mu::llvmc::global_processor analyzer_l (result.error);
	module_a->visit (&analyzer_l);
	if (result.error == nullptr)
	{
		module_checker checker (result.error, module_a->region);
		assert (module_a->generated.size () == 1);
		if (result.error == nullptr)
		{
			result.module = mu::cast <mu::llvmc::skeleton::module> (module_a->generated [0]);
		}
	}
	return result;
}

void mu::llvmc::function_processor::process_parameters ()
{
	for (auto k (node_m->parameters.begin ()), l (node_m->parameters.end ()); k != l && module_m.global_m.error == nullptr; ++k)
	{
		auto parameter (mu::cast <mu::llvmc::ast::parameter> (*k));
        auto node (parameter->type);
		auto type_l (module_m.process_type (node));
		if (module_m.global_m.error == nullptr)
		{
			if (type_l != nullptr)
			{
				auto parameter_s (b.parameter (parameter->region, function_m->entry, type_l, parameter->name));
				parameter->generated.push_back (parameter_s);
				parameter->assigned = true;
				function_m->parameters.push_back (parameter_s);
			}
			else
			{
				module_m.global_m.error = new mu::core::error_string (U"Expecting a type", mu::core::error_type::expecting_type_in_parameters, node->region);
			}
		}
	}
}

mu::llvmc::skeleton::number * mu::llvmc::global_processor::process_number (mu::llvmc::ast::node * node_a)
{
	mu::llvmc::skeleton::number * result (nullptr);
	process_node (node_a);
	if (error == nullptr)
	{
		assert (node_a->assigned);
		if (node_a->generated.size () == 1)
		{
			result = dynamic_cast <mu::llvmc::skeleton::number *> (node_a->generated [0]);
			if (result == nullptr)
			{
				error = new mu::core::error_string (U"Expecting number", mu::core::error_type::expecting_a_number);
			}
		}
		else
		{
			error = new mu::core::error_string (U"Expecting one node", mu::core::error_type::expecting_one_value);
		}
	}
	return result;
}

void mu::llvmc::function_processor::node (mu::llvmc::ast::node * node_a)
{
	node_a->visit (previous);
}

void mu::llvmc::module_processor::value (mu::llvmc::ast::value * value_node)
{
	value_node->generated.push_back (value_node->node_m);
	value_node->assigned = true;
}

void mu::llvmc::module_processor::integer_type (mu::llvmc::ast::integer_type * type_a)
{
	auto number (global_m.process_number (type_a->bits));
	if (global_m.error == nullptr)
	{
		assert (number != nullptr);
		if (number->value <= 1024)
		{
			type_a->generated.push_back (b.integer_type (number->value));
			type_a->assigned = true;
		}
		else
		{
			global_m.error = new mu::core::error_string (U"Bit width too wide", mu::core::error_type::bit_width_too_wide, type_a->region);
		}
	}
}

class naming_visitor : public mu::llvmc::skeleton::visitor
{
public:
    naming_visitor (mu::llvmc::module_processor & module_a, mu::llvmc::ast::element * element_a, mu::core::error * & error_a) :
	module (module_a),
    element (element_a),
    error (error_a)
    {
    }
    void node (mu::llvmc::skeleton::node * node_a)
    {
        element->generated.push_back (node_a);
    }
    void value (mu::llvmc::skeleton::value * node_a)
    {
        element->generated.push_back (b.named (element->region, node_a, element->name));
    }
	void global_value (mu::llvmc::skeleton::global_value * node_a)
	{
		global_naming naming (module, error, element->name);
		node_a->visit (&naming);
		element->generated.push_back (node_a);
	}
	mu::llvmc::module_processor & module;
    mu::llvmc::ast::element * element;
    mu::core::error * & error;
};

void mu::llvmc::module_processor::element (mu::llvmc::ast::element * element_a)
{
	global_m.process_node (element_a->node_m);
	if (global_m.error == nullptr)
	{
		assert (element_a->node_m->assigned);
		auto & existing (element_a->node_m->generated);
		if (existing.size () > element_a->index)
		{
			auto node (existing [element_a->index]);
            naming_visitor naming (*this, element_a, global_m.error);
            node->visit (&naming);
			if (global_m.error == nullptr)
			{
				element_a->assigned = true;
			}
		}
		else
		{
			global_m.error = new mu::core::error_string (U"No value at index", mu::core::error_type::no_value_at_index, element_a->region);
		}
	}
}

void mu::llvmc::function_processor::unit (mu::llvmc::ast::unit * node_a)
{
	node_a->generated.push_back (&module_m.module_m->the_unit_value);
	node_a->assigned = true;
}

void mu::llvmc::module_processor::constant_int (mu::llvmc::ast::constant_int * constant_a)
{
	constant_a->assigned = true;
	constant_a->generated.push_back (b.constant_int_c ());
}

void mu::llvmc::module_processor::process_constant_int (mu::llvmc::ast::expression * expression_a)
{
	mu::vector <mu::llvmc::skeleton::node *> arguments;
    mu::vector <mu::llvmc::skeleton::value *> sequenced;
	auto most_specific_branch (&mu::llvmc::skeleton::branch::global);
	process_expression_value_arguments (expression_a->arguments, arguments, sequenced, most_specific_branch);
	if (global_m.error == nullptr)
	{
		if (arguments.size () == 3)
		{
			auto type_l (dynamic_cast <mu::llvmc::skeleton::integer_type *> (arguments [1]));
			if (type_l != nullptr)
			{
				auto number_l (dynamic_cast <mu::llvmc::skeleton::number *> (arguments [2]));
				if (number_l != nullptr)
				{
					expression_a->generated.push_back (b.constant_integer (expression_a->region, type_l, number_l->value));
					expression_a->assigned = true;
				}
				else
				{
					global_m.error = new mu::core::error_string (U"Constant function requires second argument to be a number", mu::core::error_type::expecting_a_number, expression_a->region);
				}
			}
			else
			{
				global_m.error = new mu::core::error_string (U"Constant function requires first argument to be an integer type", mu::core::error_type::expecting_an_integer_type, expression_a->region);
			}
		}
		else
		{
			global_m.error = new mu::core::error_string (U"Constant function requires two arguments", mu::core::error_type::incorrect_number_of_arguments, expression_a->region);
		}
	}
}

void mu::llvmc::module_processor::pointer_type (mu::llvmc::ast::pointer_type * type_a)
{
	auto pointed_type (process_type (type_a->pointed_type));
	if (pointed_type != nullptr)
	{
		type_a->generated.push_back (b.pointer_type (pointed_type));
		type_a->assigned = true;
	}
}

void mu::llvmc::function_processor::asm_c (mu::llvmc::ast::asm_c * asm_l)
{
	auto type (module_m.process_type (asm_l->type));
	if (type != nullptr)
	{
		asm_l->generated.push_back (b.asm_c (type, asm_l->text, asm_l->constraints));
		asm_l->assigned = true;
	}
	else
	{
		module_m.global_m.error = new mu::core::error_string (U"Expecting a type", mu::core::error_type::expecting_a_type, asm_l->type->region);
	}
}

void mu::llvmc::module_processor::fixed_array_type (mu::llvmc::ast::fixed_array_type * type_a)
{
	auto element (process_type (type_a->element_type));
	if (element != nullptr)
	{
		global_m.process_node (type_a->size);
		if (global_m.error == nullptr)
		{
			assert (type_a->size->assigned);
			if (type_a->size->generated.size () == 1)
			{
				auto number_l (dynamic_cast <mu::llvmc::skeleton::number *> (type_a->size->generated [0]));
				if (number_l != nullptr)
				{
					type_a->generated.push_back (b.fixed_array_type (element, number_l->value));
					type_a->assigned = true;
				}
				else
				{
					global_m.error = new mu::core::error_string (U"Expecting number", mu::core::error_type::expecting_a_number, type_a->size->region);
				}
			}
			else
			{
				global_m.error = new mu::core::error_string (U"Array size expects one value", mu::core::error_type::expecting_one_value, type_a->size->region);
			}
		}
	}
}

void mu::llvmc::module_processor::constant_array (mu::llvmc::ast::constant_array * array_a)
{
	auto type (process_type (array_a->type));
	if (type != nullptr)
	{
		mu::vector <mu::llvmc::skeleton::constant *> initializer;
		for (auto i (array_a->initializer.begin ()), j (array_a->initializer.end ()); i != j && global_m.error == nullptr; ++i)
		{
			global_m.process_node (*i);
			auto & values ((*i)->generated);
			for (auto k (values.begin ()), l (values.end ()); k != l && global_m.error == nullptr; ++k)
			{
				auto constant (dynamic_cast <mu::llvmc::skeleton::constant *> (*k));
				if (constant != nullptr)
				{
					if (*type == *constant->type ())
					{
						initializer.push_back (constant);
					}
					else
					{
						global_m.error = new mu::core::error_string (U"Initializer type doesn't match array element type", mu::core::error_type::initializer_type_doesnt_match, (*i)->region);
					}
				}
				else
				{
					global_m.error = new mu::core::error_string (U"Initializer must be a constant", mu::core::error_type::initializer_must_be_constant, (*i)->region);
				}
			}
		}
		if (global_m.error == nullptr)
		{
			auto array_type (b.fixed_array_type (type, initializer.size ()));
			array_a->generated.push_back (b.constant_array (array_a->region, array_type, initializer));
			array_a->assigned = true;
		}
	}
}

void mu::llvmc::module_processor::global_variable (mu::llvmc::ast::global_variable * global_variable)
{
	global_m.process_node (global_variable->initializer);
	if (global_m.error == nullptr)
	{
		assert (global_variable->initializer->assigned);
		auto & values (global_variable->initializer->generated);
		if (values.size () == 1)
		{
			auto constant (dynamic_cast <mu::llvmc::skeleton::constant *> (values [0]));
			if (constant != nullptr)
			{
				global_variable->assigned = true;
				auto & targets (global_variable->generated);
				assert (targets.empty ());
				auto skeleton (b.global_variable (global_variable->region, constant));
				assert (unnamed_globals.find (skeleton) == unnamed_globals.end ());
				unnamed_globals.insert (skeleton);
				targets.push_back (skeleton);
			}
			else
			{
				global_m.error = new mu::core::error_string (U"Global variables must have constant initializers", mu::core::error_type::global_constant_initializer, global_variable->initializer->region);
			}
		}
		else
		{
			global_m.error = new mu::core::error_string (U"Global variables expect one initializer", mu::core::error_type::global_one_initializer, mu::core::region (global_variable->region.first, global_variable->initializer->region.last));
		}
	}
}

void mu::llvmc::module_processor::constant_pointer_null (mu::llvmc::ast::constant_pointer_null * constant_pointer_null)
{
	auto type (process_type (constant_pointer_null->type));
	if (type != nullptr)
	{
		auto pointer (dynamic_cast <mu::llvmc::skeleton::pointer_type *> (type));
		if (pointer != nullptr)
		{
			constant_pointer_null->assigned = true;
			auto & values (constant_pointer_null->generated);
			auto skeleton (b.constant_pointer_null (constant_pointer_null->region, type));
			values.push_back (skeleton);
		}
		else
		{
			global_m.error = new mu::core::error_string (U"Type is not a pointer", mu::core::error_type::expecting_a_pointer_type, constant_pointer_null->type->region);
		}
	}
}

void mu::llvmc::module_processor::unit_type (mu::llvmc::ast::unit_type * unit_type)
{
	auto & values (unit_type->generated);
	unit_type->assigned = true;
	values.push_back (&module_m->the_unit_type);
}

void mu::llvmc::function_processor::join (mu::llvmc::ast::join * node_a)
{
	mu::llvmc::branch_analyzer analyzer_l (module_m.global_m.current_origin, module_m.global_m.error);
	auto join (new mu::llvmc::skeleton::join_value);
	for (auto & i: node_a->branches)
	{
		auto & target (join->add_branch ());
		for (auto j: i.arguments)
		{
			if (module_m.global_m.error == nullptr)
			{
				module_m.global_m.process_node (j);
				auto & values (j->generated);
				for (auto k: values)
				{
					auto value (dynamic_cast <mu::llvmc::skeleton::value *> (k));
					if (value != nullptr)
					{
						target.arguments.push_back (value);
						analyzer_l.add_branch (value->branch, value->region);
					}
					else
					{
						module_m.global_m.error = new mu::core::error_string (U"Join arguments must be values", mu::core::error_type::join_arguments_must_be_values, j->region);
					}
				}
			}
		}
		for (auto j: i.predicates)
		{
			if (module_m.global_m.error == nullptr)
			{
				module_m.global_m.process_node (j);
				auto & values (j->generated);
				for (auto k: values)
				{
					auto value (dynamic_cast <mu::llvmc::skeleton::value *> (k));
					if (value != nullptr)
					{
						target.predicates.push_back (value);
						analyzer_l.add_branch (value->branch, value->region);
					}
					else
					{
						module_m.global_m.error = new mu::core::error_string (U"Join arguments must be values", mu::core::error_type::join_arguments_must_be_values, j->region);
					}
				}
			}
		}
		analyzer_l.new_set ();
	}
	if (module_m.global_m.error == nullptr)
	{
		if (join->branches.size () > 1)
		{
			auto size (join->branches [0].arguments.size ());
			if (size > 0)
			{
				mu::vector <mu::llvmc::skeleton::type *> types;
				for (auto i: join->branches [0].arguments)
				{
					types.push_back (i->type ());
				}
				for (auto & i: join->branches)
				{
					if (i.arguments.size () == size)
					{
						size_t index (0);
						for (auto j: i.arguments)
						{
							if (*j->type() != *types [index])
							{
								module_m.global_m.error = new mu::core::error_string (U"Join argument types must match for each branch", mu::core::error_type::joining_types_are_different, node_a->region);
							}
							++index;
						}
					}
					else
					{
						module_m.global_m.error = new mu::core::error_string (U"Join branches must have same cardinality", mu::core::error_type::join_branches_same_cardinality, node_a->region);
					}
				}
				if (module_m.global_m.error == nullptr)
				{
					assert (!analyzer_l.leaves.empty ());
					auto least_specific_branch (*analyzer_l.leaves.begin ());
					for (auto i: analyzer_l.leaves)
					{
						least_specific_branch = least_specific_branch->least_specific (i);
					}
					assert (least_specific_branch != &mu::llvmc::skeleton::branch::global);
					auto & elements (join->elements);
					auto & generated (node_a->generated);
					node_a->assigned = true;
					for (auto i: types)
					{
						auto element (b.join_element (node_a->region, least_specific_branch->parent, join, i));
						generated.push_back (element);
						elements.push_back (element);
					}
				}
			}
			else
			{
				module_m.global_m.error = new mu::core::error_string (U"Joining branches must contain at least one value", mu::core::error_type::must_be_joining_at_least_two_values, node_a->region);
			}
		}
		else
		{
			module_m.global_m.error = new mu::core::error_string (U"Must be joining at least two branches", mu::core::error_type::must_be_joining_at_least_two_branches, node_a->region);
		}
	}
}

void mu::llvmc::module_processor::node (mu::llvmc::ast::node * node_a)
{
	node_a->visit (previous);
}

void mu::llvmc::module_processor::function (mu::llvmc::ast::function * function_node)
{
	mu::llvmc::function_processor nodes (*this, function_node);
	nodes.process ();
}

void mu::llvmc::function_processor::process ()
{
    process_parameters ();
	process_returns ();
	if (module_m.global_m.error == nullptr)
	{
		node_m->generated.push_back (function_m);
		node_m->assigned = true;
		process_results ();
		if (module_m.global_m.error == nullptr)
		{
			assert (module_m.unnamed_globals.find (function_m) == module_m.unnamed_globals.end ());
			module_m.unnamed_globals.insert (function_m);
		}
		else
		{
			node_m->generated.clear ();
			node_m->assigned = false;
		}
	}
}

void mu::llvmc::module_processor::entry (mu::llvmc::ast::entry * node_a)
{
	global_m.process_node (node_a->function);
    if (global_m.error == nullptr)
    {
        assert (node_a->function->assigned);
        if (node_a->function->generated.size () == 1)
        {
            if (module_m->entry == nullptr)
            {
                auto function_l (dynamic_cast <mu::llvmc::skeleton::function *> (node_a->function->generated [0]));
                if (function_l != nullptr)
                {
                    if (function_l->parameters.size () == 0)
                    {
                        if (function_l->returns.size () == 1)
                        {
                            if (function_l->returns [0].types.size () == 1)
                            {
                                if (function_l->returns [0].types [0]->is_unit_type ())
                                {
                                    module_m->entry = function_l;
                                    node_a->generated.push_back (function_l);
                                    node_a->assigned = true;
                                }
                                else
                                {
                                    global_m.error = new mu::core::error_string (U"Entry point function must return unit", mu::core::error_type::entry_point_must_return_unit);
                                }
                            }
                            else
                            {
                                global_m.error = new mu::core::error_string (U"Entry point function must return unit", mu::core::error_type::entry_point_must_return_unit);
                            }
                        }
                        else
                        {
                            global_m.error = new mu::core::error_string (U"Entry point function must have one return branch", mu::core::error_type::entry_point_must_have_one_return_branch);
                        }
                    }
                    else
                    {
                        global_m.error = new mu::core::error_string (U"Entry point function must take no arguments", mu::core::error_type::entry_point_must_have_no_arguments);
                    }
                }
                else
                {
                    global_m.error = new mu::core::error_string (U"Entry point must be a function", mu::core::error_type::entry_point_must_be_a_function);
                }
            }
            else
            {
                global_m.error = new mu::core::error_string (U"Entry point has already been defined", mu::core::error_type::only_one_entry_point);
            }
        }
        else
        {
            global_m.error = new mu::core::error_string (U"Only one function can be defined as the entry point", mu::core::error_type::only_one_entry_point);
        }
    }
}

void mu::llvmc::function_processor::loop (mu::llvmc::ast::loop * loop_a)
{
	auto loop_s (new mu::llvmc::skeleton::loop (&module_m.module_m->the_unit_type));
	mu::llvmc::skeleton::branch * loop_branch (&mu::llvmc::skeleton::branch::global);
	module_m.process_expression_value_arguments (loop_a->arguments, loop_s->arguments, loop_s->sequenced, loop_branch);
	if (module_m.global_m.error == nullptr)
	{
		if (loop_s->arguments.size () == loop_a->parameters.size ())
		{
			loop_s->loop_entry_branch = loop_branch;
			auto i (loop_a->parameters.begin ());
			auto j (loop_a->parameters.end ());
			auto k (loop_s->arguments.begin ());
			for (; i != j; ++i, ++k)
			{
				auto parameter (mu::cast <mu::llvmc::ast::loop_parameter> (*i));
				auto argument (dynamic_cast<mu::llvmc::skeleton::value *> (*k));
				if (argument != nullptr)
				{
					auto new_parameter (b.loop_parameter (loop_a->region, loop_branch, static_cast<mu::llvmc::skeleton::value *> (*k)->type (), parameter->name));
					loop_s->parameters.push_back (new_parameter);
					parameter->assigned = true;
					parameter->generated.push_back (new_parameter);
				}
				else
				{
					module_m.global_m.error = new mu::core::error_string (U"Loop argument must be a value", mu::core::error_type::loop_argument_must_be_value);
				}
			}
            if (loop_a->results.size () >=2)
            {
                auto previous_origin (module_m.global_m.current_origin);
                module_m.global_m.current_origin = loop_branch;
                mu::llvmc::branch_analyzer branches (module_m.global_m.current_origin, module_m.global_m.error);
                auto & feedback_branch (loop_a->results [0]);
                auto & target_branch (loop_s->add_branch ());
                for (auto i (feedback_branch.nodes.begin ()), j (feedback_branch.nodes.end ()); i != j && module_m.global_m.error == nullptr; ++i)
                {
                    auto value (*i);
                    module_m.global_m.process_node (value);
                    if (module_m.global_m.error == nullptr)
                    {
                        for (auto k (value->generated.begin ()), l (value->generated.end ()); k != l && module_m.global_m.error == nullptr; ++k)
                        {
                            auto value (dynamic_cast <mu::llvmc::skeleton::value *> (*k));
                            if (value != nullptr)
                            {
                                branches.add_branch (value->branch, value->region);
                                target_branch.values.push_back (value);
                            }
                            else
                            {
                                auto sequence (dynamic_cast <mu::llvmc::skeleton::sequence *> (*k));
                                if (sequence != nullptr)
                                {
                                    branches.add_branch (sequence->value->branch, sequence->value->region);
                                    target_branch.sequenced.push_back (sequence->value);
                                }
                                else
                                {
                                    module_m.global_m.error = new mu::core::error_string (U"Loop result must be a value", mu::core::error_type::loop_result_must_be_value);
                                }
                            }
                        }
                    }
                }
                branches.new_set ();
                auto branch (new mu::llvmc::skeleton::branch (loop_branch));
                for (auto i (loop_a->results.begin () + 1), j (loop_a->results.end ()); i != j && module_m.global_m.error == nullptr; ++i)
                {
                    auto & target_branch (loop_s->add_branch ());
                    for (auto k (i->nodes.begin ()), l (i->nodes.end ()); k != l && module_m.global_m.error == nullptr; ++k)
                    {
                        auto value (*k);
                        module_m.global_m.process_node (value);
                        if (module_m.global_m.error == nullptr)
                        {
                            for (auto m (value->generated.begin ()), n (value->generated.end ()); m != n && module_m.global_m.error == nullptr; ++m)
                            {
                                auto value (dynamic_cast <mu::llvmc::skeleton::value *> (*m));
                                if (value != nullptr)
                                {
                                    branches.add_branch (value->branch, value->region);
                                    target_branch.values.push_back (value);
                                    auto element (b.loop_element (value->region, branch, loop_s, value->type ()));
                                    loop_s->elements.push_back (element);
                                }
                                else
                                {
                                    auto sequence (dynamic_cast <mu::llvmc::skeleton::sequence *> (*m));
                                    if (sequence != nullptr)
                                    {
                                        branches.add_branch (sequence->value->branch, sequence->value->region);
                                        target_branch.sequenced.push_back (sequence->value);
                                    }
                                    else
                                    {
                                        module_m.global_m.error = new mu::core::error_string (U"Loop result must be a value", mu::core::error_type::loop_result_must_be_value);
                                    }
                                }
                            }
                        }
                    }
                    branches.new_set ();
                    if (module_m.global_m.error == nullptr)
                    {
                        if (target_branch.values.empty ())
                        {
                            module_m.global_m.error = new mu::core::error_string (U"Loop result branches must return a value", mu::core::error_type::branches_must_return_a_value);
                        }
                    }
                    if (module_m.global_m.error == nullptr)
                    {
                        loop_a->assigned = true;
                        assert (!loop_s->elements.empty ());
                        loop_a->generated.insert (loop_a->generated.begin (), loop_s->elements.begin (), loop_s->elements.end ());
                    }
                }
                module_m.global_m.current_origin = previous_origin;
            }
            else
            {
                module_m.global_m.error = new mu::core::error_string (U"Loop must have one feedback branch and at least one result branch", mu::core::error_type::loop_not_enough_branches);
            }
		}
		else
		{
			module_m.global_m.error = new mu::core::error_string (U"Number of arguments does not match number of parameters", mu::core::error_type::mismatch_number_of_arguments_number_of_parameters);
		}
	}
}

void mu::llvmc::module_processor::expression (mu::llvmc::ast::expression * expression_a)
{
	auto existing (current_expression_generation.find (expression_a));
	if (existing == current_expression_generation.end ())
	{
		current_expression_generation.insert (expression_a);
		if (!expression_a->arguments.empty ())
		{
			global_m.process_node (expression_a->arguments [0]);
			if (global_m.error == nullptr)
			{
				mu::llvmc::skeleton::node * target;
				auto & generated = expression_a->arguments [0]->generated;
				if (!generated.empty ())
				{
					target = generated [0];
                    assert (target != nullptr);
				}
				else
				{
					global_m.error = new mu::core::error_string (U"Expecting target", mu::core::error_type::expecting_a_target, expression_a->arguments [0]->region);
				}
				if (global_m.error == nullptr)
				{
					auto identity (dynamic_cast <mu::llvmc::skeleton::identity *> (target));
					if (identity != nullptr)
					{
						process_identity (expression_a);
					}
					else
					{
						auto template_l (dynamic_cast <mu::llvmc::skeleton::template_c *> (target));
						if (template_l != nullptr)
						{
							process_template (expression_a);
						}
						else
						{
							auto constant_int_l (dynamic_cast <mu::llvmc::skeleton::constant_int_c *> (target));
							if (constant_int_l != nullptr)
							{
								process_constant_int (expression_a);
							}
							else
							{
								global_m.error = new mu::core::error_string (U"Expecting first argument to be call target", mu::core::error_type::expecting_first_argument_to_be_call_target, expression_a->arguments [0]->region);
							}
						}
					}
				}
			}
		}
		else
		{
			global_m.error = new mu::core::error_string (U"Expecting a call target", mu::core::error_type::expecting_a_call_target, expression_a->region);
		}
	}
	else
	{
		global_m.error = new mu::core::error_string (U"Cycle in expressions", mu::core::error_type::cycle_in_expressions, expression_a->region);
	}
	current_expression_generation.erase (expression_a);
	assert (current_expression_generation.find (expression_a) == current_expression_generation.end ());
}

void mu::llvmc::function_processor::expression (mu::llvmc::ast::expression * expression_a)
{
	auto call_parent (false);
	auto existing (module_m.current_expression_generation.find (expression_a));
	if (existing == module_m.current_expression_generation.end ())
	{
		module_m.current_expression_generation.insert (expression_a);
		if (!expression_a->arguments.empty ())
		{
			module_m.global_m.process_node (expression_a->arguments [0]);
			if (module_m.global_m.error == nullptr)
			{
				mu::llvmc::skeleton::node * target;
				auto & generated = expression_a->arguments [0]->generated;
				if (!generated.empty ())
				{
					target = generated [0];
                    assert (target != nullptr);
				}
				else
				{
					module_m.global_m.error = new mu::core::error_string (U"Expecting target", mu::core::error_type::expecting_a_target, expression_a->arguments [0]->region);
				}
				if (module_m.global_m.error == nullptr)
				{
					auto value (dynamic_cast <mu::llvmc::skeleton::value *> (target));
					if (value != nullptr)
					{
						process_value_call (expression_a);
					}
					else
					{
						auto marker (dynamic_cast <mu::llvmc::skeleton::marker *> (target));
						if (marker != nullptr)
						{
							process_marker (expression_a);
						}
						else
						{
							auto asm_l (dynamic_cast <mu::llvmc::skeleton::asm_c *> (target));
							if (asm_l != nullptr)
							{
								process_asm (expression_a);
							}
							else
							{
								call_parent = true;
							}
						}
					}
				}
			}
		}
		else
		{
			module_m.global_m.error = new mu::core::error_string (U"Expecting a call target", mu::core::error_type::expecting_a_call_target, expression_a->region);
		}
	}
	else
	{
		module_m.global_m.error = new mu::core::error_string (U"Cycle in expressions", mu::core::error_type::cycle_in_expressions, expression_a->region);
	}
	module_m.current_expression_generation.erase (expression_a);
	assert (module_m.current_expression_generation.find (expression_a) == module_m.current_expression_generation.end ());
	if (call_parent)
	{
		assert (!expression_a->assigned);
		expression_a->visit (&module_m);
	}
}

void mu::llvmc::module_processor::struct_type (mu::llvmc::ast::struct_type * node_a)
{
	auto struct_l (b.struct_type ());
	for (auto i: node_a->elements)
	{
		auto type (process_type (i));
		if (type != nullptr)
		{
			struct_l->elements.push_back (type);
		}
		else
		{
			global_m.error = new mu::core::error_string (U"Struct type definition must list other types", mu::core::error_type::struct_must_contain_types, i->region);
		}
	}
	for (auto i: node_a->names)
	{
		assert (struct_l->names.find (i.first) == struct_l->names.end ());
		struct_l->names [i.first] = b.constant_integer (node_a->region, &module_m->integer_32_type, i.second);
	}
	if (global_m.error == nullptr)
	{
		node_a->assigned = true;
		node_a->generated.push_back (struct_l);
	}
}

void mu::llvmc::function_processor::undefined (mu::llvmc::ast::undefined * node_a)
{
	auto type (module_m.process_type (node_a->type));
	if (type != nullptr)
	{
		auto undefined_l (b.undefined (node_a->region, &mu::llvmc::skeleton::branch::global, type));
		node_a->assigned = true;
		node_a->generated.push_back (undefined_l);
	}
	else
	{
		module_m.global_m.error = new mu::core::error_string (U"Expecting a type", mu::core::error_type::expecting_a_type, node_a->region);
	}
}

void mu::llvmc::module_processor::template_c (mu::llvmc::ast::template_c * node_a)
{
	auto node (b.template_c (node_a->base));
	node->body = node_a->body;
	node->parameters = node_a->parameters;
	node_a->assigned = true;
	node_a->generated.push_back (node);
}

void mu::llvmc::module_processor::process_template (mu::llvmc::ast::expression * node_a)
{
    mu::vector <mu::llvmc::skeleton::node *> arguments;
    for (auto i (node_a->arguments.begin ()), j (node_a->arguments.end ()); i != j && global_m.error == nullptr; ++i)
    {
        auto value (*i);
        global_m.process_node (value);
        auto & nodes (value->generated);
        arguments.insert (arguments.end (), nodes.begin (), nodes.end ());
    }
    if (global_m.error == nullptr)
    {
        auto template_l (mu::cast <mu::llvmc::skeleton::template_c> (arguments [0]));
        if (arguments.size () - 1 == template_l->parameters.size ())
        {
            mu::llvmc::clone_context context (template_l->base);
            {
                auto i (template_l->parameters.begin ());
                auto j (template_l->parameters.end ());
                auto k (arguments.begin () + 1);
                while (i != j)
                {
                    auto value ((*i)->clone (context));
                    value->assigned = true;
                    value->generated.push_back (*k);
                    ++i;
                    ++k;
                }
                assert (k == arguments.end ());
            }
            auto & target (node_a->generated);
            for (auto i (template_l->body.begin ()), j (template_l->body.end ()); i != j && global_m.error == nullptr; ++i)
            {
                auto orig (*i);
                auto value (orig->clone (context));
                global_m.process_node (value);
                auto & nodes (value->generated);
                target.insert (target.end (), nodes.begin (), nodes.end ());
            }
            if (global_m.error == nullptr)
            {
                node_a->assigned = true;
            }
        }
        else
        {
            global_m.error = new mu::core::error_string (U"Number of template arguments doesn't match number of parameters", mu::core::error_type::template_argument_count_mismatch);
        }
    }
}

void mu::llvmc::global_processor::process_node (mu::llvmc::ast::node * node_a)
{
	assert (node_a != nullptr);
	assert (error == nullptr);
	if (!node_a->assigned)
	{
		analysis_stack.push_back (node_a);
		node_a->visit (current_context);
		analysis_stack.pop_back ();
	}
	assert ((error != nullptr) xor (node_a->assigned));
}

std::string mu::llvmc::global_processor::print_analysis_stack ()
{
	std::string result;
	for (auto i: analysis_stack)
	{
		mu::string string (i->region.string ());
		result.append (string.begin (), string.end ());
		result.push_back (U'\n');
	}
	return result;
}

void mu::llvmc::function_processor::process_asm (mu::llvmc::ast::expression * asm_a)
{
	mu::vector <mu::llvmc::skeleton::node *> arguments;
    mu::vector <mu::llvmc::skeleton::value *> sequenced;
	mu::llvmc::skeleton::branch * most_specific_branch (&mu::llvmc::skeleton::branch::global);
	module_m.process_expression_value_arguments (asm_a->arguments, arguments, sequenced, most_specific_branch);
	if (module_m.global_m.error == nullptr)
	{
		assert (dynamic_cast <mu::llvmc::skeleton::asm_c *> (arguments [0]) != nullptr);
		auto instruction (b.inline_asm (asm_a->region, most_specific_branch, arguments, sequenced));
		asm_a->assigned = true;
		asm_a->generated.push_back (instruction);
	}
}

mu::llvmc::skeleton::number::number (uint64_t value_a) :
value (value_a)
{
}

void mu::llvmc::global_processor::number (mu::llvmc::ast::number * number_a)
{
	std::string data_l (number_a->number_m.begin (), number_a->number_m.end ());
	if (data_l.size () > 0)
	{
		auto prefix (data_l [0]);
		std::unique_ptr <uint8_t []> remaining (new uint8_t [data_l.size () + 1]);
		uint64_t value;
		int parsed;
		switch (prefix)
		{
			case 'h':		
				parsed = sscanf (&data_l.c_str () [1], "%" PRIx64 " %s", &value, remaining.get ());
				break;
			case 'd':
				parsed = sscanf (&data_l.c_str () [1], "%" PRIu64 " %s", &value, remaining.get ());		
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				parsed = sscanf (&data_l.c_str () [0], "%" PRIu64 " %s", &value, remaining.get ());		
				break;
			case 'o':
				parsed = sscanf (&data_l.c_str () [1], "%" PRIo64 " %s", &value, remaining.get ());		
				break;
			default:
				parsed = 0;
				break;
		}
		if (parsed == 1)
		{
			auto result (new mu::llvmc::skeleton::number (value));
			number_a->assigned = true;
			number_a->generated.push_back (result);
		}
		else
		{
			error = new mu::core::error_string (U"Unable to convert string to number", mu::core::error_type::error_converting_string_to_number, number_a->region);
		}
	}
	else
	{
		error = new mu::core::error_string (U"Unable to convert string to number", mu::core::error_type::error_converting_string_to_number, number_a->region);
	}
}

void mu::llvmc::module_processor::process_single_node (mu::llvmc::ast::node * node_a)
{
	assert (node_a != nullptr);
	global_m.process_node (node_a);
    auto size (node_a->generated.size ());
	if (global_m.error == nullptr && size != 1)
	{
		global_m.error = new mu::core::error_string (U"Expecting 1 value", mu::core::error_type::expecting_one_value, node_a->region);
	}
}

mu::llvmc::skeleton::value * mu::llvmc::function_processor::process_value (mu::llvmc::ast::node * node_a)
{
	assert (node_a != nullptr);
	mu::llvmc::skeleton::value * result (nullptr);
	module_m.process_single_node (node_a);
	if (module_m.global_m.error == nullptr)
	{
        auto & nodes (node_a->generated);
        assert (nodes.size () == 1);
		result = dynamic_cast <mu::llvmc::skeleton::value *> (nodes [0]);
		if (result == nullptr)
		{
			module_m.global_m.error = new mu::core::error_string (U"Node is not value", mu::core::error_type::node_is_not_a_value, node_a->region);
		}
	}
	return result;
}

mu::llvmc::skeleton::type * mu::llvmc::module_processor::process_type (mu::llvmc::ast::node * node_a)
{
	mu::llvmc::skeleton::type * result (nullptr);
	process_single_node (node_a);
	if (global_m.error == nullptr)
	{
		auto & nodes (node_a->generated);
        assert (nodes.size () == 1);
		result = dynamic_cast<mu::llvmc::skeleton::type *> (nodes [0]);
	}
	return result;
}

void mu::llvmc::function_processor::sequence (mu::llvmc::ast::sequence * sequence_a)
{
    module_m.global_m.process_node (sequence_a->node_m);
    for (auto i (sequence_a->node_m->generated.begin ()), j (sequence_a->node_m->generated.end ()); module_m.global_m.error == nullptr && i != j; ++i)
    {
        auto value (dynamic_cast <mu::llvmc::skeleton::value *> (*i));
        if (value != nullptr)
        {
            sequence_a->generated.push_back (new mu::llvmc::skeleton::sequence (value));
        }
        else
        {
            module_m.global_m.error = new mu::core::error_string (U"Expecting a value", mu::core::error_type::expecting_a_value);
        }
    }
    if (module_m.global_m.error == nullptr)
    {
        sequence_a->assigned = true;
    }
}

void mu::llvmc::function_processor::process_returns ()
{
	if (node_m->returns.size () == node_m->results.branches.size ())
	{
		for (auto i (node_m->returns.begin ()), j (node_m->returns.end ()); i != j && module_m.global_m.error == nullptr; ++i)
		{
			function_m->returns.push_back (decltype (function_m->returns)::value_type ());
			for (auto k ((*i).types.begin ()), l ((*i).types.end ()); k != l && module_m.global_m.error == nullptr; ++k)
			{
				auto type (module_m.process_type (*k));
				if (type != nullptr)
				{
					function_m->returns.back ().types.push_back (type);
				}
				else
				{
					module_m.global_m.error = new mu::core::error_string (U"Expecting a type", mu::core::error_type::expecting_a_type, (*k)->region);
				}
			}
		}
	}
	else
	{
		module_m.global_m.error = new mu::core::error_string (U"Number of formal return branches does not match number of actual result branches", mu::core::error_type::number_of_formal_return_branches_not_matching_number_of_actual_results);
	}
}

void mu::llvmc::function_processor::process_results ()
{
	mu::llvmc::branch_analyzer branches (module_m.global_m.current_origin, module_m.global_m.error);
	auto o (function_m->returns.begin ());
	for (auto i (node_m->results.branches.begin ()), j (node_m->results.branches.end ()); module_m.global_m.error == nullptr && i != j; ++i, ++o)
	{
		assert (o != function_m->returns.end ());
		auto & current_branch (function_m->results.add_branch ());
		auto p (o->types.begin ());
		auto q (o->types.end ());
		for (auto k (i->nodes.begin ()), l (i->nodes.end ()); module_m.global_m.error == nullptr && k != l; ++k)
		{
			auto node_a (*k);
			module_m.global_m.process_node (*k);
			for (auto m (node_a->generated.begin ()), n (node_a->generated.end ()); module_m.global_m.error == nullptr && m != n; ++m)
			{
				auto node_l (*m);
				auto sequence (dynamic_cast <mu::llvmc::skeleton::sequence *> (node_l));
				if (sequence != nullptr)
				{
					branches.add_branch (sequence->value->branch, sequence->value->region);
					current_branch.sequenced.push_back (sequence->value);
				}
				else
				{
					if (p != q)
					{
						auto new_value (node_l->adapt_result (*p, *this,
							[]
							(mu::core::region const & region_a)
							{
								return new mu::core::error_string (U"Actual result type does not match formal result type", mu::core::error_type::actual_formal_result_type_mismatch, region_a);
							}));
						if (new_value != nullptr)
						{
							branches.add_branch (new_value->branch, new_value->region);
							current_branch.results.push_back (new_value);
							++p;
						}
					}
					else
					{
						module_m.global_m.error = new mu::core::error_string (U"Number of formal returns in branch does not match number of actual results in branch", mu::core::error_type::number_of_formal_returns_not_matching_number_of_actual_results);
					}
				}
			}
			if (module_m.global_m.error == nullptr)
			{
				if (current_branch.results.empty ())
				{
					module_m.global_m.error = new mu::core::error_string (U"Functions must return at least one value", mu::core::error_type::branches_must_return_a_value);
				}
			}
		}
		if (module_m.global_m.error == nullptr && p != q)
		{
			module_m.global_m.error = new mu::core::error_string (U"Number of formal returns in branch does not match number of actual results in branch", mu::core::error_type::number_of_formal_returns_not_matching_number_of_actual_results);
		}
		branches.new_set ();
	}
}

void mu::llvmc::module_processor::process_identity (mu::llvmc::ast::expression * expression_a)
{
	mu::vector <mu::llvmc::skeleton::node *> arguments;
    mu::vector <mu::llvmc::skeleton::value *> sequenced;
	mu::llvmc::skeleton::branch * most_specific_branch (&mu::llvmc::skeleton::branch::global);
	process_expression_value_arguments (expression_a->arguments, arguments, sequenced, most_specific_branch);
    auto source (new mu::llvmc::skeleton::identity_call (arguments, sequenced, &module_m->the_unit_type));
    auto argument_size (arguments.size ());
    switch (argument_size)
    {
        case 0:
            assert (false);
            break;
        case 1:
        {
            auto element (b.identity_element (most_specific_branch, source, &module_m->the_unit_type));
            source->elements.push_back (element);
            expression_a->generated.push_back (element);
            break;
        }
        default:
        {
            auto & values (expression_a->generated);
            for (size_t i (1); i < argument_size; ++i)
            {
                auto element (b.identity_element (most_specific_branch, source, mu::cast <mu::llvmc::skeleton::value> (values [i])->type ()));
                source->elements.push_back (element);
                values.push_back (element);
            }
            break;
        }
    }
    if (argument_size == 2)
    {
    }
    else
    {
        auto & results (expression_a->generated);
        results.insert (results.end (), arguments.begin () + 1, arguments.begin () + argument_size);
    }
}

void mu::llvmc::function_processor::process_value_call (mu::llvmc::ast::expression * expression_a)
{
	mu::vector <mu::llvmc::skeleton::node *> arguments;
    mu::vector <mu::llvmc::skeleton::value *> sequenced;
	mu::llvmc::skeleton::branch * most_specific_branch (function_m->entry);
	module_m.process_expression_value_arguments (expression_a->arguments, arguments, sequenced, most_specific_branch);
	auto target (static_cast<mu::llvmc::skeleton::value *> (arguments [0]));
	auto type_l (target->type ());
	auto pointer_type (dynamic_cast<mu::llvmc::skeleton::pointer_type *> (type_l));
	if (pointer_type != nullptr)
	{
		auto function_type (dynamic_cast<mu::llvmc::skeleton::function_type *> (pointer_type->pointed_type));
		if (function_type != nullptr)
		{
			auto k (arguments.begin () + 1);
			auto l (arguments.end ());
			size_t i (0);
			size_t j (function_type->function->parameters.size ());
			for (; i != j && k != l && module_m.global_m.error == nullptr; ++i, ++k)
			{
				auto argument_node (*k);
				if (argument_node != nullptr)
				{
					auto parameter_type (function_type->function->parameters [i]->type ());
					auto new_value ((*k)->adapt (parameter_type, *this,
						[argument_node, parameter_type]
						(mu::core::region const & region_a)
						{
							mu::string message (U"Argument type does not match parameter type: ");
							/*message.append (argument_node->type ()->name ());
							message.push_back (U' ');*/
							message.append (parameter_type->name ());
							return new mu::core::error_string (message, mu::core::error_type::argument_type_does_not_match_parameter_type, region_a);
						}));
					*k = new_value;
				}
				else
				{
					module_m.global_m.error = new mu::core::error_string (U"Argument to function is not a value", mu::core::error_type::argument_to_function_is_not_a_value, expression_a->region);
				}
			}
			if (module_m.global_m.error == nullptr)
			{
				if ((i == j) == (k == l))
				{
					if (!arguments.empty ())
					{
						auto call (new mu::llvmc::skeleton::function_call (function_type->function, most_specific_branch, arguments, sequenced, &module_m.module_m->the_unit_type));
						mu::vector <mu::llvmc::skeleton::node *> returned_results;
                        mu::llvmc::skeleton::branch * branch;
                        if (function_type->function->results.size () < 2)
                        {
                            branch = most_specific_branch;
                        }
                        else
                        {
                            branch = new mu::llvmc::skeleton::branch (most_specific_branch);
                        }
                        for (auto & i: function_type->function->returns)
                        {
                            if (i.types.empty ())
                            {
                                auto element (b.call_element (expression_a->region, branch, call, &module_m.module_m->the_unit_type));
                                returned_results.push_back (element);
                                call->elements.push_back (element);
                            }
                            else
                            {
                                for (auto j: i.types)
                                {
                                    auto element (b.call_element (expression_a->region, branch, call, j));
                                    returned_results.push_back (element);
                                    call->elements.push_back (element);
                                }
                            }
                            branch = new mu::llvmc::skeleton::branch (most_specific_branch);
                        }
						expression_a->assigned = true;
                        switch (returned_results.size ())
                        {
                            case 0:
                            {
                                auto element (b.call_element (expression_a->region, most_specific_branch, call, &module_m.module_m->the_unit_type));
                                expression_a->generated.push_back (element);
                                call->elements.push_back (element);
                                break;
                            }
                            case 1:
                            {
                                expression_a->generated.push_back (returned_results [0]);
                                break;
                            }
                            default:
                            {
                                auto & target (expression_a->generated);
                                target.insert (target.end (), returned_results.begin (), returned_results.end ());
                                break;
                            }
                        }
					}
				}
				else
				{
					module_m.global_m.error = new mu::core::error_string (U"Incorrect number of arguments", mu::core::error_type::incorrect_number_of_arguments, expression_a->region);
				}
			}
		}
		else
		{
			module_m.global_m.error = new mu::core::error_string (U"Pointer does not point to a function", mu::core::error_type::pointer_does_not_point_to_a_function, expression_a->region);
		}
	}
	else
	{
		module_m.global_m.error = new mu::core::error_string (U"Only function pointers can be the target of a call", mu::core::error_type::only_function_pointers_can_be_target_of_call, expression_a->region);
	}
	module_m.current_expression_generation.erase (expression_a);
}

void mu::llvmc::module_processor::process_expression_value_arguments (mu::vector <mu::llvmc::ast::node *> const & arguments, mu::vector <mu::llvmc::skeleton::node *> & arguments_a, mu::vector <mu::llvmc::skeleton::value *> & sequenced_a, mu::llvmc::skeleton::branch * & most_specific_branch)
{
    mu::llvmc::branch_analyzer branches (global_m.current_origin, global_m.error);
    for (auto i (arguments.begin ()), j (arguments.end ()); i != j && global_m.error == nullptr; ++i)
    {
        auto node_a (*i);
        global_m.process_node (node_a);
        if (global_m.error == nullptr)
        {
            auto & nodes (node_a->generated);
            for (auto k (nodes.begin ()), l (nodes.end ()); k != l && global_m.error == nullptr; ++k)
            {
                auto node (*k);
                if (node_a->is_sequenced ())
                {
                    auto value (mu::cast <mu::llvmc::skeleton::sequence> (node)->value);
                    most_specific_branch = branches.add_branch (value->branch, node_a->region);
                    sequenced_a.push_back (value);
                }
                else
                {
                    auto value (dynamic_cast <mu::llvmc::skeleton::value *> (node));
                    if (value != nullptr)
                    {
                        most_specific_branch = branches.add_branch (value->branch, node_a->region);
                    }
                    arguments_a.push_back (node);
                }
            }
        }
    }
}

static unsigned minimum_bit_width (mu::core::error * & error_a, mu::llvmc::skeleton::node * node_a)
{
	unsigned result (0);
	auto value (dynamic_cast <mu::llvmc::skeleton::value *> (node_a));
	if (value != nullptr)
	{
		auto type (dynamic_cast <mu::llvmc::skeleton::integer_type *> (value->type ()));
		if (type != nullptr)
		{
			result = type->bits;
		}
		else
		{
			error_a = new mu::core::error_string (U"Instruction argument must be an integer type", mu::core::error_type::instruction_arguments_must_be_integers, mu::core::region ());
		}
	}
	else
	{
		auto number (dynamic_cast <mu::llvmc::skeleton::number *> (node_a));
		if (number != nullptr)
		{
			result = number->bits_required ();
		}
		else
		{
			error_a = new mu::core::error_string (U"Instruction argument must be a value", mu::core::error_type::instruction_arguments_must_be_values, mu::core::region ());
		}
	}
	return result;
}

template <typename ... Args>
static unsigned minimum_bit_width (mu::core::error * & error_a, mu::llvmc::skeleton::node * node_a, Args ... args)
{
	unsigned result (0);
	unsigned lhs (minimum_bit_width (error_a, node_a));
	if (error_a == nullptr)
	{
		unsigned rhs (minimum_bit_width (error_a, args ...));
		if (error_a == nullptr)
		{
			result = std::max (lhs, rhs);
		}
	}
	return result;
}

void mu::llvmc::function_processor::process_binary_integer_instruction (mu::llvmc::ast::expression * expression_a, mu::vector <mu::llvmc::skeleton::node *> & arguments, mu::vector <mu::llvmc::skeleton::value *> & sequenced, mu::llvmc::skeleton::branch * most_specific_branch)
{
	if (arguments.size () == 3)
	{
		auto width (minimum_bit_width (module_m.global_m.error, arguments [1], arguments [2]));
		if (module_m.global_m.error == nullptr)
		{
			auto type (b.integer_type (width));
			auto error_action ([]
					(mu::core::region const &)
					{
						assert (false);
						return nullptr;
					});
			arguments [1] = arguments [1]->adapt (type, *this, error_action);
			arguments [2] = arguments [2]->adapt (type, *this, error_action);
			expression_a->assigned = true;
			expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
		}
	}
	else
	{
		module_m.global_m.error = new mu::core::error_string (U"Instruction expects two arguments", mu::core::error_type::instruction_expects_two_arguments, expression_a->region);
	}
}

void mu::llvmc::function_processor::process_marker (mu::llvmc::ast::expression * expression_a)
{
	mu::vector <mu::llvmc::skeleton::node *> arguments;
    mu::vector <mu::llvmc::skeleton::value *> sequenced;
	mu::llvmc::skeleton::branch * most_specific_branch (&mu::llvmc::skeleton::branch::global);
	module_m.process_expression_value_arguments (expression_a->arguments, arguments, sequenced, most_specific_branch);
    auto result (false);
    if (module_m.global_m.error == nullptr)
    {
        auto marker (static_cast<mu::llvmc::skeleton::marker *> (arguments [0]));
        switch (marker->type)
        {
            case mu::llvmc::instruction_type::add:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::alloca:
            {
                if (arguments.size () == 2)
                {
                    auto type (dynamic_cast <mu::llvmc::skeleton::type *> (arguments [1]));
                    if (type != nullptr)
                    {
						expression_a->assigned = true;
                        expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"Alloca instruction expects its argument to be a type", mu::core::error_type::alloca_argument_type, expression_a->region);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"Alloca instruction expects one argument", mu::core::error_type::alloca_expects_one_argument, expression_a->region);
                }
                break;
            }
            case mu::llvmc::instruction_type::and_i:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::ashr:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::asm_i:
            {
                if (arguments.size () >= 2)
                {
                    auto asm_l (dynamic_cast <mu::llvmc::skeleton::asm_c *> (arguments [1]));
                    if (asm_l != nullptr)
                    {
                        for (auto i (arguments.begin () + 2), j (arguments.end ()); i < j && module_m.global_m.error == nullptr; ++i)
                        {
                            auto value (dynamic_cast <mu::llvmc::skeleton::value *> (*i));
                            if (value == nullptr)
                            {
                                module_m.global_m.error = new mu::core::error_string (U"Inline asm requires value arguments", mu::core::error_type::inline_asm_requires_values, expression_a->region);
                            }
                        }
                    }
                    else
                    {
                        assert (false);
                    }
                }
                break;
            }
            case mu::llvmc::instruction_type::bitcast:
            {
                if (arguments.size () >= 3)
                {
                    auto value (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [1]));
                    if (value != nullptr)
                    {
                        auto type (dynamic_cast <mu::llvmc::skeleton::type *> (arguments [2]));
                        if (type != nullptr)
                        {
                            auto value_pointer (dynamic_cast <mu::llvmc::skeleton::pointer_type *> (value->type ()));
                            if (value_pointer != nullptr)
                            {
                                auto type_pointer (dynamic_cast <mu::llvmc::skeleton::pointer_type *> (type));
                                if (type_pointer != nullptr)
                                {
									expression_a->assigned = true;
                                    expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
                                }
                                else
                                {
                                    module_m.global_m.error = new mu::core::error_string (U"Second argument to bitcast must be a pointer type", mu::core::error_type::argument_must_be_pointer_type, expression_a->region);
                                }
                            }
                            else
                            {
                                module_m.global_m.error = new mu::core::error_string (U"First argument to bitcast must be a pointer type", mu::core::error_type::argument_must_be_pointer_type, expression_a->region);
                            }
                        }
                        else
                        {
                            module_m.global_m.error = new mu::core::error_string (U"Second argument to bitcast must be a type", mu::core::error_type::expecting_a_type, expression_a->region);
                        }
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"First argument to bitcast must be a value", mu::core::error_type::instruction_arguments_must_be_values, expression_a->region);
                    }
                }
                break;
            }
            case mu::llvmc::instruction_type::cmpxchg:
            {
                if (arguments.size () == 4)
                {
                    auto one (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [1]));
                    if (one != nullptr)
                    {
                        auto two (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [2]));
                        if (two != nullptr)
                        {
                            auto three (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [3]));
                            if (three != nullptr)
                            {
                                auto one_type (dynamic_cast <mu::llvmc::skeleton::pointer_type *> (one->type()));
                                if (one_type != nullptr)
                                {
                                    if (*one_type->pointed_type == *two->type ())
                                    {
                                        if (*one_type->pointed_type == *three->type ())
                                        {
											expression_a->assigned = true;
                                            expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
                                        }
                                        else
                                        {
                                            module_m.global_m.error = new mu::core::error_string (U"Cmpxchg requires argument one to point to type of argument three", mu::core::error_type::cmpxchg_one_point_three, expression_a->region);
                                        }
                                    }
                                    else									
                                    {
                                        module_m.global_m.error = new mu::core::error_string (U"Cmpxchg requires argument one to point to type of argument two", mu::core::error_type::cmpxchg_one_point_two, expression_a->region);
                                    }
                                }
                                else
                                {
                                    module_m.global_m.error = new mu::core::error_string (U"Cmpxchg requires argument one to be a pointer", mu::core::error_type::cmpxchg_argument_one_pointer, expression_a->region);
                                }
                            }
                            else
                            {
                                module_m.global_m.error = new mu::core::error_string (U"Cmpxchg requires arguments to be values", mu::core::error_type::instruction_arguments_must_be_values, expression_a->region);
                            }
                        }
                        else
                        {
                            module_m.global_m.error = new mu::core::error_string (U"Cmpxchg requires arguments to be values", mu::core::error_type::instruction_arguments_must_be_values, expression_a->region);
                        }
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"Cmpxchg requires arguments to be values", mu::core::error_type::instruction_arguments_must_be_values, expression_a->region);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"Cmpxchg requires three arguments", mu::core::error_type::cmpxchg_requires_three_arguments, expression_a->region);
                }
                break;
            }
            case mu::llvmc::instruction_type::extractvalue:
            {
                if (arguments.size () == 3)
                {
                    auto aggregate (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [1]));
                    if (aggregate != nullptr)
                    {
                        auto struct_l (dynamic_cast <mu::llvmc::skeleton::struct_type *> (aggregate->type ()));
                        if (struct_l != nullptr)
                        {
                            auto index (dynamic_cast <mu::llvmc::skeleton::constant_integer *> (arguments [2]));
                            if (index != nullptr)
                            {
                                if (index->value_m < struct_l->elements.size ())
                                {
									expression_a->assigned = true;
                                    expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
                                }
                                else
                                {
                                    module_m.global_m.error = new mu::core::error_string (U"Extractvalue index is out of range", mu::core::error_type::index_out_of_bounds, expression_a->region);
                                }
                            }
                            else
                            {
                                module_m.global_m.error = new mu::core::error_string (U"Extractvalue requires second argument to be a constant", mu::core::error_type::expecting_a_constant, expression_a->region);
                            }
                        }
                        else
                        {
                            module_m.global_m.error = new mu::core::error_string (U"Exctractvalue requires first argument to be an aggregate type", mu::core::error_type::expecting_an_aggregate, expression_a->region);
                        }
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"Exctractvalue requires first arguments to be a values", mu::core::error_type::expecting_a_value, expression_a->region);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"Extractvalue requires two arguments", mu::core::error_type::incorrect_number_of_arguments, expression_a->region);
                }
                break;
            }
			case mu::llvmc::instruction_type::getelementptr:
			{
				if (arguments.size () >= 3)
				{
					auto ptr (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [1]));
					if (ptr != nullptr)
					{
						auto ptr_index (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [2]));
						if (ptr_index != nullptr)
						{							
							auto ptr_type (dynamic_cast <mu::llvmc::skeleton::pointer_type *> (ptr->type ()));
							if (ptr_type != nullptr)
							{
								auto ptr_index_type (dynamic_cast <mu::llvmc::skeleton::integer_type  *> (ptr_index->type ()));
								if (ptr_index_type != nullptr)
								{
									for (auto i (arguments.begin () + 3), j (arguments.end ()); i != j && module_m.global_m.error == nullptr; ++i)
									{
										auto index (dynamic_cast <mu::llvmc::skeleton::constant_integer *> (*i));
										if (index != nullptr)
										{
											if (mu::cast <mu::llvmc::skeleton::integer_type> (index->type ())->bits != 32)
											{
												module_m.global_m.error = new mu::core::error_string (U"Getelementptr requires trailing index types to be 32bit integers", mu::core::error_type::getelementptr_trailing_32bit, expression_a->region);
											}
										}
										else
										{
											module_m.global_m.error = new mu::core::error_string (U"Getelementptr requires trailing indicies to be constant integers", mu::core::error_type::getelementptr_trailing_constant, expression_a->region);
										}
									}
									if (module_m.global_m.error == nullptr)
									{
										expression_a->assigned = true;
										expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
									}
								}
								else
								{
									module_m.global_m.error = new mu::core::error_string (U"Getelementptr requires pointer index to be an integer", mu::core::error_type::getelementptr_first_argument_integer_type, expression_a->region);
								}
							}
							else
							{
								module_m.global_m.error = new mu::core::error_string (U"Getelementptr first argument must be a pointer", mu::core::error_type::getelementptr_requires_pointer_type, expression_a->region);
							}
						}
						else
						{
							module_m.global_m.error = new mu::core::error_string (U"Getelementptr requires its arguments to be values", mu::core::error_type::getelementptr_requires_values, expression_a->region);
						}
					}
					else
					{
						module_m.global_m.error = new mu::core::error_string (U"Getelementptr requires its arguments to be values", mu::core::error_type::getelementptr_requires_values, expression_a->region);
					}
				}
				else
				{
					module_m.global_m.error = new mu::core::error_string (U"Getelementptr requires at least two arguments", mu::core::error_type::getelementptr_requires_two, expression_a->region);
				}
				break;
			}
            case mu::llvmc::instruction_type::icmp:
            {
                if (arguments.size () == 4)
                {
                    auto predicate (dynamic_cast <mu::llvmc::skeleton::predicate *> (arguments [1]));
                    if (predicate != nullptr)
                    {
                        auto left (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [2]));
                        if (left != nullptr)
                        {
                            auto right (dynamic_cast<mu::llvmc::skeleton::value *> (arguments [3]));
                            if (right != nullptr)
                            {
                                auto left_type (dynamic_cast <mu::llvmc::skeleton::integer_type *> (left->type ()));
                                if (left_type != nullptr)
                                {
                                    if (*left->type () == *right->type ())
                                    {
                                        auto icmp (b.icmp (expression_a->region, most_specific_branch, &module_m.module_m->integer_1_type, predicate, left, right, sequenced));
										expression_a->assigned = true;
                                        expression_a->generated.push_back (icmp);
                                    }
                                    else
                                    {
                                        module_m.global_m.error = new mu::core::error_string (U"ICMP arguments must be the same type", mu::core::error_type::icmp_arguments_same_type, expression_a->region);
                                    }
                                }
                                else
                                {
                                    module_m.global_m.error = new mu::core::error_string (U"ICMP arguments must be integers", mu::core::error_type::icmp_arguments_integers, expression_a->region);
                                }
                            }
                            else
                            {
                                module_m.global_m.error = new mu::core::error_string (U"ICMP right argument must be a value", mu::core::error_type::icmp_right_argument_value, expression_a->region);
                            }
                        }
                        else
                        {
                            module_m.global_m.error = new mu::core::error_string (U"ICMP left argument must be a value", mu::core::error_type::icmp_left_argument_value, expression_a->region);
                        }
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"ICMP first argument must be predicate", mu::core::error_type::icmp_first_argument_predicate, expression_a->region);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"ICMP instruction expects a predicate and two arguments", mu::core::error_type::icmp_expects_predicate_two_arguments, expression_a->region);
                }
                break;
            }
            case mu::llvmc::instruction_type::if_i:
            {
                if (arguments.size () == 2)
                {
					auto type (b.integer_type (1));
					arguments [1] = arguments [1]->adapt (type, *this,
						[&]
						(mu::core::region const & region_a)
						{
							return new mu::core::error_string (U"If instruction expects 1 bit integer", mu::core::error_type::if_instruction_expects_one_bit_integer, region_a);
						});
					if (module_m.global_m.error == nullptr)
                    {
						result = true;
						auto false_const (b.constant_integer (expression_a->region, &module_m.module_m->integer_1_type, 0));
						auto true_const (b.constant_integer (expression_a->region, &module_m.module_m->integer_1_type, 1));
						arguments.push_back (false_const);
						arguments.push_back (true_const);
						auto switch_i (new mu::llvmc::skeleton::switch_i (most_specific_branch, arguments, sequenced, &module_m.module_m->the_unit_type));
						auto true_branch (new mu::llvmc::skeleton::branch (most_specific_branch));
						auto false_branch (new mu::llvmc::skeleton::branch (most_specific_branch));
						auto true_element (b.switch_element (expression_a->region, true_branch, switch_i, true_const));
						auto false_element (b.switch_element (expression_a->region, false_branch, switch_i, false_const));
						expression_a->assigned = true;
						auto & values (expression_a->generated);
						values.push_back (true_element);
						values.push_back (false_element);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"If instruction expects one argument", mu::core::error_type::if_instruction_expects_one_argument, expression_a->region);
                }
                break;
            }
            case mu::llvmc::instruction_type::insertvalue:
            {
                if (arguments.size () >= 4)
                {
                    auto struct_l (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [1]));
                    if (struct_l != nullptr)
                    {
                        auto value (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [2]));
                        if (value != nullptr)
                        {                            
                            auto current_aggregate (struct_l->type ());
                            for (auto i (arguments.begin () + 3), j (arguments.end ()); i < j && module_m.global_m.error == nullptr; ++i)
                            {
                                auto type (dynamic_cast <mu::llvmc::skeleton::struct_type *> (current_aggregate));
                                if (type != nullptr)
                                {
                                    auto position (dynamic_cast <mu::llvmc::skeleton::constant_integer *> (*i));
                                    if (position != nullptr)
                                    {
                                        if (position->value_m < type->elements.size ())
                                        {
                                            current_aggregate = type->elements [position->value_m];
                                        }
                                        else
                                        {
                                            module_m.global_m.error = new mu::core::error_string (U"Aggregate has no element at this index", mu::core::error_type::index_out_of_bounds, expression_a->region);
                                        }
                                    }
                                    else
                                    {
                                        module_m.global_m.error = new mu::core::error_string (U"Value is not an aggregate at this index", mu::core::error_type::expecting_an_aggregate, expression_a->region);
                                    }
                                }
                                else
                                {
                                    module_m.global_m.error = new mu::core::error_string (U"Insertvalue requires first value to be a struct type", mu::core::error_type::expecting_value_to_be_struct, expression_a->region);
                                }
                            }
                            if (module_m.global_m.error == nullptr)
                            {
                                if (*current_aggregate == *value->type ())
                                {
									expression_a->assigned = true;
                                    expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
                                }
                                else
                                {
                                    module_m.global_m.error = new mu::core::error_string (U"Insertvalue value does not match type of aggregate", mu::core::error_type::value_does_not_match_aggregate, expression_a->region);
                                }
                            }
                        }
                        else
                        {
                            module_m.global_m.error = new mu::core::error_string (U"Insertvalue second argument must be a value", mu::core::error_type::expecting_a_value, expression_a->region);
                        }
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"Insertvalue instruction requires first argument to be a value", mu::core::error_type::expecting_a_value, expression_a->region);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"Insertvalue instruction requires three arguments", mu::core::error_type::insertvalue_expects_three_arguments, expression_a->region);
                }
                break;
            }
            case mu::llvmc::instruction_type::inttoptr:
            {
                if (arguments.size () == 3)
                {
                    auto value (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [1]));
                    if (value != nullptr)
                    {
                        auto integer (dynamic_cast <mu::llvmc::skeleton::integer_type *> (value->type ()));
                        if (integer != nullptr)
                        {
                            auto type (dynamic_cast <mu::llvmc::skeleton::pointer_type *> (arguments [2]));
                            if (type != nullptr)
                            {
								expression_a->assigned = true;
                                expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
                            }
                            else
                            {
                                module_m.global_m.error = new mu::core::error_string (U"Ptrfromint instruction requires second argument to be a pointer type", mu::core::error_type::expecting_integer_type, expression_a->region);
                            }
                        }
                        else
                        {
                            module_m.global_m.error = new mu::core::error_string (U"Ptrfromint source must be an integer", mu::core::error_type::expecting_integer_type, expression_a->region);
                        }
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"Ptrfromint instruction requires first argument to be value", mu::core::error_type::expecting_a_value, expression_a->region);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"Ptrfromint instruction requires two arguments", mu::core::error_type::ptr_to_int_expects_two, expression_a->region);
                }
                break;
            }
            case mu::llvmc::instruction_type::load:
            {
                if (arguments.size () == 2)
                {
                    auto source (dynamic_cast<mu::llvmc::skeleton::value *> (arguments [1]));
                    if (source != nullptr)
                    {
                        auto type (dynamic_cast<mu::llvmc::skeleton::pointer_type *> (source->type ()));
                        if (type != nullptr)
                        {
							expression_a->assigned = true;
                            expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
                        }
                        else
                        {
                            module_m.global_m.error = new mu::core::error_string (U"Load argument must be a pointer type", mu::core::error_type::load_argument_pointer_type, expression_a->region);
                        }
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"Load argument must be a value", mu::core::error_type::load_argument_must_be_values, expression_a->region);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"Load instruction expects two arguments", mu::core::error_type::load_expects_one_argument, expression_a->region);
                }
                break;
            }
            case mu::llvmc::instruction_type::lshr:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::mul:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::or_i:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::ptrtoint:
            {
                if (arguments.size () == 3)
                {
                    auto value (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [1]));
                    if (value != nullptr)
                    {
                        auto pointer (dynamic_cast <mu::llvmc::skeleton::pointer_type *> (value->type ()));
                        if (pointer != nullptr)
                        {
                            auto type (dynamic_cast <mu::llvmc::skeleton::integer_type *> (arguments [2]));
                            if (type != nullptr)
                            {
								expression_a->assigned = true;
                                expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
                            }
                            else
                            {
                                module_m.global_m.error = new mu::core::error_string (U"Ptrtoint instruction requires second argument to be an integer type", mu::core::error_type::expecting_integer_type, expression_a->region);
                            }
                        }
                        else
                        {
                            module_m.global_m.error = new mu::core::error_string (U"Ptrtoint source must be a pointer", mu::core::error_type::expecting_pointer_type, expression_a->region);
                        }
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"Ptrtoint instruction requires first argument to be value", mu::core::error_type::expecting_a_value, expression_a->region);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"Ptrtoint instruction requires two arguments", mu::core::error_type::ptr_to_int_expects_two, expression_a->region);
                }
                break;
            }
            case mu::llvmc::instruction_type::sdiv:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::select:
            {
				if (arguments.size () == 4)
				{
					auto predicate (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [1]));
					if (predicate != nullptr)
					{
						auto predicate_type (dynamic_cast <mu::llvmc::skeleton::integer_type *> (predicate->type ()));
						if (predicate_type != nullptr)
						{
							if (predicate_type->bits == 1)
							{
								auto left (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [2]));
								if (left != nullptr)
								{
									auto right (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [3]));
									if (right != nullptr)
									{
										if (*left->type () == *right->type ())
										{
											expression_a->assigned = true;
											expression_a->generated.push_back (b.instruction (expression_a->region, most_specific_branch, arguments, sequenced));
										}
										else
										{
											module_m.global_m.error = new mu::core::error_string (U"Select second and third arguments must be the same type", mu::core::error_type::select_arguments_same_type, expression_a->region);
										}
									}
									else
									{
										module_m.global_m.error = new mu::core::error_string (U"Select third argument must be a value", mu::core::error_type::expecting_a_value, expression_a->region);
									}
								}
								else
								{
									module_m.global_m.error = new mu::core::error_string (U"Select second argument must be a value", mu::core::error_type::expecting_a_value, expression_a->region);
								}
							}
							else
							{
								module_m.global_m.error = new mu::core::error_string (U"Select first argument must be one bit", mu::core::error_type::expecting_one_bit_integer, expression_a->region);
							}
						}
						else
						{
							module_m.global_m.error = new mu::core::error_string (U"Select first argument must be an integer", mu::core::error_type::expecting_integer_type, expression_a->region);
						}
					}
					else
					{
						module_m.global_m.error = new mu::core::error_string (U"Select first argument must be a value", mu::core::error_type::expecting_a_value, expression_a->region);
					}
				}
				else
				{
					module_m.global_m.error = new mu::core::error_string (U"Select instruction requires three arguments", mu::core::error_type::select_three_arguments, expression_a->region);
				}
                break;
            }
            case mu::llvmc::instruction_type::shl:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::srem:
            {
                process_binary_integer_instruction (expression_a,  arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::store:
            {
                if (arguments.size () == 3)
                {
                    auto source (dynamic_cast<mu::llvmc::skeleton::value *> (arguments [1]));
                    if (source != nullptr)
                    {
                        auto destination (dynamic_cast<mu::llvmc::skeleton::value *> (arguments [2]));
                        if (destination != nullptr)
                        {
                            auto destination_type (dynamic_cast<mu::llvmc::skeleton::pointer_type *> (destination->type ()));
                            if (destination_type != nullptr)
                            {
                                if (*destination_type->pointed_type == *source->type ())
                                {
                                    auto store (b.store (expression_a->region, most_specific_branch, &module_m.module_m->the_unit_type, source, destination, sequenced));
									expression_a->assigned = true;
                                    expression_a->generated.push_back (store);
                                }
                                else
                                {
                                    module_m.global_m.error = new mu::core::error_string (U"Type pointed to by store right argument does not match type of left argument", mu::core::error_type::store_right_pointed_type_doesnt_match_left, expression_a->region);
                                }
                            }
                            else
                            {
                                module_m.global_m.error = new mu::core::error_string (U"Store right argument must be a pointer type", mu::core::error_type::store_right_argument_pointer_type, expression_a->region);
                            }
                        }
                        else
                        {
                            module_m.global_m.error = new mu::core::error_string (U"Store right argument must be a value", mu::core::error_type::store_arguments_must_be_values, expression_a->region);
                        }
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"Store left argument must be a value", mu::core::error_type::store_arguments_must_be_values, expression_a->region);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"Store instruction expects two arguments", mu::core::error_type::store_expects_two_arguments, expression_a->region);
                }
                break;
            }
            case mu::llvmc::instruction_type::switch_i:
            {
                if (arguments.size () > 1)
                {
                    auto input (dynamic_cast <mu::llvmc::skeleton::value *> (arguments [1]));
                    if (input != nullptr)
                    {
                        auto type (dynamic_cast <mu::llvmc::skeleton::integer_type *> (input->type ()));
                        if (type != nullptr)
                        {
                            std::set <uint64_t> used;
                            for (auto i (arguments.begin () + 2), j (arguments.end ()); i != j && module_m.global_m.error == nullptr; ++i)
                            {
                                auto constant (dynamic_cast <mu::llvmc::skeleton::constant_integer *> (*i));
                                if (constant != nullptr)
                                {
                                    if (*constant->type () != *type)
                                    {
                                        module_m.global_m.error = new mu::core::error_string (U"Switch requires case arguments to be same type as input", mu::core::error_type::switch_requires_matching_case_types, expression_a->region);
                                    }
                                    auto existing (used.find (constant->value_m));
                                    if (existing != used.end ())
                                    {
                                        module_m.global_m.error = new mu::core::error_string (U"Switch requires case arguments to be unique", mu::core::error_type::switch_requires_unique_case, expression_a->region);
                                    }
                                    used.insert (constant->value_m);
                                }
                                else
                                {
                                    module_m.global_m.error = new mu::core::error_string (U"Switch requires case arguments to be constant integers", mu::core::error_type::switch_requires_case_constant, expression_a->region);
                                }
                            }
                            if (module_m.global_m.error == nullptr)
                            {
                                result = true;
								expression_a->assigned = true;
                                auto & values (expression_a->generated);
                                auto switch_i (new mu::llvmc::skeleton::switch_i (most_specific_branch, arguments, sequenced, &module_m.module_m->the_unit_type));
                                for (auto i (switch_i->arguments.begin () + 2), j (switch_i->arguments.end ()); i != j; ++i)
                                {
                                    auto branch (new mu::llvmc::skeleton::branch (most_specific_branch));
                                    assert (dynamic_cast <mu::llvmc::skeleton::constant_integer *> (*i) != nullptr);
                                    auto element (b.switch_element (expression_a->region, branch, switch_i, static_cast <mu::llvmc::skeleton::constant_integer *> (*i)));
                                    values.push_back (element);
                                }
                            }
                        }
                        else
                        {
                            module_m.global_m.error = new mu::core::error_string (U"Switch requires input to be an integer", mu::core::error_type::switch_requires_integer, expression_a->region);
                        }
                    }
                    else
                    {
                        module_m.global_m.error = new mu::core::error_string (U"Switch requires input to be a value", mu::core::error_type::switch_requires_value, expression_a->region);
                    }
                }
                else
                {
                    module_m.global_m.error = new mu::core::error_string (U"Switch requires an input argument", mu::core::error_type::switch_requires_input, expression_a->region);
                }
                break;
            }
            case mu::llvmc::instruction_type::sub:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::typeof_i:
            {
                assert (false);
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::udiv:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::urem:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            case mu::llvmc::instruction_type::xor_i:
            {
                process_binary_integer_instruction (expression_a, arguments, sequenced, most_specific_branch);
                break;
            }
            default:
                assert (false);
                break;
        }
    }
	assert ((module_m.global_m.error != nullptr) xor (expression_a->assigned));
}

mu::llvmc::function_processor::function_processor (mu::llvmc::module_processor & module_a, mu::llvmc::ast::function * node_a) :
module_m (module_a),
function_m (b.function (node_a->region)),
node_m (node_a),
previous (module_a.global_m.current_context),
previous_origin (module_a.global_m.current_origin)
{
	module_a.global_m.current_context = this;
    module_a.global_m.current_origin = function_m->entry;
}

mu::llvmc::function_processor::~function_processor ()
{
	assert (module_m.global_m.current_context == this);
	module_m.global_m.current_context = previous;
    assert (module_m.global_m.current_origin == function_m->entry);
    module_m.global_m.current_origin = previous_origin;
}

void mu::llvmc::module_processor::set (mu::llvmc::ast::set * node_a)
{
	for (auto i: node_a->nodes)
	{
		global_m.process_node (i);
		node_a->generated.insert (node_a->generated.end (), i->generated.begin (), i->generated.end ());
	}
	if (global_m.error == nullptr)
	{
		node_a->assigned = true;
	}
}

class namespace_extractor : public mu::llvmc::skeleton::namespace_visitor
{
public:
    namespace_extractor (mu::core::error * & error_a, mu::llvmc::ast::namespace_c * node_a) :
    error (error_a),
    node (node_a)
    {
        assert (node_a->node_m->assigned);
    }
    mu::core::error * & error;
    mu::llvmc::ast::namespace_c * node;
    void named (mu::llvmc::skeleton::namespace_container * namespace_a) override
    {
        auto element ((*namespace_a) [node->member]);
        if (element != nullptr)
        {
            node->assigned = true;
            node->generated.push_back (element);
        }
        else
        {
            error = new mu::core::error_string (U"Namespace container does not have member with stated name", mu::core::error_type::not_a_member, node->region);
        }
    }
    void unnamed () override
    {
        error = new mu::core::error_string (U"Node is not a namespace container", mu::core::error_type::not_a_namespace_container);
    }
};

void mu::llvmc::function_processor::namespace_c (mu::llvmc::ast::namespace_c * node_a)
{
    module_m.global_m.process_node (node_a->node_m);
    if (module_m.global_m.error == nullptr)
    {
        assert (node_a->node_m->assigned);
        if (node_a->node_m->generated.size () == 1)
        {
            namespace_extractor extractor (module_m.global_m.error, node_a);
            node_a->node_m->generated [0]->named (&extractor);
        }
        else
        {
            module_m.global_m.error = new mu::core::error_string (U"Can only extract name from one node", mu::core::error_type::can_only_extract_from_one);
        }
    }
}

void mu::llvmc::global_processor::template_parameter (mu::llvmc::ast::template_parameter * node_a)
{
	assert (false && "Template parameter was not replaced");
}

void mu::llvmc::module_processor::function_overload (mu::llvmc::ast::function_overload * overload_a)
{
	global_m.process_node (overload_a->family);
	if (global_m.error == nullptr)
	{
		assert (overload_a->family->assigned);
	}
}

void mu::llvmc::module_processor::function_family (mu::llvmc::ast::function_family * family_a)
{
	family_a->assigned = true;
	family_a->generated.push_back (new mu::llvmc::skeleton::function_family);
}