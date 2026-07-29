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
		// Set, Get current project root
		static void SetProjectRoot(const std::filesystem::path&);
		static const std::filesystem::path& ProjectRoot();

		// For Engine
		static std::filesystem::path ToRuntime(const std::filesystem::path& projectPath);
		static std::filesystem::path ToProject(const std::filesystem::path& runtimePath);
		
		static std::filesystem::path ContentResource(); //runtime - contents

		static std::filesystem::path RuntimeResource();
		static std::filesystem::path EngineResource();
		static std::filesystem::path RuntimeFont(const std::string& file);
		static std::filesystem::path RuntimeIcon(const std::string& file);
		static std::filesystem::path RuntimeShader(const std::string& file);

		// For user: user uses project root, engine converts to runtime root
		static std::filesystem::path ProjectResource();
		static std::filesystem::path ProjectFont(const std::string& file = "");
		static std::filesystem::path ProjectImage(const std::string& file = "");
		static std::filesystem::path ProjectSound(const std::string& file = "");
		static std::filesystem::path ProjectParticle(const std::string& file = "");

	private:
		static std::filesystem::path projectRoot_;
	};
}
#endif // !MANGO_PATHMANAGER_H
