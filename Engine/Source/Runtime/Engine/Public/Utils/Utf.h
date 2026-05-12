#ifndef MANGO_UTF_H
#define MANGO_UTF_H

#include <string>

namespace tomato
{
	std::u32string UTF8ToUTF32(std::string_view utr8str);
}

#endif // !MANGO_UTF_H
