#include "../third/ConvertUTF/ConvertUTF.h"


template <typename T>
struct ConvertTrait {
	typedef T ArgType;
};
template <>
struct ConvertTrait<char> {
	typedef UTF8 ArgType;
};
template <>
struct ConvertTrait<char16_t> {
	typedef UTF16 ArgType;
};
template <>
struct ConvertTrait<char32_t> {
	typedef UTF32 ArgType;
};

template <typename From, typename To, typename FromTrait = ConvertTrait<From>, typename ToTrait = ConvertTrait<To>>
bool utfConvert(
	const std::basic_string<From>& from, std::basic_string<To>& to,
	ConversionResult(*cvtfunc)(const typename FromTrait::ArgType**, const typename FromTrait::ArgType*,
		typename ToTrait::ArgType**, typename ToTrait::ArgType*,
		ConversionFlags)
)
{
	static_assert(sizeof(From) == sizeof(typename FromTrait::ArgType), "Error size mismatched");
	static_assert(sizeof(To) == sizeof(typename ToTrait::ArgType), "Error size mismatched");

	if (from.empty())
	{
		to.clear();
		return true;
	}

	// See: http://unicode.org/faq/utf_bom.html#gen6
	static const int most_bytes_per_character = 4;

	const size_t maxNumberOfChars = from.length(); // all UTFs at most one element represents one character.
	const size_t numberOfOut = maxNumberOfChars * most_bytes_per_character / sizeof(To);

	std::basic_string<To> working(numberOfOut, 0);

	auto inbeg = reinterpret_cast<const typename FromTrait::ArgType*>(&from[0]);
	auto inend = inbeg + from.length();


	auto outbeg = reinterpret_cast<typename ToTrait::ArgType*>(&working[0]);
	auto outend = outbeg + working.length();
	auto r = cvtfunc(&inbeg, inend, &outbeg, outend, strictConversion);
	if (r != conversionOK)
		return false;

	working.resize(reinterpret_cast<To*>(outbeg) - &working[0]);
	to = std::move(working);

	return true;
};


bool UTF8ToUTF16(const std::string& utf8, std::u16string& outUtf16)
{
	return utfConvert(utf8, outUtf16, ConvertUTF8toUTF16);
}

bool UTF8ToUTF32(const std::string& utf8, std::u32string& outUtf32)
{
	return utfConvert(utf8, outUtf32, ConvertUTF8toUTF32);
}

bool UTF16ToUTF8(const std::u16string& utf16, std::string& outUtf8)
{
	return utfConvert(utf16, outUtf8, ConvertUTF16toUTF8);
}

bool UTF16ToUTF32(const std::u16string& utf16, std::u32string& outUtf32)
{
	return utfConvert(utf16, outUtf32, ConvertUTF16toUTF32);
}

bool UTF32ToUTF8(const std::u32string& utf32, std::string& outUtf8)
{
	return utfConvert(utf32, outUtf8, ConvertUTF32toUTF8);
}

bool UTF32ToUTF16(const std::u32string& utf32, std::u16string& outUtf16)
{
	return utfConvert(utf32, outUtf16, ConvertUTF32toUTF16);
}
