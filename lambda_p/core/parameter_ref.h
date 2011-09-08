//
//  parameter_ref.h
//  lambda_p
//
//  Created by Colin LeMahieu on 8/31/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <cstring>
#include <iostream>

#include <lambda_p/core/node.h>

namespace lambda_p
{
    namespace serialization
    {
        template <typename> class simple;
    }
    namespace core
    {
        class routine;
        class parameter_ref : public node
        {
            template <typename> friend class ::lambda_p::serialization::simple;
        public:
            parameter_ref (::lambda_p::core::routine * routine_a, size_t target_parameter_a, size_t self_statement_a, size_t self_argument_a);
            ~parameter_ref ();
            void validate (::std::iostream & problems) const;
			node_id node_type () const;
        private:
            ::lambda_p::core::routine * routine;
            size_t target_parameter;
            size_t self_statement;
            size_t self_argument;
        };
    }
}