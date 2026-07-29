
#include <iostream>
#include "Resource/PathManager.h"

namespace tomato
{
	std::filesystem::path PathManager::projectRoot_;

	void PathManager::SetProjectRoot(const std::filesystem::path& path)
	{
		projectRoot_ = path;
		projectRoot_.make_preferred(); // the path foo/bar will be converted to foo\bar.
	}

	const std::filesystem::path& PathManager::ProjectRoot()
	{
		return projectRoot_;
	}

	std::filesystem::path PathManager::EngineResource()
	{
		return RuntimeResource() / "Engine";
	}
	std::filesystem::path PathManager::ContentResource()
	{
		return RuntimeResource() / "Contents";
	}

	std::filesystem::path PathManager::ToRuntime(const std::filesystem::path& projectPath)
	{
		auto relative =
			std::filesystem::relative(projectPath, projectRoot_ / "Contents\\Resources");

		return ContentResource() / relative;
	}

	std::filesystem::path PathManager::ToProject(const std::filesystem::path& runtimePath)
	{
		auto project =
			std::filesystem::relative(runtimePath, "Resources\\Contents");
		return ProjectRoot() / "Contents\\Resources" / project;
	}

	std::filesystem::path PathManager::RuntimeFont(const std::string& file)
	{
		return EngineResource() / "Assets" / "Fonts" / file;
	}
	std::filesystem::path PathManager::RuntimeIcon(const std::string& file)
	{
		return EngineResource() / "Assets" / "Icon" / file;
	}
	std::filesystem::path PathManager::RuntimeShader(const std::string& file)
	{
		return EngineResource() / "Shaders" / file;
	}

	std::filesystem::path PathManager::RuntimeResource()
	{
		return "Resources";
	}

	std::filesystem::path PathManager::ProjectResource()
	{
		return projectRoot_ / "Contents\\Resources";
	}

	std::filesystem::path PathManager::ProjectFont(const std::string& file)
	{
		return ToRuntime(ProjectResource() / "Fonts" / file);
	}

	std::filesystem::path PathManager::ProjectImage(const std::string& file)
	{
		return ToRuntime(ProjectResource() / "Img" / file);
	}

	std::filesystem::path PathManager::ProjectSound(const std::string& file)
	{
		return ToRuntime(ProjectResource() / "Sound" / file);
	}

	std::filesystem::path PathManager::ProjectParticle(const std::string& file)
	{
		return ToRuntime(ProjectResource() / "Particle" / file);
	}
}