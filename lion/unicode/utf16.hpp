#ifndef LION_UNICODE_UTF16_HPP
#define LION_UNICODE_UTF16_HPP

#include "utffwd.hpp"
#include "codepoint.hpp"
#include "encoding.hpp"

#include <cstddef>
#include <algorithm>
#include <string>
#include <ostream>
#include <istream>
#include <iterator>

namespace lion::unicode
{
	template<typename ForwardIterator>
	constexpr ForwardIterator utf16::decode(ForwardIterator first, ForwardIterator last, codepoint& cp)
	{
		const char_type one = *first++;
		if (is_high_surrogate(one))
		{
			if (first != last)
			{
				const char_type two = *first;
				if (is_low_surrogate(two)) 
				{
					++first;
					cp = codepoint(((one - 0xD800) << 10) + (two - 0xDC00) + 0x0010000);
				}
				else {
					cp = replacement_character();
				}
			}
			else {
				cp = replacement_character();
			}
		}
		else {
			cp = is_valid(one) ? one : replacement_character();
		}
		return first;
	}

	template<typename ForwardIterator>
	constexpr ForwardIterator utf16::decode_lenient(ForwardIterator first, ForwardIterator last, codepoint& cp)
	{
		if (const char_type one = *first++; is_high_surrogate(one))
		{
			const char_type two = *first++;
			cp = codepoint(((one - 0xD800) << 10) + (two - 0xDC00) + 0x0010000);
		}
		else {
			cp = one;
		}
		return first;
	}

	template<typename OutputIterator>
	constexpr OutputIterator utf16::encode(codepoint cp, OutputIterator output)
	{
		if (cp <= 0xFFFF) {
			*output++ = char_type(cp);
		}
		else
		{
			cp -= 0x0010000;
			*output++ = char_type((cp >> 10) + 0xD800);
			*output++ = char_type((cp & 0x3FF) + 0xDC00);
		}
		return output;
	}

	template<typename ForwardIterator>
	constexpr std::size_t utf16::length(ForwardIterator first, ForwardIterator last)
	{
		std::size_t len = 0;
		while (first != last)
		{
			codepoint cp;
			first = decode_lenient(first, last, cp);
			++len;
		}
		return len;
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf16::to_utf8(ForwardIterator first, ForwardIterator last, OutputIterator output)
	{
		while (first != last)
		{
			codepoint cp;
			first = decode(first, last, cp);
			output = utf8::encode(cp, output);
		}
		return output;
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf16::to_utf16(ForwardIterator first, ForwardIterator last, OutputIterator output) {
		return std::copy(first, last, output);
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf16::to_utf32(ForwardIterator first, ForwardIterator last, OutputIterator output)
	{
		while (first != last)
		{
			codepoint cp;
			first = decode(first, last, cp);
			*output++ = cp;
		}
		return output;
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf16::to_utf8_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output)
	{
		while (first != last)
		{
			codepoint cp;
			first = decode_lenient(first, last, cp);
			output = utf8::encode(cp, output);
		}
		return output;
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf16::to_utf16_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output) {
		return std::copy(first, last, output);
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf16::to_utf32_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output)
	{
		while (first != last)
		{
			codepoint cp;
			first = decode_lenient(first, last, cp);
			*output++ = cp;
		}
		return output;
	}

	template<typename OutputIterator>
	OutputIterator utf16::read(std::istream& in, OutputIterator output, byte_order order)
	{
		if (order == byte_order::none) {
			return output;
		}

		std::string text;
		in.seekg(0, std::ios::end);
		text.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&text[0], text.size());

		if (order == byte_order::little)
		{
			for (std::string::size_type i = 0; i < text.size(); i += 2)
			{
				const char_type bytes[] = {
					static_cast<unsigned char>(text[i + 1]),
					static_cast<unsigned char>(text[i])
				};
				*output++ = (bytes[0] << 8) | bytes[1];
			}
		}
		else if (order == byte_order::big)
		{
			for (std::string::size_type i = 0; i < text.size(); i += 2)
			{
				const char_type bytes[] = {
					static_cast<unsigned char>(text[i]),
					static_cast<unsigned char>(text[i + 1])
				};
				*output++ = (bytes[0] << 8) | bytes[1];
			}
		}
		return output;
	}

	template<typename ForwardIterator>
	ForwardIterator utf16::write(std::ostream& out, ForwardIterator first, ForwardIterator last, byte_order order)
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
					static_cast<char>(*it & 0x00FF),
					static_cast<char>((*it & 0xFF00) >> 8)
				};
				towrite.append(bytes, 2);
			}
		}
		else if (order == byte_order::big)
		{
			for (ForwardIterator it = first; it != last; ++it)
			{
				const char bytes[] = {
					static_cast<char>((*it & 0xFF00) >> 8),
					static_cast<char>(*it & 0x00FF)
				};
				towrite.append(bytes, 2);
			}
		}
		out.write(&towrite[0], towrite.size());
		return std::next(first, out.tellp() / 2);
	}

	template<typename ForwardIterator>
	void utf16::write_BOM(std::ostream& out, byte_order order)
	{
		if (order == byte_order::big) {
			out.write(constants::UTF16_BE_BOM.data(), 2);
		}
		else if (order == byte_order::little) {
			out.write(constants::UTF16_LE_BOM.data(), 2);
		}
	}

	template<typename ForwardIterator>
	constexpr ForwardIterator utf16::valid_sequence(ForwardIterator first, ForwardIterator last)
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
	class utf16::iterator
	{
	public:
		using value_type	    = codepoint;
		using difference_type   = std::ptrdiff_t;
		using pointer		    = void;
		using iterator_category = std::bidirectional_iterator_tag;
		using iterator_type		= Iterator;

		class reference
		{
		private:
			template<typename T>
			friend class utf16::iterator;

			iterator_type current;

		public:
			constexpr reference& operator=(codepoint cp)
			{
				utf16::encode(cp, current);
				return *this;
			}

			constexpr operator codepoint()
			{
				codepoint cp = 0;

				const utf16::char_type one = *current;
				if (one >= 0xD800 && one <= 0xDBFF)
				{
					const utf16::char_type two = *std::next(current);
					cp = codepoint(((one - 0xD800) << 10) + (two - 0xDC00) + 0x0010000);
				}
				else {
					cp = one;
				}

				return cp;
			}
		};

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

		constexpr reference operator*() const noexcept
		{
			reference ref;
			ref.current = current;
			return ref;
		}

		constexpr iterator& operator++() noexcept
		{
			const utf16::char_type one = *current;
			++current;
			if (one >= 0xD800 && one <= 0xDBFF) {
				++current;
			}
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
			const utf16::char_type two = *current;
			if (two >= 0xDC00 && two <= 0xDFFF) {
				--current;
			}
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
	constexpr bool operator==(const utf16::iterator<Iterator>& lhs, const utf16::iterator<Iterator>& rhs) noexcept {
		return lhs.base() == rhs.base();
	}

	template<typename Iterator>
	constexpr bool operator!=(const utf16::iterator<Iterator>& lhs, const utf16::iterator<Iterator>& rhs) noexcept {
		return lhs.base() != rhs.base();
	}
}

#endif
