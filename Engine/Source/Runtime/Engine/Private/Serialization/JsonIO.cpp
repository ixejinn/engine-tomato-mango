#include <fstream>

#include "Serialization/JsonIO.h"
#include "Utils/Logger.h"

namespace tomato::Serialization
{
	void CreateJsonFile(const char* path)
	{
		std::ofstream ofs(path);
		ofs.close();
	}

	json LoadJsonData(const char* path)
	{
		std::ifstream jf(path);
		if (!jf.is_open())
		{
			TMT_INFO << "FILE NOT FOUND " << path;
			CreateJsonFile(path);

			return nullptr;
		}

		json data;
		jf.seekg(0, std::ios::end);

		if (jf.tellg() != 0)
		{
			jf.seekg(0);
			jf >> data;
		}

		jf.close();

		return data;
	}
}