#pragma once

#include <vector>

namespace lambda_p_script
{
	template <typename T, typename U = std::vector <T>>
	class segment
	{
	public:
		segment (U & source_a)
			: lower (0),
			size_m (source_a.size ()),
			source (source_a)
		{
		}
		segment (size_t lower_a, size_t size_a, U & source_a)
			: lower (lower_a),
			size_m (size_a),
			source (source_a)
		{
		}
		T & operator [] (size_t position_a)
		{
			return source [lower + position_a];
		}
		typename U::iterator begin ()
		{
			return source.begin () + lower;
		}
		typename U::iterator end ()
		{
			return source.begin () + lower + size_m;
		}
		size_t size ()
		{
			return size_m;
		}
		size_t lower;
		size_t size_m;
		U & source;
	};
}

