#ifndef LION_UNICODE_UTILITIES_HPP
#define LION_UNICODE_UTILITIES_HPP

#include "utf8.hpp"
#include "utf16.hpp"
#include "utf32.hpp"
#include "encoding.hpp"

#include <string_view>
#include <type_traits>
#include <istream>
#include <ostream>

namespace lion::unicode
{		
	using default_utf = utf8;
	inline constexpr byte_order default_byte_order = byte_order::big;

	template<typename UTF = default_utf>
	constexpr typename UTF::string_type convert(const utf8::string_type& str)
	{
		typename UTF::string_type res;

		if constexpr(std::is_same_v<UTF, utf8>) {
			utf8::to_utf8_lenient(str.begin(), str.end(), std::back_inserter(res));
		}
		else if constexpr(std::is_same_v<UTF, utf16>) {
			utf8::to_utf16_lenient(str.begin(), str.end(), std::back_inserter(res));
		}
		else if constexpr(std::is_same_v<UTF, utf32>) {
			utf8::to_utf32_lenient(str.begin(), str.end(), std::back_inserter(res));
		}
		return res;
	}

	template<typename UTF = default_utf>
	constexpr typename UTF::string_type convert(const utf16::string_type& str)
	{
		typename UTF::string_type res;

		if constexpr(std::is_same_v<UTF, utf8>) {
			utf16::to_utf8_lenient(str.begin(), str.end(), std::back_inserter(res));
		}
		else if constexpr(std::is_same_v<UTF, utf16>) {
			utf16::to_utf16_lenient(str.begin(), str.end(), std::back_inserter(res));
		}
		else if constexpr(std::is_same_v<UTF, utf32>) {
			utf16::to_utf32_lenient(str.begin(), str.end(), std::back_inserter(res));
		}
		return res;
	}

	template<typename UTF = default_utf>
	constexpr typename UTF::string_type convert(const utf32::string_type& str)
	{
		typename UTF::string_type res;

		if constexpr(std::is_same_v<UTF, utf8>) {
			utf32::to_utf8_lenient(str.begin(), str.end(), std::back_inserter(res));
		}
		else if constexpr(std::is_same_v<UTF, utf16>) {
			utf32::to_utf16_lenient(str.begin(), str.end(), std::back_inserter(res));
		}
		else if constexpr(std::is_same_v<UTF, utf32>) {
			utf32::to_utf32_lenient(str.begin(), str.end(), std::back_inserter(res));
		}
		return res;
	}

	template<typename UTF = default_utf>
	inline typename UTF::string_type read_file(std::istream& in)
	{
		typename UTF::string_type str;
		
		const encoding encoding = encoding::get(in);

		if constexpr(std::is_same_v<UTF, utf8>)
		{
			switch (encoding.format)
			{
			case format::utf32:
			{
				utf32::string_type u32;
				utf32::read(in, std::back_inserter(u32), encoding.order);
				utf32::to_utf8_lenient(u32.begin(), u32.end(), std::back_inserter(str));
				break;
			}
			case format::utf16:
			{
				utf16::string_type u16;
				utf16::read(in, std::back_inserter(u16), encoding.order);
				utf16::to_utf8_lenient(u16.begin(), u16.end(), std::back_inserter(str));
				break;
			}
			case format::unknown: // assume ASCII, same as utf8 - only one byte
			case format::utf8:
			{
				utf8::read(in, std::back_inserter(str), encoding.order);
				break;
			}
			}
		}
		else if constexpr(std::is_same_v<UTF, utf16>)
		{
			switch (encoding.format)
			{
			case format::utf32:
			{
				utf32::string_type u32;
				utf32::read(in, std::back_inserter(u32), encoding.order);
				utf32::to_utf16_lenient(u32.begin(), u32.end(), std::back_inserter(str));
				break;
			}
			case format::utf16:
			{
				utf16::read(in, std::back_inserter(str), encoding.order);
				break;
			}
			case format::unknown: // assume ASCII, same as utf8 - only one byte
			case format::utf8:
			{
				utf8::string_type u8;
				utf8::read(in, std::back_inserter(u8), encoding.order);
				utf8::to_utf16_lenient(u8.begin(), u8.end(), std::back_inserter(str));
				break;
			}
			}
		}
		else if constexpr(std::is_same_v<UTF, utf32>)
		{
			switch (encoding.format)
			{
			case format::utf32:
			{
				utf32::read(in, std::back_inserter(str), encoding.order);
				break;
			}
			case format::utf16:
			{
				utf16::string_type u16;
				utf16::read(in, std::back_inserter(u16), encoding.order);
				utf16::to_utf32_lenient(u16.begin(), u16.end(), std::back_inserter(str));
				break;
			}
			case format::unknown: // assume ASCII, same as utf8 - only one byte
			case format::utf8:
			{
				utf8::string_type u8;
				utf8::read(in, std::back_inserter(u8), encoding.order);
				utf8::to_utf32_lenient(u8.begin(), u8.end(), std::back_inserter(str));
				break;
			}
			}
		}

		return str;
	}

	template<typename UTFO = default_utf>
	inline void write_file(std::ostream& file, const utf8::string_type& text, byte_order order = byte_order::none)
	{
		if constexpr(std::is_same_v<UTFO, utf8>) 
		{
			utf8::write_BOM(file);
			utf8::write(file, text.begin(), text.end(), order);
		}
		else if constexpr(std::is_same_v<UTFO, utf16>)
		{
			utf16::string_type u16;
			utf8::to_utf16_lenient(text.begin(), text.end(), std::back_inserter(u16));
			utf16::write_BOM(file, order);
			utf16::write(file, u16.begin(), u16.end(), order);
		}
		else if constexpr(std::is_same_v<UTFO, utf32>)
		{
			utf32::string_type u32;
			utf8::to_utf32_lenient(text.begin(), text.end(), std::back_inserter(u32));
			utf32::write_BOM(file, order);
			utf32::write(file, u32.begin(), u32.end(), order);
		}
	}

	template<typename UTFO = default_utf>
	inline void write_file(std::ostream& file, const utf16::string_type& text, byte_order order = default_byte_order)
	{
		if constexpr(std::is_same_v<UTFO, utf8>)
		{
			utf8::string_type u8;
			utf16::to_utf8_lenient(text.begin(), text.end(), std::back_inserter(u8));
			utf8::write_BOM(file);
			utf8::write(file, u8.begin(), u8.end(), order);
		}
		else if constexpr(std::is_same_v<UTFO, utf16>) 
		{
			utf16::write_BOM(file, order);
			utf16::write(file, text.begin(), text.end(), order);
		}
		else if constexpr(std::is_same_v<UTFO, utf32>)
		{
			utf32::string_type u32;
			utf16::to_utf32_lenient(text.begin(), text.end(), std::back_inserter(u32));
			utf32::write_BOM(file, order);
			utf32::write(file, u32.begin(), u32.end(), order);
		}
	}

	template<typename UTFO = default_utf>
	void write_file(std::ostream& file, const utf32::string_type& text, byte_order order = default_byte_order)
	{
		if constexpr(std::is_same_v<UTFO, utf8>)
		{
			utf8::string_type u8;
			utf32::to_utf8_lenient(text.begin(), text.end(), std::back_inserter(u8));
			utf8::write_BOM(file);
			utf8::write(file, u8.begin(), u8.end(), order);
		}
		else if constexpr(std::is_same_v<UTFO, utf16>)
		{
			utf16::string_type u16;
			utf32::to_utf16(text.begin(), text.end(), std::back_inserter(u16));
			utf16::write_BOM(file, order);
			utf16::write(file, u16.begin(), u16.end(), order);
		}
		else if constexpr(std::is_same_v<UTFO, utf32>) 
		{
			utf32::write_BOM(file, order);
			utf32::write(file, text.begin(), text.end(), order);
		}
	}

	inline auto make_iterator(const utf8::string_type::iterator& it) {
		return utf8::iterator<utf8::string_type::iterator>(it);
	}

	inline auto make_iterator(const utf16::string_type::iterator& it) {
		return utf16::iterator<utf16::string_type::iterator>(it);
	}

	inline auto make_iterator(const utf32::string_type::iterator& it) {
		return utf32::iterator<utf32::string_type::iterator>(it);
	}

	constexpr auto make_iterator(const utf8::string_view_type::iterator& it) {
		return utf8::iterator<utf8::string_view_type::iterator>(it);
	}

	constexpr auto make_iterator(const utf16::string_view_type::iterator& it) {
		return utf16::iterator<utf16::string_view_type::iterator>(it);
	}

	constexpr auto make_iterator(const utf32::string_view_type::iterator& it) {
		return utf32::iterator<utf32::string_view_type::iterator>(it);
	}
}

#endif
