#ifndef LION_UNICODE_UTF8_HPP
#define LION_UNICODE_UTF8_HPP

#include "utffwd.hpp"
#include "codepoint.hpp"
#include "encoding.hpp"

#include <iterator>
#include <algorithm>
#include <cstddef>
#include <string>
#include <istream>
#include <ostream>

namespace lion::unicode
{
	template<typename ForwardIterator>
	constexpr ForwardIterator utf8::decode(ForwardIterator first, ForwardIterator last, codepoint& cp)
	{
		static constexpr unsigned char trailing[256] =
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
		};
		static constexpr codepoint offsets[4] = {
			0x00000000, 0x00003080, 0x000E2080, 0x03C82080
		};

		// the well_formedi functions are based on table 3-7 of the unicode 10.0 standard
		const auto well_formed1 = [](auto byte) {
			return byte >> 7 == 0;
		};
		const auto well_formed2 = [](auto byte1, auto byte2) {
			return (byte1 >= 0xC2 && byte1 <= 0xDF) && (byte2 >= 0x80 && byte2 <= 0xBF);
		};
		const auto well_formed3 = [](auto byte1, auto byte2, auto byte3) 
		{
			return (byte1 == 0xE0 && (byte2 >= 0xA0 && byte2 <= 0xBF) && 
						(byte3 >= 0x80 && byte3 <= 0xBF)) ||
				((byte1 >= 0xE1 && byte1 <= 0xEC) && (byte2 >= 0x80 && byte2 <= 0xBF) && 
					(byte3 >= 0x80 && byte3 <= 0xBF)) ||
				((byte1 == 0xED && byte2 >= 0x80) && (byte2 <= 0x9F && byte3 >= 0x80) && 
					byte3 <= 0xBF) ||
				((byte1 >= 0xEE && byte1 <= 0xEF) && (byte2 >= 0x80 && byte2 <= 0xBF) && 
					(byte3 >= 0x80 && byte3 <= 0xBF));
		};
		const auto well_formed4 = [](auto byte1, auto byte2, auto byte3, auto byte4) 
		{
			return ((byte1 == 0xF0 && byte2 >= 0x90) && (byte2 <= 0xBF && byte3 >= 0x80) && 
						(byte3 <= 0xBF && byte4 >= 0x80) && byte4 <= 0xBF) ||
				((byte1 >= 0xF1 && byte1 <= 0xF3) && (byte2 >= 0x80 && byte2 <= 0xBF) && 
					(byte3 >= 0x80 && byte3 <= 0xBF) && (byte4 >= 0x80 && byte4 <= 0xBF)) ||
				((byte1 == 0xF4 && byte2 >= 0x80) && (byte2 <= 0x8F && byte3 >= 0x80) && 
					(byte3 <= 0xBF && byte4 >= 0x80) && byte4 <= 0xBF);
		};

		const unsigned char extra = trailing[static_cast<unsigned char>(*first)];
		if (std::distance(first, last) >= extra)
		{
			cp = 0;
			if (extra == 0)
			{
				unsigned char byte = static_cast<unsigned char>(*first);
				if (well_formed1(byte))
				{
					cp += byte;
					++first;
				}
				else
				{
					cp = replacement_character();
					return ++first;
				}
			}
			else if (extra == 1)
			{
				unsigned char byte1 = static_cast<unsigned char>(*first);
				if (((byte1 >> 5) | 0b110) == 0b110)
				{
					cp += byte1;
					cp <<= 6;
					++first;

					unsigned char byte2 = static_cast<unsigned char>(*first);
					if (((byte2 >> 6) | 0b10) == 0b10 && well_formed2(byte1, byte2))
					{
						cp += byte2;
						++first;
					}
					else
					{
						cp = replacement_character();
						return first;
					}
				}
				else
				{
					cp = replacement_character();
					return first;
				}
			}
			else if (extra == 2)
			{
				unsigned char byte1 = static_cast<unsigned char>(*first);
				if (((byte1 >> 4) | 0b1110) == 0b1110)
				{
					cp += byte1;
					cp <<= 6;
					++first;

					unsigned char byte2 = static_cast<unsigned char>(*first);
					if (((byte2 >> 6) | 0b10) == 0b10)
					{
						cp += byte2;
						cp <<= 6;
						++first;

						unsigned char byte3 = static_cast<unsigned char>(*first);
						if (((byte2 >> 6) | 0b10) == 0b10 && well_formed3(byte1, byte2, byte3))
						{
							cp += byte3;
							++first;
						}
						else
						{
							cp = replacement_character();
							return first;
						}
					}
					else
					{
						cp = replacement_character();
						return first;
					}
				}
				else
				{
					cp = replacement_character();
					return first;
				}
			}
			else if (extra == 3)
			{
				unsigned char byte1 = static_cast<unsigned char>(*first);
				if (((byte1 >> 3) | 0b11110) == 0b11110)
				{
					cp += byte1;
					cp <<= 6;
					++first;

					unsigned char byte2 = static_cast<unsigned char>(*first);
					if (((byte2 >> 6) | 0b10) == 0b10)
					{
						cp += byte2;
						cp <<= 6;
						++first;

						unsigned char byte3 = static_cast<unsigned char>(*first);
						if (((byte2 >> 6) | 0b10) == 0b10)
						{
							cp += byte3;
							cp <<= 6;
							++first;

							unsigned char byte4 = static_cast<unsigned char>(*first);
							if (((byte2 >> 6) | 0b10) == 0b10 && well_formed4(byte1, byte2, byte3, byte4))
							{
								cp += byte4;
								++first;
							}
							else
							{
								cp = replacement_character();
								return first;
							}
						}
						else
						{
							cp = replacement_character();
							return first;
						}
					}
					else
					{
						cp = replacement_character();
						return first;
					}
				}
				else
				{
					cp = replacement_character();
					return first;
				}
			}
			else
			{
				cp = replacement_character();
				return ++first;
			}
			cp -= offsets[extra];
		}
		else
		{
			first = last;
			cp = replacement_character();
		}

		return first;
	}

	template<typename ForwardIterator>
	constexpr ForwardIterator utf8::decode_lenient(ForwardIterator first, ForwardIterator last, codepoint& cp)
	{
		static constexpr unsigned char trailing[256] =
		{
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
		};
		static constexpr codepoint offsets[4] = {
			0x00000000, 0x00003080, 0x000E2080, 0x03C82080
		};

		const unsigned char extra = trailing[static_cast<unsigned char>(*first)];
		cp = 0;
		switch (extra)
		{
		case 3:
			cp += static_cast<unsigned char>(*first++);
			cp <<= 6;
		case 2:
			cp += static_cast<unsigned char>(*first++);
			cp <<= 6;
		case 1:
			cp += static_cast<unsigned char>(*first++);
			cp <<= 6;
		case 0:
			cp += static_cast<unsigned char>(*first++);
		}
		cp -= offsets[extra];

		return first;
	}

	template<typename OutputIterator>
	constexpr OutputIterator utf8::encode(codepoint codepoint, OutputIterator output)
	{
		static constexpr unsigned char firsts[4] = {
			0x00, 0xC0, 0xE0, 0xF0
		};

		std::size_t nbytes = 0;
		if (codepoint < 0x80) {
			nbytes = 0;
		}
		else if (codepoint < 0x800) {
			nbytes = 1;
		}
		else if (codepoint < 0x10000) {
			nbytes = 2;
		}
		else if (codepoint <= 0x0010FFFF) {
			nbytes = 3;
		}

		unsigned char bytes[4];
		switch (nbytes)
		{
		case 3:
			bytes[3] = static_cast<unsigned char>((codepoint | 0x80) & 0xBF);
			codepoint >>= 6;
		case 2:
			bytes[2] = static_cast<unsigned char>((codepoint | 0x80) & 0xBF);
			codepoint >>= 6;
		case 1:
			bytes[1] = static_cast<unsigned char>((codepoint | 0x80) & 0xBF);
			codepoint >>= 6;
		case 0:
			bytes[0] = static_cast<unsigned char>(codepoint | firsts[nbytes]);
		}
		return std::copy(bytes, bytes + nbytes + 1, output);
	}

	template<typename ForwardIterator>
	constexpr std::size_t utf8::length(ForwardIterator first, ForwardIterator last)
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

	template< typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf8::to_utf8(ForwardIterator first, ForwardIterator last, OutputIterator output) {
		return std::copy(first, last, output);
	}

	template< typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf8::to_utf16(ForwardIterator first, ForwardIterator last, OutputIterator output)
	{
		while (first != last)
		{
			codepoint cp;
			first = decode(first, last, cp);
			output = utf16::encode(cp, output);
		}
		return output;
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf8::to_utf32(ForwardIterator first, ForwardIterator last, OutputIterator output)
	{
		while (first != last)
		{
			codepoint cp;
			first = decode(first, last, cp);
			*output++ = cp;
		}
		return output;
	}

	template< typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf8::to_utf8_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output) {
		return std::copy(first, last, output);
	}

	template< typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf8::to_utf16_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output)
	{
		while (first != last)
		{
			codepoint cp;
			first = decode_lenient(first, last, cp);
			output = utf16::encode(cp, output);
		}
		return output;
	}

	template<typename ForwardIterator, typename OutputIterator>
	constexpr OutputIterator utf8::to_utf32_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output)
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
	OutputIterator utf8::read(std::istream& in, OutputIterator output, byte_order order)
	{
		if (order == byte_order::none)
		{
			std::string text;
			in.seekg(0, std::ios::end);
			text.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&text[0], text.size());
			return std::copy(text.begin(), text.end(), output);
		}
		return output;
	}

	template<typename ForwardIterator>
	ForwardIterator utf8::write(std::ostream& out, ForwardIterator first, ForwardIterator last, byte_order order)
	{
		std::string towrite(first, last);
		out.write(&towrite[0], towrite.size());
		return std::next(first, out.tellp() / 1); 
	}

	template<typename ForwardIterator>
	void utf8::write_BOM(std::ostream& out, byte_order order) {
		out.write(constants::UTF8_BOM.data(), 3);
	}

	template<typename ForwardIterator>
	constexpr ForwardIterator utf8::valid_sequence(ForwardIterator first, ForwardIterator last)
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
	class utf8::iterator
	{
	public:
		using value_type	    = codepoint;
		using difference_type   = std::ptrdiff_t;
		using pointer	        = void;
		using iterator_category = std::bidirectional_iterator_tag;
		using iterator_type		= Iterator;

		class reference
		{
		private:
			template<typename T>
			friend class utf8::iterator;

			iterator_type current;

		public:
			constexpr reference& operator=(codepoint cp)
			{
				utf8::encode(cp, current);
				return *this;
			}

			constexpr operator codepoint()
			{
				codepoint cp = 0;

				const char byte1 = *current;
				if (byte1 & 128)
				{
					if (byte1 & 32)
					{
						if (byte1 & 16)
						{
							const char byte2 = *std::next(current);
							const char byte3 = *std::next(current, 2);
							const char byte4 = *std::next(current, 3);

							cp = (byte1 & 0x07) << 18;
							cp += (byte2 & 0x3f) << 12;
							cp += (byte3 & 0x3f) << 6;;
							cp += (byte4 & 0x3f);
						}
						else
						{
							const char byte2 = *std::next(current);
							const char byte3 = *std::next(current, 2);

							cp = (byte1 & 0x0f) << 12;
							cp += (byte2 & 0x3f) << 6;
							cp += (byte3 & 0x3f);
						}
					}
					else
					{
						const char byte2 = *std::next(current);

						cp = (byte1 & 0x1f) << 6;
						cp += (byte2 & 0x3f);
					}
				}
				else {
					cp = byte1;
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
			const char byte1 = *current;
			if (byte1 & 128)
			{
				if (byte1 & 32)
				{
					if (byte1 & 16) {
						std::advance(current, 4);
					}
					else {
						std::advance(current, 3);
					}
				}
				else {
					std::advance(current, 2);
				}
			}
			else {
				std::advance(current, 1);
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
			if (*current & 128)
			{
				--current;
				if ((*current & 64) == 0)
				{
					--current;
					if ((*current & 64) == 0) {
						--current;
					}
				}
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
	constexpr bool operator==(const utf8::iterator<Iterator>& lhs, const utf8::iterator<Iterator>& rhs) noexcept {
		return lhs.base() == rhs.base();
	}

	template<typename Iterator>
	constexpr bool operator!=(const utf8::iterator<Iterator>& lhs, const utf8::iterator<Iterator>& rhs) noexcept {
		return lhs.base() != rhs.base();
	}
}

#endif
