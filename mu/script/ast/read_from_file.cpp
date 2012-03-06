#include "read_from_file.h"

#include <mu/core/errors/error_list.h>
#include <mu/script/string/node.h>
#include <mu/io/lexer/istream_input.h>
#include <mu/io/builder.h>
#include <mu/io/ast/cluster.h>

#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#include <sstream>
#include <fstream>

void mu::script::ast::read_from_file::operator () (boost::shared_ptr <mu::core::errors::error_target> errors_a, mu::core::segment <boost::shared_ptr <mu::core::node>> parameters, std::vector <boost::shared_ptr <mu::core::node>> & results)
{
	auto one (boost::dynamic_pointer_cast <mu::script::string::node> (parameters [0]));
	if (one.get () != nullptr)
	{			
		auto path (::boost::filesystem::initial_path ());
		std::string relative (one->string.begin (), one->string.end ());
		path /= relative;
		std::ifstream stream;
		stream.open (path.string ().c_str ());
		if (stream.is_open ())
		{
			auto input (boost::shared_ptr <mu::io::lexer::istream_input> (new mu::io::lexer::istream_input (stream)));
			mu::io::builder builder;
			mu::io::source source (boost::bind (&mu::io::lexer::lexer::operator(), &builder.lexer, _1));
			source (input);
			source ();
			if (builder.errors->errors.empty ())
			{
				if (builder.clusters.size () == 1)
				{
					auto cluster (builder.clusters [0]);
					results.push_back (cluster);
				}
				else
				{
					std::wstringstream message;
					message << L"File did not contain one cluster: ";
					message << builder.clusters.size ();
					(*errors_a) (message.str ());
				}
			}
			else
			{
				for (auto i (builder.errors->errors.begin ()), j (builder.errors->errors.end ()); i != j; ++i)
				{
					(*errors_a) ((*i).first, (*i).second);
				}
			}
		}
		else
		{
			std::wstringstream message;
			message << L"File could not be opened: ";
			std::string patha (path.string ());
			std::wstring path (patha.begin (), patha.end ());
			message << path;
			(*errors_a) (message.str ());
		}
	}
	else
	{
		invalid_type (errors_a, parameters [0], 0);
	}
}

size_t mu::script::ast::read_from_file::count ()
{
	return 1;
}