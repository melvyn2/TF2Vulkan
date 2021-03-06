#pragma once

#undef min
#undef max

#include <algorithm>

namespace Util{ namespace algorithm
{
	using std::find;

	template<typename TContainer, typename TValue>
	inline bool contains(const TContainer& container, const TValue& value)
	{
		return std::find(std::begin(container), std::end(container), value) != std::end(container);
	}

	template<typename TContainer, typename TFunc>
	inline bool contains_if(const TContainer& container, const TFunc& func)
	{
		return std::find_if(std::begin(container), std::end(container), func) != std::end(container);
	}

	template<typename TContainer, typename TValue>
	inline auto find(const TContainer& container, const TValue& value)
	{
		return std::find(std::begin(container), std::end(container), value);
	}

	template<typename TContainer, typename TValue, typename TFunc>
	inline bool find_and(const TContainer& container, const TValue& value, const TFunc& func)
	{
		if (auto found = find(container, value); found != std::end(container))
		{
			func(found);
			return true;
		}

		return false;
	}

	template<typename TContainer, typename TFunc>
	inline auto find_if(const TContainer& container, const TFunc& func)
	{
		return std::find_if(std::begin(container), std::end(container), func);
	}

	template<typename TContainer, typename TSearchFunc, typename TFoundFunc>
	inline bool find_if_and(const TContainer& container, const TSearchFunc& searchFunc, const TFoundFunc& foundFunc)
	{
		if (auto found = find_if(container, searchFunc); found != std::end(container))
		{
			foundFunc(found);
			return true;
		}

		return false;
	}

	template<typename TContainer, typename TValue, typename TFunc>
	inline bool find_and(TContainer& container, const TValue& value, const TFunc& func)
	{
		if (auto found = find(container, value); found != std::end(container))
		{
			func(found);
			return true;
		}

		return false;
	}

	template<typename TContainer, typename TValue>
	inline bool try_erase(TContainer& container, const TValue& value)
	{
		return find_and<TContainer, TValue>(container, value, [&](auto found) { container.erase(found); });
	}

	template<typename T, size_t size>
	inline void copy(const T(&src)[size], T(&dest)[size])
	{
		std::copy(std::begin(src), std::end(src), dest);
	}

	template<typename T, typename T2>
	inline void copy(const T& src, T2& dest)
	{
		std::copy(std::begin(src), std::end(src), dest);
	}

	template<typename T1, typename T2>
	inline constexpr auto max(const T1& t1, const T2& t2)
	{
		using CT = std::common_type_t<T1, T2>;
		return std::max<CT>(Util::SafeConvert<CT>(t1), Util::SafeConvert<CT>(t2));
	}
	template<typename T1, typename T2>
	inline constexpr auto min(const T1& t1, const T2& t2)
	{
		using CT = std::common_type_t<T1, T2>;
		return std::min<CT>(Util::SafeConvert<CT>(t1), Util::SafeConvert<CT>(t2));
	}

	template<typename TIter>
	inline bool all_of(TIter begin, TIter end)
	{
		return std::all_of(begin, end, [](const auto& v) { return !!v; });
	}
	template<typename TContainer>
	inline bool all_of(const TContainer& container)
	{
		return all_of(std::begin(container), std::end(container));
	}
} }
