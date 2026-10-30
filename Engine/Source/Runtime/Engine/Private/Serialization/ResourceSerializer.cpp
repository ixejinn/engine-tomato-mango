#include "Serialization/ResourceSerializer.h"

#include "Resource/AssetRegistry.h"
#include "Resource/Render/Font.h"
#include "Resource/Render/Mesh.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/ParticleEffect.h"

namespace tomato::Serialization
{
	void ResourceSerializer::SaveResources(json& root)
	{
		root["Resource"] = json::array();

		json font, mesh, tex, particle;

		auto itBegin = AssetRegistry<Font>::GetInstance().GetNameMapBegin();
		auto itEnd = AssetRegistry<Font>::GetInstance().GetNameMapEnd();

		for (itBegin; itBegin != itEnd; ++itBegin)
			font["Font"].push_back(itBegin->second);

		root["Resource"].push_back(font);

		itBegin = AssetRegistry<Mesh>::GetInstance().GetNameMapBegin();
		itEnd = AssetRegistry<Mesh>::GetInstance().GetNameMapEnd();

		for (itBegin; itBegin != itEnd; ++itBegin)
		{
			bool flag = false;
			for (auto meta : Mesh::PrimitiveMetas)
			{
				if (Mesh::GetPrimitiveName(meta.primitive) == itBegin->second)
				{
					flag = true;
					break;
				}
			}

			if (!flag)
				mesh["Mesh"].push_back(itBegin->second);
		}

		root["Resource"].push_back(mesh);

		itBegin = AssetRegistry<Texture>::GetInstance().GetNameMapBegin();
		itEnd = AssetRegistry<Texture>::GetInstance().GetNameMapEnd();

		for (itBegin; itBegin != itEnd; ++itBegin)
			tex["Texture"].push_back(itBegin->second);

		root["Resource"].push_back(tex);

		itBegin = AssetRegistry<ParticleEffect>::GetInstance().GetNameMapBegin();
		itEnd = AssetRegistry<ParticleEffect>::GetInstance().GetNameMapEnd();

		for (itBegin; itBegin != itEnd; ++itBegin)
			particle["Particle"].push_back(itBegin->second);

		root["Resource"].push_back(particle);
	}

	void ResourceSerializer::LoadResources(const json& root)
	{
		for (auto& src : root["Resource"])
		{
			//std::cout << src.items().begin().key() << '\n';
			if (src.items().begin().key() == "Font")
			{
				for (auto& font : src["Font"])
					Font::Create(font);
			}

			if (src.items().begin().key() == "Mesh")
			{
				for (auto& mesh : src["Mesh"])
				{
					std::string meshName{ mesh };

					auto first = meshName.find("_");
					auto second = meshName.find("_", first + 1);

					auto sector = meshName.substr(first + 1, second - first - 1);
					auto stack = meshName.substr(second + 1);

					auto type = meshName.substr(0, first);
					for (auto meta : Mesh::PrimitiveMetas)
					{
						if (Mesh::GetPrimitiveName(meta.primitive) == type)
						{
							Mesh::Create(meta.primitive, std::stoi(sector), std::stoi(stack));
							break;
						}
					}
				}
			}

			if (src.items().begin().key() == "Texture")
			{
				for (auto& tex : src["Texture"])
					Texture::Create(tex);
			}

			if (src.items().begin().key() == "Particle")
			{
				for (auto& particle : src["Particle"])
					ParticleEffect::Create(particle);
			}
		}
	}
}