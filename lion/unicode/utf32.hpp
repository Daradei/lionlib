#ifndef LION_UNICODE_UTF32_HPP
#define LION_UNICODE_UTF32_HPP

#include "utffwd.hpp"
#include "codepoint.hpp"
#include "encoding.hpp"

#include <cstddef>
#include <iterator>
#include <algorithm>
#include <string>
#include <istream>
#include <ostream>

namespace lion::unicode
{
	template<typename ForwardIterator>
	constexpr ForwardIterator utf32::decode(ForwardIterator first, ForwardIterator last, codepoint& cp)
	{
		cp = is_valid(*first) ? *first : replacement_character();
		return ++first;
	}

	template<typename ForwardIterator>
	constexpr ForwardIterator utf32::decode_lenient(ForwardIterator first, ForwardIterator last, codepoint& cp)
	{
		cp = *first;
		return ++first;
	}

	template<typename OutputIterator>
	constexpr OutputIterator utf32::encode(codepoint cp, OutputIterator output)
	{
		*output = cp;
		return ++output;
	}

	template<typename ForwardIterator>
	constexpr std::size_t utf32::length(ForwardIterator first, ForwardIterator last) {
		return std::distance(first, last);
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf32::to_utf8(ForwardIterator first, ForwardIterator last, OutputIterator output)
	{
		for (; first != last; ++first) {
			output = utf8::encode(*first, output);
		}
		return output;
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf32::to_utf16(ForwardIterator first, ForwardIterator last, OutputIterator output)
	{
		for (; first != last; ++first) {
			output = utf16::encode(*first, output);
		}
		return output;
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf32::to_utf32(ForwardIterator first, ForwardIterator last, OutputIterator output) {
		return std::copy(first, last, output);
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf32::to_utf8_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output) {
		return to_utf8(first, last, output);
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf32::to_utf16_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output) {
		return to_utf16(first, last, output);
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf32::to_utf32_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output) {
		return to_utf32(first, last, output);
	}

	template<typename OutputIterator>
	OutputIterator utf32::read(std::istream& in, OutputIterator output, byte_order order)
	{
		if (order == byte_order::none) {
			return output;
		}

		std::string text;
		in.seekg(0, std::ios::end);
		if (in.tellg() % 4 == 0) {
			text.resize(in.tellg());
		}
		else {
			text.resize(((in.tellg() / 4) + 1) * 4);
		}
			
		in.seekg(0, std::ios::beg);
		in.read(&text[0], text.size());

		if (order == byte_order::little)
		{
			for (std::string::size_type i = 0; i < text.size(); i += 4)
			{
				const codepoint bytes[] = {
					static_cast<unsigned char>(text[i + 3]),
					static_cast<unsigned char>(text[i + 2]),
					static_cast<unsigned char>(text[i + 1]),
					static_cast<unsigned char>(text[i])
				};
				*output++ = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
			}
		}
		else if (order == byte_order::big)
		{
			for (std::string::size_type i = 0; i < text.size(); i += 4)
			{
				const codepoint bytes[] = {
					static_cast<unsigned char>(text[i]),
					static_cast<unsigned char>(text[i + 1]),
					static_cast<unsigned char>(text[i + 2]),
					static_cast<unsigned char>(text[i + 3])
				};
				*output++ = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
			}
		}
		return output;
	}

	template<typename ForwardIterator>
	ForwardIterator utf32::write(std::ostream& out, ForwardIterator first, ForwardIterator last, byte_order order)
	{
		if (order == byte_order::none) {
			return first;
		}

		std::string towrite;
		if (order == byte_order::little)
		{
			for (ForwardIterator it = first; it != last; ++it)
			{
				const char bytes[] = {
					static_cast<char>(*it & 0x000000FF),
					static_cast<char>((*it & 0x0000FF00) >> 8),
					static_cast<char>((*it & 0x00FF0000) >> 16),
					static_cast<char>(*it >> 24)
				};
				towrite.append(bytes, 4);
			}
		}
		else if (order == byte_order::big)
		{
			for (ForwardIterator it = first; it != last; ++it)
			{
				const char bytes[] = {
					static_cast<char>(*it >> 24),
					static_cast<char>((*it & 0x00FF0000) >> 16),
					static_cast<char>((*it & 0x0000FF00) >> 8),
					static_cast<char>(*it & 0x000000FF)
				};
				towrite.append(bytes, 4);
			}
		}
		out.write(&towrite[0], towrite.size());
		return std::next(first, out.tellp() / 4);
	}

	template<typename ForwardIterator>
	void utf32::write_BOM(std::ostream& out, byte_order order)
	{
		if (order == byte_order::big) {
			out.write(constants::UTF32_BE_BOM.data(), 4);
		}
		else if (order == byte_order::little) {
			out.write(constants::UTF32_LE_BOM.data(), 4);
		}
	}

	template<typename ForwardIterator>
	constexpr ForwardIterator utf32::valid_sequence(ForwardIterator first, ForwardIterator last)
	{
		while (first != last)
		{
			ForwardIterator temp = first;

			codepoint cp;
			first = decode(first, last, cp);
			if (cp == replacement_character()) {
				return temp;
			}
		}
		return first;
	}

	template<typename Iterator>
	class utf32::iterator
	{
	public:
		using value_type		= codepoint;
		using difference_type	= std::ptrdiff_t;
		using pointer	        = void;
		using reference		    = codepoint&;
		using iterator_category = std::bidirectional_iterator_tag;
		using iterator_type		= Iterator;

	protected:
		iterator_type current;

	public:
		constexpr iterator() noexcept
			: current()
		{}

		explicit constexpr iterator(iterator_type it) noexcept
			: current(it)
		{}

		constexpr iterator(const iterator& rhs) noexcept
			: current(rhs.current)
		{}

		constexpr iterator& operator=(const iterator& rhs) noexcept
		{
			current = rhs.current;
			return *this;
		}

		constexpr reference operator*() const noexcept { return *current; }

		constexpr iterator& operator++() noexcept
		{
			++current;
			return *this;
		}

		constexpr iterator operator++(int) noexcept
		{
			iterator temp(*this);
			++*this;
			return temp;
		}

		constexpr iterator& operator--() noexcept
		{
			--current;
			return *this;
		}

		constexpr iterator operator--(int) noexcept
		{
			iterator temp(*this);
			--*this;
			return temp;
		}

		constexpr iterator_type base() const noexcept {
			return current;
		}
	};

	template<typename Iterator>
	constexpr bool operator==(const utf32::iterator<Iterator>& lhs, const utf32::iterator<Iterator>& rhs) noexcept {
		return lhs.base() == rhs.base();
	}

	template<typename Iterator>
	constexpr bool operator!=(const utf32::iterator<Iterator>& lhs, const utf32::iterator<Iterator>& rhs) noexcept {
		return lhs.base() != rhs.base();
	}
}

#endif
