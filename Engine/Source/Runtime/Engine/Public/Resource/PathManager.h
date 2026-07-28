#ifndef MANGO_PATHMANAGER_H
#define MANGO_PATHMANAGER_H

#include <string>
#include <filesystem>

namespace tomato
{
	/*
	* Editor: Project Root
	* Engine: Executable(Runtime) Root
	*/

	class PathManager
	{
	public:
		static std::filesystem::path ExecutableDir();

		static std::filesystem::path GetExecutableDir();

		static std::filesystem::path EngineResource();
		static std::filesystem::path ContentResource();

		static std::filesystem::path ToRuntime(const std::filesystem::path& projectPath);
		static std::filesystem::path ToProject(const std::filesystem::path& runtimePath);

		static std::filesystem::path Font(const std::string& file);
		static std::filesystem::path Icon(const std::string& file);
		static std::filesystem::path Shader(const std::string& file);

		static std::filesystem::path ContentScene(const std::string& file = "");
		static std::filesystem::path ContentImage(const std::string& file = "");
		static std::filesystem::path ContentParticle(const std::string& file = "");
		static std::filesystem::path ContentSound(const std::string& file = "");
		static std::filesystem::path ContentFont(const std::string& file = "");

	};
}
#endif // !MANGO_PATHMANAGER_H
