#include <filesystem>
#include <optional>
#include "Resource/PathManager.h"
#include "Utils/Logger.h"

namespace tomato::FileDialog
{
	std::optional<std::filesystem::path> OpenFile(
		const char* title,
		const char* filter = "All Files (*.*)\0*.*\0\0",
		const std::filesystem::path& initialDir = "Resources/");

	std::optional<std::filesystem::path> SaveFile(
		const char* title,
		const char* extention,
		const char* filter = "All Files (*.*)\0*.*\0\0",
		const std::filesystem::path& initialDir = "Resources/");
}

namespace tomato::FileUtils
{
	bool CopyAsset(const std::filesystem::path& source, const std::filesystem::path& destination);
}