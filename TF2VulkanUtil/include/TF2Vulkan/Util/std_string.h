#pragma once

#include <string>

#include "std_compare.h"

namespace Util{ namespace string
{
	using std::to_string;
	inline std::string&& to_string(std::string&& str)
	{
		return std::move(str);
	}
	inline std::string to_string(const std::string& str)
	{
		return str;
	}
	inline std::string to_string(const std::string_view& str)
	{
		return std::string(str);
	}

	template<typename CharT = char, typename Traits = std::char_traits<CharT>, typename Allocator = std::allocator<CharT>, typename... TArgs>
	std::basic_string<CharT, Traits, Allocator> concat(const TArgs&... args)
	{
		using str = std::basic_string<CharT, Traits, Allocator>;

		str formatted[] = { Util::string::to_string(args)... };

		for (size_t i = 1; i < std::size(formatted); i++)
			formatted[0] += formatted[i];

		return std::move(formatted[0]);
	}
} }

#ifndef __INTELLISENSE__
template<typename TChar, typename TAlloc>
inline std::strong_ordering operator<=>(const std::basic_string<TChar, TAlloc>& lhs, const std::basic_string<TChar, TAlloc>& rhs)
{
	if (lhs < rhs)
		return std::strong_ordering::less;
	else if (rhs < lhs)
		return std::strong_ordering::greater;
	else
		return std::strong_ordering::equal;
}
#endif
