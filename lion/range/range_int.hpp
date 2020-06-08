#ifndef LION_RANGE_RANGE_INT_HPP
#define LION_RANGE_RANGE_INT_HPP

#include <cstddef>
#include <iterator>

namespace lion
{
	template<typename Iterator, bool Integer, bool Reverse>
	class range;

	template<typename Integer, bool Reverse>
	class range<Integer, true, Reverse>
	{
	private:
		Integer first;
		Integer last;

		template<typename T, bool Reverse = false>
		class iter
		{
		private:
			T val;

		public:
			using value_type		= T;
			using difference_type	= std::ptrdiff_t;
			using reference_type	= T& ;
			using pointer			= T* ;
			using iterator_category = std::bidirectional_iterator_tag;

		public:
			constexpr iter() = default;
			constexpr iter(const iter&) = default;

			constexpr iter(T v) noexcept
				: val(v)
			{}

			constexpr iter& operator=(const iter&) = default;

			constexpr value_type operator*() const noexcept { return val; }
			constexpr pointer operator->() const noexcept { return nullptr; }

			constexpr iter& operator++() noexcept
			{
				++val;
				return *this;
			}

			constexpr iter operator++(int) noexcept
			{
				iter temp = *this;
				++*this;
				return temp;
			}

			constexpr iter& operator--() noexcept
			{
				--val;
				return *this;
			}

			constexpr iter operator--(int) noexcept
			{
				iter temp = *this;
				--*this;
				return temp;
			}

			template<typename U>
			friend constexpr bool operator==(const iter<U>& lhs, const iter<U>& rhs) noexcept {
				return lhs.val == rhs.val;
			}

			template<typename U>
			friend constexpr bool operator!=(const iter<U>& lhs, const iter<U>& rhs) noexcept {
				return lhs.val != rhs.val;
			}
		};

		template<typename T>
		class iter<T, true>
		{
		private:
			T val;

		public:
			using value_type		= T;
			using difference_type	= std::ptrdiff_t;
			using reference_type	= T& ;
			using pointer			= T* ;
			using iterator_category = std::bidirectional_iterator_tag;

		public:
			constexpr iter() = default;
			constexpr iter(const iter&) = default;

			constexpr iter(T v) noexcept
				: val(v)
			{}

			constexpr iter& operator=(const iter&) = default;

			constexpr value_type operator*() const noexcept { return val; }
			constexpr pointer operator->() const noexcept { return nullptr; }

			constexpr iter& operator++() noexcept
			{
				--val;
				return *this;
			}

			constexpr iter operator++(int) noexcept
			{
				iter temp = *this;
				--*this;
				return temp;
			}

			constexpr iter& operator--() noexcept
			{
				++val;
				return *this;
			}

			constexpr iter operator--(int) noexcept
			{
				iter temp = *this;
				++*this;
				return temp;
			}

			template<typename U>
			friend constexpr bool operator==(const iter<U, true>& lhs, const iter<U, true>& rhs) noexcept {
				return lhs.val == rhs.val;
			}

			template<typename U>
			friend constexpr bool operator!=(const iter<U, true>& lhs, const iter<U, true>& rhs) noexcept {
				return lhs.val != rhs.val;
			}
		};

	public:
		constexpr range(Integer begin, Integer end) noexcept
			: first(begin), last(end)
		{}

		constexpr iter<Integer, Reverse> begin() const noexcept {
			return iter<Integer, Reverse>(first);
		}
		constexpr iter<Integer, Reverse> end() const noexcept {
			return iter<Integer, Reverse>(last);
		}

		constexpr range<Integer, true, true> reverse() const noexcept {
			return range<Integer, true, true>(last - 1, first - 1);
		}

		constexpr range<Integer, true, Reverse> including() const noexcept {
			if constexpr (Reverse) {
				return { first + 1, last };
			}
			else {
				return { first, last + 1 };
			}
		}
	};
}

#endif
