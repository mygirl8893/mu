//
//  declaration.cpp
//  lambda_p
//
//  Created by Colin LeMahieu on 9/2/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <lambda_p/tokens/declaration.h>

::lambda_p::tokens::token_ids lambda_p::tokens::declaration::token_id ()
{
	return ::lambda_p::tokens::token_id_declaration;
}