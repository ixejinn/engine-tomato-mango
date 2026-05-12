#ifndef MANGO_TEXT_H
#define MANGO_TEXT_H

#include <string>
#include <glm/vec4.hpp>
#include "Resource/ResourceFwd.h"
#include "Resource/AssetHash.h"
#include "Resource/Render/Font.h"

namespace tomato {
    struct TextComponent {
        /**
		 * @brief The text to be rendered.
		 * @note Use UTF-32 literals with 'U' prefix, e.g., U"Your Text Here".
		 * @todo Planned update:
		 * Modify the write interface to accept standard std::string (UTF-8)
		 * and handle internal conversion to UTF-32 for better usability.
		 */
		std::string text{};
		glm::vec4 color{ 0.0f, 0.0f, 0.0f, 1.0f };
		float fontSize{ 48.f };
		AssetID font{ GetAssetID(Font::PrimitiveName) };

		std::u32string codepoints{};
		bool dirty{ true };
	};
}

#endif //MANGO_TEXT_H