#include <windows.h>
#include <commdlg.h>
#include <string>
#include "FileDialog.h"

namespace tomato::FileDialog
{
	std::optional<std::filesystem::path> OpenFile(const char* title, const char* filter, const std::filesystem::path& initialDir)
	{
		char filePath[MAX_PATH] = "";
		
		OPENFILENAMEA ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = title;
		ofn.lpstrFilter = filter;

		std::string dir = initialDir.string();
		ofn.lpstrInitialDir = dir.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (GetOpenFileNameA(&ofn))
			return std::filesystem::path(filePath);

		return std::nullopt;
	}
	std::optional<std::filesystem::path> SaveFile(const char* title, const char* extension, const char* filter, const std::filesystem::path& initialDir)
	{
		char filePath[260] = "";

		OPENFILENAMEA ofn{};
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = title;
		ofn.lpstrDefExt = extension;
		ofn.lpstrFilter = filter;

		std::string dir = initialDir.string();
		ofn.lpstrInitialDir = dir.c_str();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		if (GetSaveFileNameA(&ofn))
			return std::filesystem::path(filePath);

		return std::nullopt;
	}
}

namespace tomato::FileUtils
{
	bool CopyAsset(const std::filesystem::path& source, const std::filesystem::path& destination)
	{
		try
		{
			if (destination.parent_path().empty())
				std::filesystem::create_directories(destination.parent_path());

			std::filesystem::copy_file(source, destination, std::filesystem::copy_options::none);

			return true;
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			TMT_ERR << e.what();
			return false;
		}
	}
}