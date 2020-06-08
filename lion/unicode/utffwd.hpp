#ifndef LION_UNICODE_UTFFWD_HPP
#define LION_UNICODE_UTFFWD_HPP

#include "encoding.hpp"

#include <string>
#include <string_view>
#include <istream>
#include <ostream>

namespace lion::unicode
{
	class utf8
	{
	public:
		using string_type	   = std::string;
		using string_view_type = std::string_view;
		using char_type		   = unsigned char;

		template<typename Iterator>
		class iterator;

		template<typename ForwardIterator>
		static constexpr ForwardIterator decode(ForwardIterator first, ForwardIterator last, codepoint& cp);

		template<typename ForwardIterator>
		static constexpr ForwardIterator decode_lenient(ForwardIterator first, ForwardIterator last, codepoint& cp);

		template<typename OutputIterator>
		static constexpr OutputIterator encode(codepoint codepoint, OutputIterator output);

		template<typename ForwardIterator>
		static constexpr std::size_t length(ForwardIterator first, ForwardIterator last);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf8(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf16(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf32(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf8_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf16_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf32_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename OutputIterator>
		static OutputIterator read(std::istream& in, OutputIterator output, byte_order order = byte_order::none);

		template<typename ForwardIterator>
		static ForwardIterator write(std::ostream& out, ForwardIterator first, ForwardIterator last, byte_order order = byte_order::none);

		template<typename ForwardIterator>
		static void write_BOM(std::ostream& out, byte_order order = byte_order::none);

		template<typename ForwardIterator>
		static constexpr ForwardIterator valid_sequence(ForwardIterator first, ForwardIterator last);
	};

	class utf16
	{
	public:
		using string_type	   = std::u16string;
		using string_view_type = std::u16string_view;
		using char_type		   = std::u16string::value_type;

		template<typename Iterator>
		class iterator;

		template<typename ForwardIterator>
		static constexpr ForwardIterator decode(ForwardIterator first, ForwardIterator last, codepoint& cp);

		template<typename ForwardIterator>
		static constexpr ForwardIterator decode_lenient(ForwardIterator first, ForwardIterator last, codepoint& cp);

		template<typename OutputIterator>
		static constexpr OutputIterator encode(codepoint cp, OutputIterator output);

		template<typename ForwardIterator>
		static constexpr std::size_t length(ForwardIterator first, ForwardIterator last);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf8(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf16(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf32(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf8_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf16_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf32_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename OutputIterator>
		static OutputIterator read(std::istream& in, OutputIterator output, byte_order order);

		template<typename ForwardIterator>
		static ForwardIterator write(std::ostream& out, ForwardIterator first, ForwardIterator last, byte_order order);

		template<typename ForwardIterator>
		static void write_BOM(std::ostream& out, byte_order order);

		template<typename ForwardIterator>
		static constexpr ForwardIterator valid_sequence(ForwardIterator first, ForwardIterator last);
	};

	class utf32
	{
	public:
		using string_type      = std::u32string;
		using string_view_type = std::u32string_view;
		using char_type        = std::u32string::value_type;

		template<typename Iterator>
		class iterator;

		template<typename ForwardIterator>
		static constexpr ForwardIterator decode(ForwardIterator first, ForwardIterator last, codepoint& cp);

		template<typename ForwardIterator>
		static constexpr ForwardIterator decode_lenient(ForwardIterator first, ForwardIterator last, codepoint& cp);

		template<typename OutputIterator>
		static constexpr OutputIterator encode(codepoint cp, OutputIterator output);

		template<typename ForwardIterator>
		static constexpr std::size_t length(ForwardIterator first, ForwardIterator last);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf8(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf16(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf32(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf8_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf16_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename ForwardIterator, typename OutputIterator>
		static constexpr OutputIterator to_utf32_lenient(ForwardIterator first, ForwardIterator last, OutputIterator output);

		template<typename OutputIterator>
		static OutputIterator read(std::istream& in, OutputIterator output, byte_order order);

		template<typename ForwardIterator>
		static ForwardIterator write(std::ostream& out, ForwardIterator first, ForwardIterator last, byte_order order);

		template<typename ForwardIterator>
		static void write_BOM(std::ostream& out, byte_order order);

		template<typename ForwardIterator>
		static constexpr ForwardIterator valid_sequence(ForwardIterator first, ForwardIterator last);
	};
}

#endif
