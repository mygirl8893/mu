#include <mu/llvmc/parser.hpp>

#include <mu/core/error_string.hpp>
#include <mu/core/stream.hpp>
#include <mu/core/tokens.hpp>
#include <mu/llvmc/ast.hpp>
#include <mu/core/stream_token.hpp>
#include <mu/llvmc/skeleton.hpp>
#include <mu/llvmc/parser_t.hpp>

#include <llvm/IR/Type.h>

#include <sstream>

mu::llvmc::global::global (mu::llvmc::keywords * keywords_a) :
keywords (keywords_a)
{    
}

mu::llvmc::parser::parser (mu::io::stream_token & stream_a):
current_template (nullptr),
builtins (&keywords),
current_mapping (&builtins),
stream (stream_a),
function_overload_hook (function)
{
    bool error (false);
    error = builtins.insert  (U"~", new mu::llvmc::ast::value (new mu::llvmc::skeleton::identity, current_template));
    assert (!error);
	auto constant_int (new mu::llvmc::ast::constant_int);
    error = builtins.insert (U"int-c", constant_int);
    assert (!error);
    error = keywords.insert (U"`", &namespace_hook);
    assert (!error);
    error = keywords.insert (U"#", &number);
    assert (!error);
    error = keywords.insert (U"!", &sequence_hook);
    assert (!error);
    error = keywords.insert (U"farray", &array_type);
    assert (!error);
    error = keywords.insert (U"ascii", &ascii_hook);
    assert (!error);
    error = keywords.insert (U"asm", &asm_hook);
    assert (!error);
    error = keywords.insert (U"carray", &constant_array);
    assert (!error);
    error = keywords.insert (U"entrypoint", &entry_hook);
    assert (!error);
    error = keywords.insert (U"global", &global_variable);
    assert (!error);
    error = keywords.insert (U"function", &function);
    assert (!error);
	error = keywords.insert (U"ofunction", &function_overload_hook);
	assert (!error);
    error = keywords.insert (U"int-t", &int_type);
    assert (!error);
    error = keywords.insert (U"join", &join_hook);
    assert (!error);
    error = keywords.insert (U"loop", &loop_hook);
    assert (!error);
    error = keywords.insert (U"let", &let_hook);
    assert (!error);
    error = keywords.insert (U"module", &module_hook);
    assert (!error);
    error = keywords.insert (U"null", &constant_pointer_null);
    assert (!error);
    error = keywords.insert (U"ptr", &ptr_type);
    assert (!error);
    error = keywords.insert (U"set", &set_hook);
    assert (!error);
    error = keywords.insert (U"utf32", &string_hook);
    assert (!error);
    error = keywords.insert (U"struct", &struct_hook);
    assert (!error);
    error = keywords.insert (U"template", &template_hook);
    assert (!error);
    error = keywords.insert (U"undefined", &undefined_hook);
    assert (!error);
	auto bit_size (new mu::llvmc::ast::number (U"1"));
	auto bit_type (new mu::llvmc::ast::integer_type);
	bit_type->bits = bit_size;
    error = builtins.insert  (U"false", new mu::llvmc::ast::expression ({constant_int, bit_type, new mu::llvmc::ast::number (U"0")}, {}));
    assert (!error);
    error = builtins.insert  (U"true", new mu::llvmc::ast::expression ({constant_int, bit_type, new mu::llvmc::ast::number (U"1")}, {}));
    assert (!error);
    error = builtins.insert (U"unit_v", new mu::llvmc::ast::unit (current_template));
    assert (!error);
    error = builtins.insert (U"add", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::add), current_template));
    assert (!error);
    error = builtins.insert (U"alloca", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::alloca), current_template));
    assert (!error);
    error = builtins.insert (U"and", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::and_i), current_template));
    assert (!error);
    error = builtins.insert (U"ashr", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::ashr), current_template));
    assert (!error);
    error = builtins.insert (U"atomicrmw", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::atomicrmw), current_template));
    assert (!error);
    error = builtins.insert (U"bitcast", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::bitcast), current_template));
    assert (!error);
    error = builtins.insert (U"call", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::call), current_template));
    assert (!error);
    error = builtins.insert (U"cmpxchg", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::cmpxchg), current_template));
    assert (!error);
    error = builtins.insert (U"extractelement", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::extractelement), current_template));
    assert (!error);
    error = builtins.insert (U"extractvalue", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::extractvalue), current_template));
    assert (!error);
    error = builtins.insert (U"fadd", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fadd), current_template));
    assert (!error);
    error = builtins.insert (U"fcmp", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fcmp), current_template));
    assert (!error);
    error = builtins.insert (U"fdiv", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fdiv), current_template));
    assert (!error);
    error = builtins.insert (U"fence", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fence), current_template));
    assert (!error);
    error = builtins.insert (U"fmul", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fmul), current_template));
    assert (!error);
    error = builtins.insert (U"fpext", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fpext), current_template));
    assert (!error);
    error = builtins.insert (U"fptoi", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fptoi), current_template));
    assert (!error);
    error = builtins.insert (U"fptosi", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fptosi), current_template));
    assert (!error);
    error = builtins.insert (U"fptoui", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fptoui), current_template));
    assert (!error);
    error = builtins.insert (U"fptrunc", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fptrunc), current_template));
    assert (!error);
    error = builtins.insert (U"frem", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::frem), current_template));
    assert (!error);
    error = builtins.insert (U"fsub", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::fsub), current_template));
    assert (!error);
    error = builtins.insert (U"getelementptr", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::getelementptr), current_template));
    assert (!error);
    error = builtins.insert (U"icmp", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::icmp), current_template));
    assert (!error);
    error = builtins.insert (U"ieq", new mu::llvmc::ast::value (new mu::llvmc::skeleton::predicate (mu::llvmc::predicates::icmp_eq), current_template));
    assert (!error);
    error = builtins.insert (U"ine", new mu::llvmc::ast::value (new mu::llvmc::skeleton::predicate (mu::llvmc::predicates::icmp_ne), current_template));
    assert (!error);
    error = builtins.insert (U"iugt", new mu::llvmc::ast::value (new mu::llvmc::skeleton::predicate (mu::llvmc::predicates::icmp_ugt), current_template));
    assert (!error);
    error = builtins.insert (U"iuge", new mu::llvmc::ast::value (new mu::llvmc::skeleton::predicate (mu::llvmc::predicates::icmp_uge), current_template));
    assert (!error);
    error = builtins.insert (U"iult", new mu::llvmc::ast::value (new mu::llvmc::skeleton::predicate (mu::llvmc::predicates::icmp_ult), current_template));
    assert (!error);
    error = builtins.insert (U"iule", new mu::llvmc::ast::value (new mu::llvmc::skeleton::predicate (mu::llvmc::predicates::icmp_ule), current_template));
    assert (!error);
    error = builtins.insert (U"isgt", new mu::llvmc::ast::value (new mu::llvmc::skeleton::predicate (mu::llvmc::predicates::icmp_sgt), current_template));
    assert (!error);
    error = builtins.insert (U"isge", new mu::llvmc::ast::value (new mu::llvmc::skeleton::predicate (mu::llvmc::predicates::icmp_sge), current_template));
    assert (!error);
    error = builtins.insert (U"islt", new mu::llvmc::ast::value (new mu::llvmc::skeleton::predicate (mu::llvmc::predicates::icmp_slt), current_template));
    assert (!error);
    error = builtins.insert (U"isle", new mu::llvmc::ast::value (new mu::llvmc::skeleton::predicate (mu::llvmc::predicates::icmp_sle), current_template));
    assert (!error);
    error = builtins.insert (U"if", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::if_i), current_template));
    assert (!error);
    error = builtins.insert (U"insertelement", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::insertelement), current_template));
    assert (!error);
    error = builtins.insert (U"insertvalue", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::insertvalue), current_template));
    assert (!error);
    error = builtins.insert (U"load", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::load), current_template));
    assert (!error);
    error = builtins.insert (U"lshr", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::lshr), current_template));
    assert (!error);
    error = builtins.insert (U"mul", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::mul), current_template));
    assert (!error);
    error = builtins.insert (U"or", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::or_i), current_template));
    assert (!error);
    error = builtins.insert (U"ptrfromint", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::inttoptr), current_template));
    assert (!error);
    error = builtins.insert (U"ptrtoint", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::ptrtoint), current_template));
    assert (!error);
    error = builtins.insert (U"sdiv", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::sdiv), current_template));
    assert (!error);
    error = builtins.insert (U"select", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::select), current_template));
    assert (!error);
    error = builtins.insert (U"sext", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::sext), current_template));
    assert (!error);
    error = builtins.insert (U"shl", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::shl), current_template));
    assert (!error);
    error = builtins.insert (U"shufflevector", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::shufflevector), current_template));
    assert (!error);
    error = builtins.insert (U"sitofp", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::sitofp), current_template));
    assert (!error);
    error = builtins.insert (U"srem", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::srem), current_template));
    assert (!error);
    error = builtins.insert (U"store", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::store), current_template));
    assert (!error);
    error = builtins.insert (U"sub", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::sub), current_template));
    assert (!error);
    error = builtins.insert (U"switch", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::switch_i), current_template));
    assert (!error);
    error = builtins.insert (U"trunc", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::trunc), current_template));
    assert (!error);
    error = builtins.insert (U"typeof", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::typeof_i), current_template));
    assert (!error);
    error = builtins.insert (U"udiv", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::udiv), current_template));
    assert (!error);
    error = builtins.insert (U"uitofp", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::uitofp), current_template));
    assert (!error);
    error = builtins.insert (U"urem", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::urem), current_template));
    assert (!error);
    error = builtins.insert (U"unit", new mu::llvmc::ast::value (new mu::llvmc::skeleton::unit_type, current_template));
    assert (!error);
    error = builtins.insert (U"xor", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::xor_i), current_template));
    assert (!error);
    error = builtins.insert (U"zext", new mu::llvmc::ast::value (new mu::llvmc::skeleton::marker (mu::llvmc::instruction_type::zext), current_template));
    assert (!error);
}

mu::llvmc::node_result mu::llvmc::module_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
	mu::llvmc::module module (region_a, parser_a);
	module.parse ();
	return module.result;
}

void mu::llvmc::module::parse ()
{
	result.error = parser.parse_left_square_required (U"Module expecting left square", mu::core::error_type::expecting_left_square);
	if (result.error == nullptr)
	{
		parse_internal ();
		if (result.error == nullptr)
		{/*
			auto next (parser_a.peek ());
			assert (next.ast == nullptr);
			assert (next.error == nullptr);
			auto token (next.token);
			switch (token->id ())
			{
				case mu::io::token_id::right_square:
					break;
				default:
					result.error = new mu::core::error_string (U"Expecting right square after module", mu::core::error_type::expecting_right_square, token->region);
					break;
			}*/
		}
	}
}

void mu::llvmc::module::parse_internal ()
{
    auto module (new mu::llvmc::ast::module (parser.current_template));
	module->region.first = mu::core::position (0, 1, 1);
    while ((result.node == nullptr) and (result.error == nullptr))
    {
        auto item (parser.peek ());
        if (item.ast != nullptr)
        {
            module->globals.push_back (item.ast);
        }
        else if (item.token != nullptr)
        {
            auto id (item.token->id ());
            switch (id)
            {
                case mu::io::token_id::end:
				case mu::io::token_id::right_square:
					module->region.last = item.token->region.last;
                    result.node = module;
                    break;
                default:
                    result.error = new mu::core::error_string (U"Expecting function or end of stream", mu::core::error_type::expecting_function_or_end_of_stream, item.token->region);
                    break;
            }
        }
        else
        {
            result.error = item.error;
        }
    }
    if (result.error == nullptr)
    {
		for (auto i: block.mappings)
		{
			if (i.second != nullptr)
			{
				module->names [i.first] = i.second;
			}
			else
			{
				// Name reserved by sub-blocks
			}
		}
        if (!block.unresolved.empty ())
        {
			std::stringstream error;
			error << "Unresoled symbols:";
			for (auto i: block.unresolved)
			{
				error << " " << std::string (i.first.begin (), i.first.end ());
			}
			std::string err (error.str ());
            result.error = new mu::core::error_string (mu::string (err.begin (), err.end ()).c_str (), mu::core::error_type::unresolved_symbols, std::get <0> (block.unresolved.begin ()->second));
            result.node = nullptr;
        }
    }
}

mu::llvmc::node_result mu::llvmc::function_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::function parser_l (region_a, parser_a);
    parser_l.parse ();
    return parser_l.result;
}

mu::llvmc::function::function (mu::core::region const & region_a, mu::llvmc::parser & parser_a):
block (parser_a.current_mapping),
result ({nullptr, nullptr}),
function_m (new mu::llvmc::ast::function (parser_a.current_template)),
parser (parser_a),
first (region_a)
{
	parser_a.current_mapping = &block;
}

mu::llvmc::function::~function ()
{
	assert (parser.current_mapping == &block);
    parser.current_mapping = block.parent;
}

void mu::llvmc::function::parse ()
{
	function_m->region.first = first.first;
    parse_parameters ();
    if (result.error == nullptr)
    {
        parse_body ();
        if (result.error == nullptr)
        {
            parse_results ();
            if (result.error == nullptr)
            {
                result.node = function_m;
            }
        }
    }
}

void mu::llvmc::function::parse_parameters ()
{
    auto next (parser.stream [0]);
    switch (next->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.consume (1);
            auto done (false);
            while (result.error == nullptr && !done)
            {
                parse_parameter (done);
            }
            break;
        }
        default:
            result.error = new mu::core::error_string (U"While parsing parameters, expecting left square", mu::core::error_type::parsing_parameters_expecting_left_square);
            break;
    }
}

void mu::llvmc::function::parse_parameter (bool & done_a)
{
    auto argument (new mu::llvmc::ast::parameter (parser.current_template));
	result.error = parser.parse_ast_or_refer_or_right_square (
		[argument]
		(mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
		{
			argument->type = node_a;
            argument->region.first = region_a.first;
		},
		[&]
		(mu::io::right_square * right_square_a)
		{
			done_a = true;
		},
		U"Expecting type or right square", mu::core::error_type::expecting_type_or_right_square);
	if (result.error == nullptr && !done_a)
	{
		function_m->parameters.push_back (argument);
        result.error = parser.parse_identifier (
            [&]
            (mu::io::identifier * identifier_a)
            {
				mu::core::error * result (nullptr);
				argument->name = identifier_a->string;
                argument->region.last = identifier_a->region.last;
                if (block.insert (identifier_a->string, argument))
                {
                    result = new mu::core::error_string (U"Unable to use identifier", mu::core::error_type::unable_to_use_identifier, identifier_a->region);
                }
				return result;
            }, U"While parsing parameters, expecting an identifier", mu::core::error_type::parsing_parameters_expecting_identifier);
	}
}

void mu::llvmc::function::parse_body ()
{
    result.error = parser.parse_left_square_required (U"Expecting left square", mu::core::error_type::expecting_left_square);
    if (result.error == nullptr)
    {
        auto done (false);
        while (!done && result.error == nullptr)
        {
            switch (parser.stream [0]->id ())
            {
                case mu::io::token_id::right_square:
                    parser.stream.consume (1);
                    done = true;
                    break;
                default:
                {
                    auto next (parser.peek ());
                    if (next.ast != nullptr)
                    {
                        function_m->roots.push_back (next.ast);
                    }
                    else if (next.token != nullptr)
                    {
                        result.error = new mu::core::error_string (U"Expecting expression", mu::core::error_type::expecting_expression, next.token->region);
                    }
                    else
                    {
                        result.error = next.error;
                    }
                    break;
                }
            }
        }
    }
}

void mu::llvmc::function::parse_results ()
{
    result.error = parser.parse_left_square_required (U"Expecting left square", mu::core::error_type::expecting_left_square);
    if (result.error == nullptr)
    {
        auto next (parser.stream [0]);
        auto done (false);
        while (result.error == nullptr && !done)
        {
            switch (next->id ())
            {
                case mu::io::token_id::left_square:
                    parser.stream.consume (1);
                    parse_result_set ();
                    next = parser.stream [0];
                    break;
                case mu::io::token_id::right_square:
                    parser.stream.consume (1);
                    function_m->region = mu::core::region (first.first, next->region.last);
                    done = true;
                    break;
                default:
                    result.error = new mu::core::error_string (U"Expecting left square or right square", mu::core::error_type::expecting_left_square_or_right_square, next->region);
                    break;
            }
        }
    }
}

void mu::llvmc::function::parse_result_set ()
{
    auto done (false);
    auto predicates (false);
	auto function_l (function_m);
	auto branch_position (function_m->results.branches.size ());
	assert (function_m->returns.size () == branch_position);
    auto & value_branch (function_m->results.add_branch ());
	function_m->returns.push_back (decltype (function_m->returns)::value_type ());
	auto & type_branch (function_m->returns.back ());
    while (result.error == nullptr && !done && !predicates)
    {
		assert (value_branch.nodes.size () == type_branch.types.size ());
		auto value_position (value_branch.nodes.size ());
		value_branch.nodes.push_back (nullptr);
		type_branch.types.push_back (nullptr);
		result.error = parser.parse_ast_or_refer_or_right_square_or_terminator (
			[function_l, branch_position, value_position]
			(mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
			{
				function_l->returns [branch_position].types [value_position] = node_a;
			},
			[&]
			(mu::core::region const & region_a)
			{
				done = true;
				value_branch.nodes.pop_back ();
				type_branch.types.pop_back ();
			},
			[&]
			(mu::core::region const & region_a)
			{
				predicates = true;
				value_branch.nodes.pop_back ();
				type_branch.types.pop_back ();
			}, U"Expecting a type or right square or terminator", mu::core::error_type::expecting_type_or_right_square_or_terminator);
		if (!result.error && !done && !predicates)
		{
			result.error = parser.parse_ast_or_refer (
				[function_l, branch_position, value_position]
				(mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
				{
					function_l->results.branches [branch_position].nodes [value_position] = node_a;
				});
		}
    }
    while (result.error == nullptr && !done)
    {
        auto sequence (new mu::llvmc::ast::sequence (nullptr));
        value_branch.nodes.push_back (sequence);
        parser.parse_ast_or_refer_or_right_square (
            [sequence]
            (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
            {
               sequence->node_m = node_a;
            },
            [&]
            (mu::io::right_square *)
            {
                value_branch.nodes.pop_back ();
                done = true;
            } , U"Parsing predicates, expecting ast or reference", mu::core::error_type::expecting_ast_or_reference);
    }
}

mu::llvmc::node_result mu::llvmc::parser::parse ()
{
	mu::llvmc::module module (mu::empty_region, *this);
	module.parse_internal ();
    auto result (module.result);
	if (result.error == nullptr)
	{
		auto next (peek ());
		assert (next.ast == nullptr);
		assert (next.error == nullptr);
		auto token (next.token);
		switch (token->id ())
		{
			case mu::io::token_id::end:
				consume ();
				break;
			default:
				result.error = new mu::core::error_string (U"Expecting right square after module", mu::core::error_type::expecting_right_square, token->region);
				break;
		}
	}
    return result;
}

bool mu::llvmc::keywords::insert (mu::string const & identifier_a, mu::llvmc::hook * hook_a)
{
    auto existing (mappings.find (identifier_a));
    auto result (existing != mappings.end ());
    if (!result)
    {
        mappings.insert (existing, decltype (mappings)::value_type (identifier_a, hook_a));
    }
    return result;
}

mu::llvmc::hook_result mu::llvmc::keywords::get_hook (mu::string const & identifier_a)
{
    hook_result result ({nullptr});
    auto existing (mappings.find (identifier_a));
	if (existing != mappings.end ())
	{
		result.hook = existing->second;
	}
    return result;
}

bool mu::llvmc::global::reserve (mu::string const & name_a)
{
    auto existing (mappings.find (name_a));
    auto result (existing != mappings.end ());
    if (result)
    {
        auto hook (keywords->get_hook (name_a));
        result = hook.hook != nullptr;
    }
    return result;
}

bool mu::llvmc::global::get (mu::string const & name_a, mu::core::region const & region_a, action_type action_a)
{
    auto existing (mappings.find (name_a));
    auto result (existing == mappings.end ());
    if (!result)
    {
        action_a (existing->second, region_a);
    }
    return result;
}

void mu::llvmc::global::refer (mu::string const & name_a, mu::core::region const & region_a, action_type action_a)
{
    auto error (get (name_a, region_a, action_a));
    if (error)
    {
        unresolved.insert (decltype (unresolved)::value_type (name_a, unresolved_type (region_a, action_a)));
    }
}

mu::llvmc::block::block (mu::llvmc::mapping * parent_a):
parent (parent_a)
{
}

bool mu::llvmc::block::insert (mu::string const & name_a, mu::llvmc::ast::node * node_a)
{
	assert (node_a != nullptr);
    auto result (parent->reserve (name_a));
    if (!result)
    {
        auto existing (mappings.find (name_a));
        result = existing != mappings.end ();
        if (!result)
        {
            mappings.insert (decltype (mappings)::value_type (name_a, node_a));
            for (auto i (unresolved.find (name_a)), j (unresolved.end ()); i != j && i->first == name_a; ++i)
            {
                std::get <1> (i->second) (node_a, std::get <0> (i->second));
            }
            unresolved.erase (name_a);
        }
    }
    return result;
}

bool mu::llvmc::block::reserve (mu::string const & name_a)
{
    auto result (parent->reserve (name_a));
    if (!result)
    {
        auto existing (mappings.find (name_a));
        result = existing != mappings.end () && existing->second != nullptr;
        mappings [name_a] = nullptr;
    }
    return result;
}

bool mu::llvmc::block::get (mu::string const & name_a, mu::core::region const & region_a, action_type action_a)
{
    auto existing (mappings.find (name_a));
    auto result (existing == mappings.end ());
    if (result)
    {
        result = parent->get (name_a, region_a, action_a);
    }
    else
    {
        if (existing->second != nullptr)
        {
            action_a (existing->second, region_a);
        }
        else
        {
			result = true;
        }
    }
    return result;
}

void mu::llvmc::block::refer (mu::string const & name_a, mu::core::region const & region_a, action_type action_a)
{
    auto existing (mappings.find (name_a));
    auto result (existing == mappings.end ());
    if (result)
    {
        result = parent->get (name_a, region_a, action_a);
        if (result)
        {
            unresolved.insert (decltype (unresolved)::value_type (name_a, unresolved_type (region_a, action_a)));
        }
    }
    else
    {
        if (existing->second != nullptr)
        {
            action_a (existing->second, region_a);
        }
        else
        {
            unresolved.insert (decltype (unresolved)::value_type (name_a, unresolved_type (region_a, action_a)));
        }
    }
}

void mu::llvmc::block::accept (mu::unordered_multimap <mu::string, unresolved_type> const & unresolved_a)
{
    unresolved.insert (unresolved_a.begin (), unresolved_a.end ());
}

void mu::llvmc::global::accept (mu::unordered_multimap <mu::string, unresolved_type> const & unresolved_a)
{
    unresolved.insert (unresolved_a.begin (), unresolved_a.end ());
}

mu::llvmc::node_result mu::llvmc::int_type::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
	auto type (new mu::llvmc::ast::integer_type (parser_a.current_template));
	result.node = type;
	type->region.first = region_a.first;
	result.error = parser_a.parse_ast_or_refer (
		[type] (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
		{
			type->bits = node_a;
			type->region.last = region_a.last;
		});
    return result;
}

mu::llvmc::expression::expression (mu::core::region const & region_a, mu::llvmc::parser & parser_a):
result ({nullptr, nullptr}),
parser (parser_a),
region (region_a)
{
}

void mu::llvmc::expression::parse ()
{
    auto expression_l (new mu::llvmc::ast::expression (parser.current_template));
    auto done (false);
    auto predicates (false);
    while (!done && result.error == nullptr)
    {
        auto next (parser.peek ());
        if (next.ast != nullptr)
        {
            expression_l->arguments.push_back (next.ast);
        }
        else if (next.token != nullptr)
        {
            switch (next.token->id ())
            {
                case mu::io::token_id::identifier:
                {
                    auto position (expression_l->arguments.size ());
                    expression_l->arguments.push_back (nullptr);
                    auto identifier (static_cast <mu::io::identifier *> (next.token));
                    parser.current_mapping->refer (identifier->string, identifier->region,
                        [expression_l, position]
                        (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
                        {
                            expression_l->arguments [position] = node_a;
                        });
                    break;
                }
                case mu::io::token_id::right_square:
                    expression_l->region = mu::core::region (region.first, next.token->region.last);
                    done = true;
                    break;
                default:
                    result.error = new mu::core::error_string (U"Expecting argument or right_square", mu::core::error_type::expecting_argument_or_right_square, next.token->region);
                    break;
            }
        }
        else
        {
            result.error = next.error;
        }
    }
    if (result.error == nullptr)
    {
        result.node = expression_l;
    }
}

mu::llvmc::node_result mu::llvmc::set_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto name (parser_a.peek ());
    if (name.token != nullptr)
    {
        auto name_id (name.token->id ());
        switch (name_id)
        {
            case mu::io::token_id::identifier:
            {
                auto next (parser_a.peek ());
                if (next.ast != nullptr)
                {
                    auto error (parser_a.current_mapping->insert(static_cast <mu::io::identifier *> (name.token)->string, next.ast));
                    if (error)
                    {
                        result.error = new mu::core::error_string (U"Unable to use name", mu::core::error_type::unable_to_use_identifier);
                    }
                    else
                    {
                        result.node = next.ast;
                    }
                }
                else if (next.ast != nullptr)
                {
                    result.error = new mu::core::error_string (U"Expecting an expression", mu::core::error_type::expecting_expression);
                }
                else
                {
                    result.error = next.error;
                }
                break;
            }
            default:
                result.error = new mu::core::error_string (U"Expecting identifier", mu::core::error_type::expecting_identifier);
                break;
        }
    }
    else if (name.ast != nullptr)
    {
        result.error = new mu::core::error_string (U"Expecting a name", mu::core::error_type::expecting_name);
    }
    else
    {
        result.error = name.error;
    }
    return result;
}

mu::llvmc::node_result mu::llvmc::let_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto done (false);
    auto next (parser_a.stream [0]);
    mu::vector <mu::io::identifier *> identifiers;
    while (!done && result.error == nullptr)
    {
        switch (next->id ())
        {
            case mu::io::token_id::identifier:
            {
                auto identifier (static_cast <mu::io::identifier *> (next));
                auto hook (parser_a.keywords.get_hook (identifier->string));
                if (hook.hook != nullptr)
                {
                    done = true;
                }
                else
                {
                    identifiers.push_back (identifier);
                    parser_a.stream.consume (1);
                    next = parser_a.stream [0];
                }
                break;
            }
            case mu::io::token_id::left_square:
                done = true;
                break;
            default:
                result.error = new mu::core::error_string (U"Expecting identifier or left square", mu::core::error_type::expecting_identifier_or_left_square);
                break;
        }
    }
	if (result.error == nullptr)
	{
		auto expression (parser_a.peek ());
		if (expression.ast != nullptr)
		{
			size_t index (0);
			size_t total (identifiers.size ());
			auto set (new mu::llvmc::ast::set (parser_a.current_template));
			set->region.first = identifiers.empty () ? expression.ast->region.first : identifiers [0]->region.first;
			set->region.last = expression.ast->region.last;
			for (auto i (identifiers.begin ()), j (identifiers.end ()); i != j && result.error == nullptr; ++i, ++index)
			{
				auto element (new mu::llvmc::ast::element (expression.ast, index, total, (*i)->string, (*i)->region, parser_a.current_template));
				set->nodes.push_back (element);
				auto error (parser_a.current_mapping->insert ((*i)->string, element));
				if (error)
				{
					result.error = new mu::core::error_string (U"Unable to use identifier", mu::core::error_type::unable_to_use_identifier, (*i)->region);
				}
			}
			if (result.error == nullptr)
			{
				result.node = set;
			}
		}
		else
		{
			assert (expression.token == nullptr);
			result.error = expression.error;
		}
	}
    return result;
}

bool mu::llvmc::global::insert (mu::string const & identifier_a, mu::llvmc::ast::node * node_a)
{
    auto hook (keywords->get_hook (identifier_a));
    auto result (hook.hook != nullptr);
    if (!result)
    {
        auto existing (mappings.find (identifier_a));
        result = existing != mappings.end ();
        if (!result)
        {
            mappings.insert (decltype (mappings)::value_type (identifier_a, node_a));
            for (auto i (unresolved.find (identifier_a)), j (unresolved.end ()); i != j && i->first == identifier_a; ++i)
            {
                std::get <1> (i->second) (node_a, std::get <0> (i->second));
            }
            unresolved.erase (identifier_a);
        }
    }
    return result;
}

mu::llvmc::loop::loop (mu::llvmc::parser & parser_a):
loop_m (new mu::llvmc::ast::loop (parser_a.current_template)),
result ({nullptr, nullptr}),
parser (parser_a)
{
}

void mu::llvmc::loop::parse ()
{
    parse_arguments ();
    if (result.error == nullptr)
    {
        parse_binds ();
        if (result.error == nullptr)
        {
            parse_body ();
            if (result.error == nullptr)
            {
                parse_results ();
                if (result.error == nullptr)
                {
                    result.node = loop_m;
                }
            }
        }
    }
}

void mu::llvmc::loop::parse_arguments ()
{
    switch (parser.stream [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.consume (1);
            auto predicates (false);
            auto done (false);
            while (!done)
            {
                auto next (parser.peek ());
                if (next.ast != nullptr)
                {
                    loop_m->arguments.push_back (next.ast);
                }
                else if (next.token != nullptr)
                {
                    switch (next.token->id ())
                    {
                        case mu::io::token_id::right_square:
                        {
                            done = true;
                            break;
                        }
                        case mu::io::token_id::identifier:
                        {
                            auto & arguments_l (loop_m->arguments);
                            auto position (arguments_l.size ());
                            arguments_l.push_back (nullptr);
                            auto identifier (mu::cast <mu::io::identifier> (next.token));
                            parser.current_mapping->refer (identifier->string, identifier->region,
                                  [&arguments_l, position]
                                   (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
                                  {
                                      arguments_l [position] = node_a;
                                  });
                            break;
                        }
                        default:
                            done = true;
                            result.error = new mu::core::error_string (U"Expecting argument or right square", mu::core::error_type::expecting_argument_or_right_square);
                            break;
                    }
                }
                else
                {
                    result.error = next.error;
                    done = true;
                }
            }
        }
            break;
        default:
            result.error = new mu::core::error_string (U"Expecting left square", mu::core::error_type::expecting_left_square);
            break;
    }
}

void mu::llvmc::loop::parse_binds ()
{
    switch (parser.stream [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.consume (1);
            auto done (false);
            while (!done)
            {
                auto next (parser.peek ());
                if (next.ast != nullptr)
                {
                    done = true;
                    result.error = new mu::core::error_string (U"Expecting identifier", mu::core::error_type::expecting_identifier);
                }
                else if (next.token != nullptr)
                {
                    switch (next.token->id ())
                    {
                        case mu::io::token_id::identifier:
                        {
							auto identifier (mu::cast <mu::io::identifier> (next.token));
                            auto parameter (new mu::llvmc::ast::loop_parameter (identifier->string, parser.current_template));
                            loop_m->parameters.push_back (parameter);
                            auto error (parser.current_mapping->insert (identifier->string, parameter));
                            if (error)
                            {
                                done = true;
                                result.error = new mu::core::error_string (U"Unable to use identifier", mu::core::error_type::unable_to_use_identifier);
                            }
                            break;
                        }
                        case mu::io::token_id::right_square:
                            done = true;
                            break;
                        default:
                            done = true;
                            result.error = new mu::core::error_string (U"Expecting identifier", mu::core::error_type::expecting_identifier);
                            break;
                    }
                }
                else
                {
                    done = true;
                    result.error = next.error;
                }
            }
            break;
        }
        default:
            result.error = new mu::core::error_string (U"Expecting left square", mu::core::error_type::expecting_left_square);
            break;
    }
}

void mu::llvmc::loop::parse_body ()
{
    switch (parser.stream [0]->id ())
    {
        case mu::io::token_id::left_square:
        {
            parser.stream.consume (1);
            auto done (false);
            while (!done)
            {
                auto next (parser.peek ());
                if (next.ast != nullptr)
                {
                    loop_m->roots.push_back (next.ast);
                }
                else if (next.token != nullptr)
                {
                    switch (next.token->id ())
                    {
                        case mu::io::token_id::right_square:
                            done = true;
                            break;
                        default:
                            done = true;
                            result.error = new mu::core::error_string (U"Expecting expression or right square", mu::core::error_type::expecting_expression_or_right_square);
                            break;
                    }
                }
                else
                {
                    result.error = next.error;
                    done = true;
                }
            }
            break;
        }
        default:
            result.error = new mu::core::error_string (U"Expecting loop body", mu::core::error_type::expecting_loop_body);
            break;
    }
}

void mu::llvmc::loop::parse_results ()
{
    result.error = parser.parse_left_square_required (U"Expecting loop results", mu::core::error_type::expecting_loop_results);
    if (result.error == nullptr)
    {
        auto done (false);
        while (!done && result.error == nullptr)
        {
            switch (parser.stream [0]->id ())
            {
                case mu::io::token_id::left_square:
                {
                    auto branch_index (loop_m->results.size ());
                    auto & branch (loop_m->add_branch ());
                    parser.stream.consume (1);
                    auto set_done (false);
                    while (!set_done && result.error == nullptr)
                    {
						auto position (branch.nodes.size ());
						branch.nodes.push_back (nullptr);
						result.error = parser.parse_ast_or_refer_or_right_square (
							[=]
							(mu::llvmc::ast::node * node_a, mu::core::region const & region_a) 
							{
								loop_m->results [branch_index].nodes [position] = node_a;
							}, 
							[&] 
                            (mu::io::right_square * token_a)
							{
                                branch.nodes.pop_back ();
								set_done = true;
							}, 
						U"Expecting result", mu::core::error_type::expecting_an_expression);
                    }
                    break;
                }
                case mu::io::token_id::right_square:
                    parser.consume ();
                    done = true;
                    break;
                default:
                    result.error = new mu::core::error_string (U"Expecting result set or right square", mu::core::error_type::expecting_result_set_or_right_square);
                    break;
            }
        }
    }
}

mu::llvmc::node_result mu::llvmc::loop_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::loop loop (parser_a);
    loop.parse ();
    return loop.result;
}

mu::llvmc::block::~block ()
{
    parent->accept (unresolved);
}

mu::llvmc::node_result mu::llvmc::ptr_type::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto type (new mu::llvmc::ast::pointer_type (parser_a.current_template));
    result.node = type;
    auto first (region_a.first);
    result.error = parser_a.parse_ast_or_refer (
                                 [type, first]
                                 (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
                                 {
                                     type->pointed_type = node_a;
                                     type->region = mu::core::region (first, region_a.last);
                                 });
    return result;
}

mu::llvmc::node_result mu::llvmc::number::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
	result.error = parser_a.parse_identifier (
		[&] (mu::io::identifier * identifier_a)
		{
			result.node = new mu::llvmc::ast::number (identifier_a->string, parser_a.current_template);
			result.node->region.first = region_a.first;
			result.node->region.last = identifier_a->region.last;
			return nullptr;
		}, U"Expecting an identifier", mu::core::error_type::expecting_identifier);
    return result;
}

mu::llvmc::ast::number::number (mu::string const & number_a, mu::llvmc::template_context * context_a) :
node (context_a),
number_m (number_a)
{
}

mu::llvmc::node_result mu::llvmc::asm_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto asm_l (new mu::llvmc::ast::asm_c (parser_a.current_template));
    asm_l->region.first = region_a.first;
    result.error = parser_a.parse_ast_or_refer (
       [asm_l]
       (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
       {
           asm_l->type = node_a;
       }
    );
    if (result.error == nullptr)
    {
        result.error = parser_a.parse_identifier (
        [&]
           (mu::io::identifier * identifier_a)
            {
                asm_l->text = identifier_a->string;
				return nullptr;
            },
        U"Expecting asm text", mu::core::error_type::asm_hook_expecting_identifier);
        if (result.error == nullptr)
        {
            result.error = parser_a.parse_identifier (
                [&]
                (mu::io::identifier * identifier_a)
                {
                    asm_l->constraints = identifier_a->string;
                    asm_l->region.last = identifier_a->region.last;
					return nullptr;
                },
        U"Expecting asm constraints", mu::core::error_type::asm_hook_expecting_constraints);
            if (result.error == nullptr)
            {
                result.node = asm_l;
            }
        }
    }
    return result;
}

mu::llvmc::partial_ast_result::partial_ast_result (mu::io::token * token_a, mu::llvmc::ast::node * ast_a, mu::core::error * error_a):
token (token_a),
ast (ast_a),
error (error_a)
{
    assert (valid ());
}

mu::llvmc::partial_ast_result::partial_ast_result (mu::llvmc::partial_ast_result const & other_a):
token (other_a.token),
ast (other_a.ast),
error (other_a.error)
{
    assert (valid ());
}

mu::llvmc::partial_ast_result & mu::llvmc::partial_ast_result::operator = (mu::llvmc::partial_ast_result const & other_a)
{
    token = other_a.token;
    ast = other_a.ast;
    error = other_a.error;
    assert (valid ());
    return *this;
}

bool mu::llvmc::partial_ast_result::valid ()
{
    auto result (not ((token != nullptr and ast != nullptr) or (token != nullptr and error != nullptr) or (ast != nullptr and error != nullptr)));
    return result;
}

void mu::llvmc::parser::consume ()
{
    stream.consume (1);
}

mu::llvmc::partial_ast_result mu::llvmc::parser::peek ()
{
    mu::llvmc::partial_ast_result result ({nullptr, nullptr, nullptr});
    auto token (stream [0]);
    consume ();
    auto id (token->id ());
    switch (id)
    {
        case mu::io::token_id::identifier:
        {
            auto identifier (static_cast <mu::io::identifier *> (token));
            auto hook (keywords.get_hook (identifier->string));
            if (hook.hook != nullptr)
            {
                mu::llvmc::parser_context context (*this, &hook.hook->name ());
                auto ast (hook.hook->parse (token->region, *this));
                if (ast.node != nullptr)
                {
                    result = {nullptr, ast.node, nullptr};
                }
                else
                {
                    result = {nullptr, nullptr, ast.error};
                }
            }
            else
            {
                result = {token, nullptr, nullptr};
            }
            break;
        }
        case mu::io::token_id::left_square:
        {
            mu::llvmc::expression expression_l (token->region, *this);
            expression_l.parse ();
            if (expression_l.result.node != nullptr)
            {
                result.ast = expression_l.result.node;
            }
            else
            {
                result.error = expression_l.result.error;
            }
            break;
        }
        default:
            result = {token, nullptr, nullptr};
            break;
    }
    return result;
}

mu::core::error * mu::llvmc::parser::parse_left_square_required (char32_t const * error_message_a, mu::core::error_type error_type_a)
{
    mu::core::error * result (nullptr);
    auto next (stream [0]);
    auto id (next->id ());
    switch (id)
    {
        case mu::io::token_id::left_square:
            consume ();
            break;
        default:
            result = new mu::core::error_string (error_message_a, error_type_a, next->region);
            break;
    }
    return result;
}

mu::llvmc::node_result mu::llvmc::array_type::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    auto node (new mu::llvmc::ast::fixed_array_type (parser_a.current_template));
    node->region.first = region_a.first;
    mu::llvmc::node_result result ({nullptr, nullptr});
    result.error = parser_a.parse_ast_or_refer (
        [=]
        (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
        {
            node->element_type = node_a;
        }
    );
    if (result.error == nullptr)
    {
        result.error = parser_a.parse_ast_or_refer (
            [=]
            (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
            {
                node->size = node_a;
                node->region.last = region_a.last;
            }
        );
        if (result.error == nullptr)
        {
            result.node = node;
        }
    }
    return result;
}

mu::llvmc::node_result mu::llvmc::constant_array::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto node (new mu::llvmc::ast::constant_array (parser_a.current_template));
    node->region.first = region_a.first;
    result.error = parser_a.parse_ast_or_refer (
        [=]
        (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
        {
            node->type = node_a;
        }
    );
    if (result.error == nullptr)
    {
        result.error = parser_a.parse_left_square_required (U"Expecting array initializer list", mu::core::error_type::expecting_array_initializers);
        if (result.error == nullptr)
        {
            auto done (false);
            while (!done && result.error == nullptr)
            {
                result.error = parser_a.parse_ast_or_refer_or_right_square (
                    [&, node]
                    (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
                    {
                        node->initializer.push_back (node_a);
                    },
                    [&]
                    (mu::io::right_square * token_a)
                    {
                        node->region.last = token_a->region.last;
                        done = true;
                    }, U"Expecting array initializer list", mu::core::error_type::expecting_array_initializers);
            }
            if (result.error == nullptr)
            {
                result.node = node;
            }
        }
    }
    return result;
}

mu::llvmc::node_result mu::llvmc::string_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
	mu::llvmc::node_result result ({nullptr, nullptr});
	result.error = parser_a.parse_identifier (
    [&]
    (mu::io::identifier * identifier_a)
    {
		auto int_type (new mu::llvmc::skeleton::integer_type (32));
		mu::vector <mu::llvmc::skeleton::constant *> initializer;
		for (auto i: identifier_a->string)
		{
			initializer.push_back (new mu::llvmc::skeleton::constant_integer (identifier_a->region, new mu::llvmc::skeleton::integer_type (32), i));
		}
		auto value (new mu::llvmc::skeleton::constant_array (mu::core::region (region_a.first, identifier_a->region.last), new mu::llvmc::skeleton::fixed_array_type (int_type, initializer.size ()), initializer));
		result.node = new mu::llvmc::ast::value (value, parser_a.current_template);
		return nullptr;
	}, U"String hook is expecting an identifier", mu::core::error_type::expecting_identifier);
	return result;
}

mu::llvmc::node_result mu::llvmc::ascii_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
	mu::llvmc::node_result result ({nullptr, nullptr});
	result.error = parser_a.parse_identifier (
		[&]
		(mu::io::identifier * identifier_a)
		{
			mu::core::error * result_l (nullptr);
			auto int_type (new mu::llvmc::skeleton::integer_type (8));
			mu::vector <mu::llvmc::skeleton::constant *> initializer;
			for (auto i (identifier_a->string.begin ()), j (identifier_a->string.end ()); i != j && result_l == nullptr; ++i)
			{
				uint32_t value (*i);
				if (value < 0x100)
				{
					initializer.push_back (new mu::llvmc::skeleton::constant_integer (identifier_a->region, new mu::llvmc::skeleton::integer_type (8), value));
				}
				else
				{
					result_l = new mu::core::error_string (U"Character doesn't fit in to an ASCII character", mu::core::error_type::character_does_not_fit_in_to_an_ascii_character, identifier_a->region);
				}
			}
			auto value (new mu::llvmc::skeleton::constant_array (mu::core::region (region_a.first, identifier_a->region.last), new mu::llvmc::skeleton::fixed_array_type (int_type, initializer.size ()), initializer));
			result.node = new mu::llvmc::ast::value (value, parser_a.current_template);
			return result_l;
		}, U"String hook is expecting an identifier", mu::core::error_type::expecting_identifier);
	return result;
}

mu::llvmc::node_result mu::llvmc::global_variable::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto global (new mu::llvmc::ast::global_variable (parser_a.current_template));
    global->region.first = region_a.first;
    result.error = parser_a.parse_ast_or_refer (
                                                [=]
                                                (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
                                                {
                                                    global->initializer = node_a;
                                                    global->region.last = region_a.last;
                                                }
                                                );
    if (result.error == nullptr)
    {
        result.node = global;
    }
    return result;
}

mu::llvmc::node_result mu::llvmc::constant_pointer_null::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto constant (new mu::llvmc::ast::constant_pointer_null (parser_a.current_template));
    constant->region.first = region_a.first;
    result.error = parser_a.parse_ast_or_refer (
                                                [=]
                                                (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
                                                {
                                                    constant->type = node_a;
                                                    constant->region.last = region_a.last;
                                                }
                                                );
    if (result.error == nullptr)
    {
        result.node = constant;
    }
    return result;
}

mu::llvmc::node_result mu::llvmc::join_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto join (new mu::llvmc::ast::join (parser_a.current_template));
    result.error = parser_a.parse_left_square_required (U"Join must start with a left square", mu::core::error_type::expecting_left_square);
    if (result.error == nullptr)
    {
        auto done (false);
        while (!done && result.error == nullptr)
        {
            result.error = parser_a.parse_left_or_right_square (
                [&]
                (mu::core::region const & region_a)
                {
                    mu::core::error * error (nullptr);
                    auto & branch (join->add_branch ());
                    auto predicates (false);
                    auto done (false);
                    while (!done && error == nullptr)
                    {
                        error = parser_a.parse_ast_or_refer_or_right_square_or_terminator (
                            [&]
                            (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
                            {
                                if (predicates)
                                {
                                    branch.predicates.push_back (node_a);
                                }
                                else
                                {
                                    branch.arguments.push_back (node_a);
                                }
                            },
                            [&]
                            (mu::core::region const & region_a)
                            {
                                done = true;
                            },
                            [&]
                            (mu::core::region const & region_a)
                            {
                                if (!predicates)
                                {
                                    predicates = true;
                                }
                                else
                                {
                                    error = new mu::core::error_string (U"Already parsing predicates", mu::core::error_type::already_parsing_predicates);
                                }
                            }, U"Expecting ast or reference or right square or terminator", mu::core::error_type::expecting_branch_or_right_square);
                    }
                    return error;
                },
                [&]
                (mu::core::region const & region_a)
                {
                    mu::core::error * error (nullptr);
                    done = true;
                    return error;
                }, U"Expecting branch or right square", mu::core::error_type::expecting_branch_or_right_square);
        }
    }
    if (result.error == nullptr)
    {
        result.node = join;
    }
    return result;
}

mu::llvmc::node_result mu::llvmc::undefined_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto undefined (new mu::llvmc::ast::undefined (parser_a.current_template));
    result.error = parser_a.parse_ast_or_refer (
        [=]
        (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
        {
            undefined->type = node_a;
        }
    );
    if (result.error == nullptr)
    {
        result.node = undefined;
    }
    return result;
}

mu::llvmc::node_result mu::llvmc::struct_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    result.error = parser_a.parse_left_square_required (U"Struct definition must beging with a left square", mu::core::error_type::expecting_left_square);
    auto struct_l (new mu::llvmc::ast::struct_type (parser_a.current_template));
    if (result.error == nullptr)
    {
        auto done (false);
		size_t index (0);
        while (!done && result.error == nullptr)
        {
			result.error = parser_a.parse_identifier_or_right_square (
				   [&]
				   (mu::io::identifier * identifier_a)
					{
						mu::core::error * result (nullptr);
						auto existing (struct_l->names.find (identifier_a->string));
						if (existing == struct_l->names.end ())
						{
							struct_l->names [identifier_a->string] = index;
							++index;
						}
						else
						{
							result = new mu::core::error_string (U"Element name has already been used", mu::core::error_type::unable_to_use_identifier, identifier_a->region);
						}
						return result;
					},
					[&]
					(mu::io::right_square * right_square_a)
					{
						done = true;
						return nullptr;
					},U"Expecting identifier or right square", mu::core::error_type::expecting_identifier_or_right_square);
			if (result.error == nullptr && !done)
			{
				result.error = parser_a.parse_ast_or_refer (
					[struct_l]
					(mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
					{
						struct_l->elements.push_back (node_a);
					});
			}
        }
    }
    if (result.error == nullptr)
    {
        result.node = struct_l;
    }
    return result;
}

mu::string const & mu::llvmc::array_type::name ()
{
    static mu::string const result (U"array_type");
    return result;
}

mu::string const & mu::llvmc::ascii_hook::name ()
{
    static mu::string const result (U"ascii_hook");
    return result;
}

mu::string const & mu::llvmc::string_hook::name ()
{
    static mu::string const result (U"string_hook");
    return result;
}

mu::string const & mu::llvmc::struct_hook::name ()
{
    static mu::string const result (U"struct_hook");
    return result;
}

mu::string const & mu::llvmc::constant_int::name ()
{
    static mu::string const result (U"constant_int");
    return result;
}

mu::string const & mu::llvmc::function_hook::name ()
{
    static mu::string const result (U"function_hook");
    return result;
}

mu::string const & mu::llvmc::constant_array::name ()
{
    static mu::string const result (U"constant_array");
    return result;
}

mu::string const & mu::llvmc::undefined_hook::name ()
{
    static mu::string const result (U"undefined_hook");
    return result;
}

mu::string const & mu::llvmc::global_variable::name ()
{
    static mu::string const result (U"global_variable");
    return result;
}

mu::string const & mu::llvmc::constant_pointer_null::name ()
{
    static mu::string const result (U"constant_pointer_null");
    return result;
}

mu::string const & mu::llvmc::module_hook::name ()
{
    static mu::string const result (U"module");
    return result;
}

mu::string const & mu::llvmc::number::name ()
{
    static mu::string const result (U"number");
    return result;
}

mu::string const & mu::llvmc::asm_hook::name ()
{
    static mu::string const result (U"asm_hook");
    return result;
}

mu::string const & mu::llvmc::int_type::name ()
{
    static mu::string const result (U"int_type");
    return result;
}

mu::string const & mu::llvmc::let_hook::name ()
{
    static mu::string const result (U"let_hook");
    return result;
}

mu::string const & mu::llvmc::ptr_type::name ()
{
    static mu::string const result (U"ptr_type");
    return result;
}

mu::string const & mu::llvmc::set_hook::name ()
{
    static mu::string const result (U"set_hook");
    return result;
}

mu::string const & mu::llvmc::join_hook::name ()
{
    static mu::string const result (U"join_hook");
    return result;
}

mu::string const & mu::llvmc::loop_hook::name ()
{
    static mu::string const result (U"loop_hook");
    return result;
}

mu::llvmc::parser_context::parser_context (mu::llvmc::parser & parser_a, mu::string const * name_a) :
parser (parser_a)
{
    parser_a.parse_stack.push_back (mu::llvmc::parser_frame ({parser_a.stream [0]->region.first, name_a}));
}

mu::llvmc::parser_context::~parser_context ()
{
    parser.parse_stack.pop_back ();
}

mu::llvmc::parser_error::parser_error (char32_t const * message_a, mu::core::error_type type_a, mu::core::region const & region_a, mu::llvmc::parser & parser_a) :
type_m (type_a),
message (message_a),
region_m (region_a),
parse_stack (parser_a.parse_stack)
{
}

void mu::llvmc::parser_error::output (std::ostream & out)
{
    std::string string (message.begin (), message.end ());
    mu::string const & region_mu (region_m.string ());
    std::string region (region_mu.begin (), region_mu.end ());
    out << string << "(" << region << ")" << std::endl;
    for (auto & i: parse_stack)
    {
        std::string message (i.name->begin (), i.name->end ());
        mu::string const & region_mu (region_m.string ());
        std::string region (region_mu.begin (), region_mu.end ());
        out << message << "(" << region << ")" << std::endl;
    }
}

mu::core::error_type mu::llvmc::parser_error::type ()
{
    return type_m;
}

mu::core::region mu::llvmc::parser_error::region ()
{
    return region_m;
}

mu::llvmc::node_result mu::llvmc::template_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
	mu::llvmc::block block (parser_a.current_mapping);
    parser_a.current_mapping = &block;
    parser_a.current_template = new mu::llvmc::template_context ({parser_a.current_template});
	mu::llvmc::node_result result ({nullptr, nullptr});
	result.error = parser_a.parse_left_square_required (U"template parser expecting parameter list", mu::core::error_type::expecting_left_square);
	auto template_l (new mu::llvmc::ast::template_c (parser_a.current_template, parser_a.current_template->parent));
	if (result.error == nullptr)
	{
		auto done (false);
		while (!done && result.error == nullptr)
		{
			result.error = parser_a.parse_identifier_or_right_square (
				[&]
				(mu::io::identifier * identifier_a) 
				{
					mu::core::error * result (nullptr);
					auto node (new mu::llvmc::ast::template_parameter (identifier_a->string, parser_a.current_template));
					template_l->parameters.push_back (node);
					auto error (parser_a.current_mapping->insert (identifier_a->string, node));
					if (error)
					{
						result = new mu::core::error_string (U"Unable to use name", mu::core::error_type::unable_to_use_identifier);
					}
					return result;
				}, 
				[&] 
				(mu::io::right_square * right_square_a) 
				{
					done = true;
					return nullptr;
				}, U"Template parser expecting parameter name or right square", mu::core::error_type::expecting_identifier_or_right_square
			);
		}
		if (result.error == nullptr)
		{
			result.error = parser_a.parse_left_square_required (U"template parser expecting template body", mu::core::error_type::expecting_left_square);
			auto done (false);
			while (!done && result.error == nullptr)
			{
				auto position (template_l->body.size ());
				template_l->body.push_back (nullptr);
				result.error = parser_a.parse_ast_or_refer_or_right_square (
					[template_l, position] 
					(mu::llvmc::ast::node * node_a, mu::core::region const & region_a) 
					{
						template_l->body [position] = node_a;
					}, 
					[&] 
					(mu::io::right_square * right_square_a) 
					{
						done = true;
					}, 
					U"Template body expecting references or right square", mu::core::error_type::expecting_argument_or_right_square
				);
			}
			template_l->body.pop_back ();
		}
	}
	if (result.error == nullptr)
	{
		result.node = template_l;
	}
    parser_a.current_template = parser_a.current_template->parent;
    parser_a.current_mapping = block.parent;
	return result;
}

mu::string const & mu::llvmc::template_hook::name ()
{
    static mu::string const result (U"template_hook");
	return result;
}

mu::llvmc::node_result mu::llvmc::entry_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    auto entry (new mu::llvmc::ast::entry (parser_a.current_template));
    entry->region = region_a;
    mu::llvmc::node_result result ({nullptr, nullptr});
    result.error = parser_a.parse_ast_or_refer (
        [entry]
        (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
        {
            entry->function = node_a;
        }
    );
    if (result.error == nullptr)
    {
        result.node = entry;
    }
    return result;
}


mu::string const & mu::llvmc::entry_hook::name ()
{
    static mu::string const result (U"entry_hook");
    return result;
}

bool mu::llvmc::template_context::should_clone (mu::llvmc::template_context * node_a)
{
	bool result (false);
	auto current (node_a);
	while (!result && current != nullptr)
	{
		result = (current == this);
		current = current->parent;
	}
	return result;
}

mu::llvmc::node_result mu::llvmc::namespace_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    auto namespace_l (new mu::llvmc::ast::namespace_c (parser_a.current_template));
    mu::llvmc::node_result result ({nullptr, nullptr});
	namespace_l->region.first = region_a.first;
    result.error = parser_a.parse_ast_or_refer (
        [namespace_l]
        (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
        {
            namespace_l->node_m = node_a;
        });
    if (result.error == nullptr)
    {
        result.error = parser_a.parse_identifier (
            [namespace_l]
            (mu::io::identifier * identifier_a)
            {
                namespace_l->member = identifier_a->string;
				namespace_l->region.last = identifier_a->region.last;
                return nullptr;
            }, U"Expecting a namespace member name", mu::core::error_type::expecting_identifier);
        if (result.error == nullptr)
        {
            result.node = namespace_l;
        }
    }
    return result;
}

mu::string const & mu::llvmc::namespace_hook::name ()
{
    static mu::string const result (U"namespace_hook");
    return result;
}

mu::llvmc::module::module (mu::core::region const & region_a, mu::llvmc::parser & parser_a) :
block (parser_a.current_mapping),
result ({nullptr, nullptr}),
parser (parser_a),
first (region_a)
{
	parser_a.current_mapping = &block;
}

mu::llvmc::module::~module ()
{
	assert (parser.current_mapping == &block);
	parser.current_mapping = block.parent;
}

mu::llvmc::node_result mu::llvmc::sequence_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    auto sequence_l (new mu::llvmc::ast::sequence (nullptr, parser_a.current_template));
    mu::llvmc::node_result result ({nullptr, nullptr});
	sequence_l->region.first = region_a.first;
    result.error = parser_a.parse_ast_or_refer (
        [sequence_l]
        (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
        {
            sequence_l->node_m = node_a;
        });
    if (result.error == nullptr)
    {
        result.node = sequence_l;
    }
    return result;
}

mu::string const & mu::llvmc::sequence_hook::name ()
{
    static mu::string const result (U"sequence_hook");
    return result;
}

mu::llvmc::node_result mu::llvmc::function_overload_hook::parse (mu::core::region const & region_a, mu::llvmc::parser & parser_a)
{
    mu::llvmc::node_result result ({nullptr, nullptr});
    auto overload (new mu::llvmc::ast::function_overload (parser_a.current_template));
    result.error = parser_a.parse_identifier (
        [&parser_a, overload]
        (mu::io::identifier * identifier_a)
        {
            auto empty (parser_a.current_mapping->get (identifier_a->string, identifier_a->region,
                [overload]
                (mu::llvmc::ast::node * node_a, mu::core::region const & region_a)
                {
                    overload->family = node_a;
                }));
            if (empty)
            {
                auto family (new mu::llvmc::ast::function_family);
                parser_a.current_mapping->insert (identifier_a->string, family);
                overload->family = family;
            }
            return nullptr;
        }, U"Expecting function name", mu::core::error_type::expecting_function_name);
    if (result.error == nullptr)
    {
		auto result2 (function_hook.parse (region_a, parser_a));
		if (result2.error == nullptr)
		{
			overload->function = result2.node;
			result.node = overload;
		}
		else
		{
			result.error = result2.error;
		}
    }
    return result;
}

mu::string const & mu::llvmc::function_overload_hook::name ()
{
    static mu::string const name (U"function_overload_hook");
    return name;
}

mu::llvmc::function_overload_hook::function_overload_hook (mu::llvmc::function_hook & function_hook_a) :
function_hook (function_hook_a)
{
}