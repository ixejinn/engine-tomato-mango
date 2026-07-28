#include <windows.h>
#include <commdlg.h>

#include "Resource/PathManager.h"

namespace tomato
{
	std::filesystem::path PathManager::ExecutableDir()
	{
		return "Resources";
	}

	std::filesystem::path PathManager::GetExecutableDir()
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(nullptr, buffer, MAX_PATH);

		return std::filesystem::path(buffer).parent_path();
	}

	std::filesystem::path PathManager::EngineResource()
	{
		return ExecutableDir() / "Engine";
	}
	std::filesystem::path PathManager::ContentResource()
	{
		return ExecutableDir() / "Contents";
	}

	std::filesystem::path PathManager::ToRuntime(const std::filesystem::path& projectPath)
	{
		auto relative =
			std::filesystem::relative(projectPath, "Contents\\Resouces");

		return ContentResource() / relative;
	}

	std::filesystem::path PathManager::ToProject(const std::filesystem::path& runtimePath)
	{
		auto project =
			std::filesystem::

	}

	std::filesystem::path PathManager::Font(const std::string& file)
	{
		return EngineResource() / "Assets" / "Fonts" / file;
	}
	std::filesystem::path PathManager::Icon(const std::string& file)
	{
		return EngineResource() / "Assets" / "Icon" / file;
	}
	std::filesystem::path PathManager::Shader(const std::string& file)
	{
		return EngineResource() / "Shaders" / file;
	}
	std::filesystem::path PathManager::ContentScene(const std::string& file)
	{
		return ContentResource() / "Scenes" / file;
	}
	std::filesystem::path PathManager::ContentImage(const std::string& file)
	{
		return ContentResource() / "Img" / file;
	}
	std::filesystem::path PathManager::ContentParticle(const std::string& file)
	{
		return ContentResource() / "Particle" / file;
	}
	std::filesystem::path PathManager::ContentSound(const std::string& file)
	{
		return ContentResource() / "Sound" / file;
	}
	std::filesystem::path PathManager::ContentFont(const std::string& file)
	{
		return ContentResource() / "Fonts" / file;
	}
}