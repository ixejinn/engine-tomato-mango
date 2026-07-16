#include "Resource/PathManager.h"

namespace tomato
{
	std::filesystem::path PathManager::ExecutableDir()
	{
		return "Resources";
	}
	std::filesystem::path PathManager::EngineResource()
	{
		return ExecutableDir() / "Engine";
	}
	std::filesystem::path PathManager::ContentResource()
	{
		return ExecutableDir() / "Contents";
	}
	std::filesystem::path PathManager::Font(const std::string& file)
	{
		return EngineResource() / "Assets" / "Fonts" / file;
	}
	std::filesystem::path PathManager::Icon(const std::string& file)
	{
		return EngineResource() / "Assets" / "icon" / file;
	}
	std::filesystem::path PathManager::Shader(const std::string& file)
	{
		return EngineResource() / "Shaders" / file;
	}
	std::filesystem::path PathManager::Scene(const std::string& file)
	{
		return ContentResource() / "Scenes" / file;
	}
}