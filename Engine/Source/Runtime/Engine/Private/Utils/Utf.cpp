#include "Utils/Utf.h"
#include <cstdint>

/**
* @brief Convert UTF-8 string to UTF-32 (codepoints)
* 
* @param input UTF-8 encoded string
* 
* @return UTF-32 string (each element is a Unicode codepoint)
* 
* @note
* - Decodes UTF-8 byte sequence into Unicode codepoints.
* - Invalid sequences are replaced with U+FFFD (�).
* - Designed for real-time usage (UI/Text rendering).
*/
namespace tomato
{
	std::u32string UTF8ToUTF32(std::string_view input)
	{
		std::u32string result;
		result.reserve(input.size());

		// Use uint8_t to avoid signed char issues during bit operations.
		const uint8_t* s = reinterpret_cast<const uint8_t*>(input.data());
		size_t i = 0;
		const size_t size = input.size();

		while (i < size)
		{
			uint32_t codepoint = 0;
			uint8_t c = s[i];

			// -------------------------
			// 1-byte (ASCII)
			// 0xxxxxxx
			// -------------------------
			if (c < 0x80)
			{
				// Direct mapping for ASCII
				codepoint = c;
				i += 1;
			}

			// -------------------------
			// 2-byte sequence
			// 110xxxxx 10xxxxxx
			// -------------------------
			else if ((c >> 5) == 0x6)
			{
				// Check if enough bytes remain
				if (i + 1 >= size) goto invalid;

				uint8_t c1 = s[i + 1];

				// Validate continuation byte (must be 10xxxxxx)
				if ((c1 & 0xC0) != 0x80) goto invalid;

				// Extract bits and combine
				codepoint = ((c & 0x1F) << 6) |
					(c1 & 0x3F);

				// Overlong encoding check
				if (codepoint < 0x80) goto invalid; //overlong

				i += 2;
			}

			// -------------------------
			// 3-byte sequence
			// 1110xxxx 10xxxxxx 10xxxxxx
			// -------------------------
			else if ((c >> 4) == 0xE)
			{
				if (i + 2 >= size) goto invalid;

				uint8_t c1 = s[i + 1];
				uint8_t c2 = s[i + 2];

				// Validate continuation bytes
				if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80)
					goto invalid;

				codepoint = ((c & 0x0F) << 12) |
					((c1 & 0x3F) << 6) |
					(c2 & 0x3F);

				// Overlong check
				if (codepoint < 0x800) goto invalid;

				// Surrogate range check (invalid in UTF-8)
				if (codepoint >= 0xD800 && codepoint <= 0xDFFF)
					goto invalid;

				i += 3;
			}

			// -------------------------
			// 4-byte sequence
			// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			// -------------------------
			else if ((c >> 3) == 0x1E)
			{
				if (i + 3 >= size) goto invalid;

				uint8_t c1 = s[i + 1];
				uint8_t c2 = s[i + 2];
				uint8_t c3 = s[i + 3];

				if ((c1 & 0xC0) != 0x80 ||
					(c2 & 0xC0) != 0x80 ||
					(c3 & 0xC0) != 0x80) goto invalid;

				codepoint = ((c & 0x07) << 18) |
					((c1 & 0x3F) << 12) |
					((c2 & 0x3F) << 6) |
					(c3 & 0x3F);

				// Valid Unicode range check
				if (codepoint < 0x10000 || codepoint > 0x10FFFF)
					goto invalid;

				i += 4;
			}
			else // Invalid leading byte
				goto invalid;

			// Valid codepoint
			result.push_back(codepoint);
			continue;

		invalid:
			result.push_back(0xFFFD); // Replace invalid sequence with U+FFFD (�)
			i += 1; // Skip one byte and continue parsing
		}

		return result;
	}
}
