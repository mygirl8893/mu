#pragma once

#include <iostream>
#include <mu/core/region.hpp>

namespace mu
{
    namespace core
    {
        enum class error_type
        {
            unknown,
            unknown_control_character,
            terminator_token_too_long,
            end_of_stream_inside_complex_identifier,
            non_hex_character,
            end_of_stream_inside_region_comment,
            expecting_type_in_parameters,
            unknown_expression_subclass,
            no_value_at_index,
            value_has_only_one_element,
            expecting_one_value,
            node_is_not_a_value,
            expecting_a_type,
            result_branch_is_not_distinct,
            expecting_a_function,
            expecting_a_module,
            expecting_a_target,
            unit_cannot_be_target_of_call,
            expecting_first_argument_to_be_call_target,
            expecting_a_call_target,
            cycle_in_expressions,
            argument_type_does_not_match_parameter_type,
            argument_to_function_is_not_a_value,
            pointer_does_not_point_to_a_function,
            only_function_pointers_can_be_target_of_call,
            join_arguments_must_be_values,
            branches_are_not_disjoint,
            joining_types_are_different,
            must_be_joining_at_least_two_values,
            if_instruction_expects_one_bit_integer,
            if_instruction_expects_integer_type_value,
            if_instruction_expects_a_value_argument,
            if_instruction_expects_one_argument,
            add_arguments_must_have_same_bit_width,
            add_arguments_must_be_integers,
            add_arguments_must_be_values,
            add_expects_two_arguments,
            arguments_are_disjoint,
            expecting_function,
            expecting_function_or_end_of_stream,
            function_name_already_used,
            expecting_identifier,
            expecting_function_name,
            parsing_parameters_expecting_left_square,
            unable_to_use_identifier,
            parsing_parameters_expecting_identifier,
            expecting_a_parameter_name,
            expecting_type_or_right_square,
            expecting_expression,
            expecting_left_square,
            expecting_identifier_or_right_square,
            expecting_result_reference,
            expecting_right_square,
            bit_width_too_wide,
            unable_to_convert_number_to_unsigned_integer,
            expecting_argument_or_right_square,
            expecting_name,
            expecting_identifier_or_left_square,
            unknown_token_in_let_statement,
            expecting_expression_or_right_square,
            expecting_loop_body,
            expecting_result_set_or_right_square,
            expecting_loop_results,
            already_parsing_predicates,
            unresolved_symbols,
            incorrect_number_of_arguments,
            expecting_a_number,
            error_converting_string_to_number,
            store_expects_two_arguments,
            store_arguments_must_be_values,
            store_right_argument_pointer_type,
            store_right_pointed_type_doesnt_match_left,
            load_expects_one_argument,
            load_argument_must_be_values,
            load_argument_pointer_type,
            mismatch_number_of_arguments_number_of_parameters,
            loop_argument_must_be_value,
            loop_result_must_be_value,
			loop_not_enough_branches,
			icmp_expects_predicate_two_arguments,
			icmp_first_argument_predicate,
			icmp_left_argument_value,
			icmp_right_argument_value,
			icmp_arguments_integers,
			icmp_arguments_same_type,
			branch_analyzer_leaves_exist,
			branch_analyzer_ancestor_exists,
			branch_analyzer_already_ancestor,
			branch_analyzer_disjoint,
			branch_analyzer_intersection_exists,
            sub_arguments_must_have_same_bit_width,
            sub_arguments_must_be_integers,
            sub_arguments_must_be_values,
            sub_expects_two_arguments,
            asm_hook_expecting_identifier,
            asm_hook_expecting_type,
            asm_hook_expecting_constraints,
			inline_asm_requires_values,
			alloca_expects_one_argument,
			alloca_argument_type,
			instruction_arguments_must_have_same_bit_width,
			instruction_arguments_must_be_integers,
			instruction_arguments_must_be_values,
			instruction_expects_two_arguments,
			cmpxchg_requires_three_arguments,
			cmpxchg_argument_one_pointer,
			cmpxchg_one_point_two,
			cmpxchg_one_point_three,
			getelementptr_requires_two,
			getelementptr_requires_values,
			getelementptr_requires_pointer_type,
			getelementptr_first_argument_integer_type,
			getelementptr_trailing_constant,
			getelementptr_trailing_32bit,
			expecting_a_result,
			expecting_an_expression,
			numbers_parsed_by_keyword,
            actual_formal_result_type_mismatch,
            switch_requires_input,
            switch_requires_value,
            switch_requires_integer,
            switch_requires_case_constant,
            switch_requires_matching_case_types,
            switch_requires_unique_case,
            expecting_ast_or_reference,
            expecting_function_declaration,
            initializer_must_be_constant,
            initializer_type_doesnt_match,
            expecting_array_initializers,
            argument_must_be_pointer_type,
			character_does_not_fit_in_to_an_ascii_character,
            expecting_a_value,
            ptr_to_int_expects_two,
            expecting_pointer_type,
            expecting_integer_type,
            global_one_initializer,
            global_constant_initializer,
            expecting_a_pointer_type,
            must_be_joining_at_least_two_branches,
            join_branches_same_cardinality,
            expecting_branch_or_right_square,
            struct_must_contain_types,
            insertvalue_expects_three_arguments,
            expecting_value_to_be_struct,
            expecting_an_aggregate,
            index_out_of_bounds,
            value_does_not_match_aggregate,
			unexpected_node_type,
			select_three_arguments,
			expecting_one_bit_integer,
			select_arguments_same_type,
            expecting_a_constant,
			template_instantiations_cannot_have_predicates,
            only_one_entry_point,
            entry_point_must_be_a_function,
            entry_point_must_have_no_arguments,
            entry_point_must_have_one_return_branch,
            entry_point_cannot_return_values,
            function_already_named,
			global_already_named,
            not_a_namespace_container,
            not_a_member,
            can_only_extract_from_one,
			template_argument_count_mismatch,
			expecting_an_integer_type,
			expecting_type_or_right_square_or_terminator,
			expecting_left_square_or_right_square
        };
        class error
        {
        public:
            virtual ~error ();
            virtual void output (std::ostream & out) = 0;
            std::string string ();
            virtual mu::core::error_type type () = 0;
			virtual mu::core::region region () = 0;
        };
    }
}