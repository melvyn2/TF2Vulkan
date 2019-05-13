#pragma once

#include <type_traits>

namespace Util{ namespace type_traits
{
	namespace detail
	{
		template<typename T> constexpr auto SafeUnderlyingTypeImpl()
		{
			if constexpr (std::is_enum_v<T>)
				return std::declval<std::underlying_type_t<T>>();
			else
				return std::declval<T>();
		}

		template<typename T, typename... TCase> constexpr auto IsAnyOfImpl()
		{

		}
	}

	template<typename T, typename... TCase> using is_any_of = std::disjunction<std::is_same<T, TCase>...>;
	template<typename T, typename... TCase> static constexpr bool is_any_of_v = is_any_of<T, TCase...>::value;

	template<typename T> using safe_underlying_type_t = decltype(detail::SafeUnderlyingTypeImpl<T>());
} }
