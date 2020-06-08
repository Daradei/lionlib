#ifndef LION_RANGE_RANGE_HPP
#define LION_RANGE_RANGE_HPP

#include <type_traits>
#include <iterator>

namespace lion
{
	template<typename Iterator, bool Integer = std::is_integral_v<Iterator>, bool Reverse = false>
	class range
	{
	private:
		Iterator first;
		Iterator last;

	public:
		constexpr range(Iterator begin, Iterator end) noexcept
			: first(begin), last(end)
		{}
		constexpr Iterator begin() const noexcept { return first; }
		constexpr Iterator end()   const noexcept { return last; }

		constexpr range<std::reverse_iterator<Iterator>> reverse() const noexcept {
			return { std::make_reverse_iterator(last), std::make_reverse_iterator(first) };
		}

		constexpr range<Iterator> including() const noexcept {
			return { first, std::next(last) };
		}
	};
}

#endif
