#ifndef MANGO_PATHMANAGER_H
#define MANGO_PATHMANAGER_H

#include <string>
#include <filesystem>

namespace tomato
{
	class PathManager
	{
	public:
		static std::filesystem::path ExecutableDir();

		static std::filesystem::path EngineResource();
		static std::filesystem::path ContentResource();

		static std::filesystem::path Font(const std::string& file);
		static std::filesystem::path Icon(const std::string& file);
		static std::filesystem::path Shader(const std::string& file);
		static std::filesystem::path Scene(const std::string& file);
	};
}
#endif // !MANGO_PATHMANAGER_H
