//
// Created by yejin on 26. 10. 23..
//#include <array>
#include "PlayTest/RenderTestState.h"
#include "ECS/Components/Components.h"
#include "Resource/Render/Shader.h"
#include "Resource/Render/Texture.h"
#include "Resource/Render/Mesh.h"
#include "Resource/AssetHash.h"
#include "Prefab/Prefab.h"
#include "Render/GLStateCache.h"
#include "Input/KeyDeviceState.h"
#include "Utils/RandomNumberGenerator.h"
#include "Utils/RegistryEntry.h"
REGISTER_STATE(RenderTestState)

using namespace tomato;

void RenderTestState::Init()
{
    Shader::Create("Resources/Engine/Shaders/shader.vs", "Resources/Contents/Shaders/red.fs");
    Shader::Create("Resources/Engine/Shaders/shader.vs", "Resources/Contents/Shaders/blue.fs");
    Shader::Create("Resources/Engine/Shaders/shader.vs", "Resources/Contents/Shaders/green.fs");

    Texture::Create(PathManager::ProjectImage("apple.jpg"));
    Texture::Create(PathManager::ProjectImage("greenapple.png"));
    Texture::Create(PathManager::ProjectImage("heart.png"));

    constexpr int testCnt = 4;
    std::array<AssetID, testCnt> shaders
    {
        GetAssetID(Shader::PrimitiveName),
        GetAssetID("Resources/Contents/Shaders/red.fs"),
        GetAssetID("Resources/Contents/Shaders/blue.fs"),
        GetAssetID("Resources/Contents/Shaders/green.fs")
    };

    std::array<AssetID, testCnt> textures
    {
        GetAssetID(Texture::PrimitiveName),
        GetAssetID("Resources\\Contents\\Img\\apple.jpg"),
        GetAssetID("Resources\\Contents\\Img\\greenapple.png"),
        GetAssetID("Resources\\Contents\\Img\\heart.png")
    };

    std::array<AssetID, testCnt> meshes
    {
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cube)),
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cone)),
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Sphere)),
        GetAssetID(Mesh::GetPrimitiveName(Mesh::Primitive::Cylinder))
    };

    for (int x = 0; x < 20; x += 2)
    {
        for (int y = 0; y < 20; y += 2)
        {
            for (int z = 0; z < 20; z += 2)
            {
                entt::entity e = Prefab::CreateStaticMesh(registry_, true, "obj");
                auto& trf = registry_.get<TransformComponent>(e);
                trf.SetPosition(x, y, z);

                int shader = RandomNumberGenerator::GetUniformIntDistribution(0, testCnt - 1);
                int texture = RandomNumberGenerator::GetUniformIntDistribution(0, testCnt - 1);
                int mesh = RandomNumberGenerator::GetUniformIntDistribution(0, testCnt - 1);

                auto& render = registry_.get<RenderComponent>(e);
                render.shader = shaders[shader];
                render.texture = textures[texture];
                render.mesh = meshes[mesh];
            }
        }
    }
}

void RenderTestState::Update()
{
    // if (KeyDeviceState::GetInstance().IsKeyPressed(Key::P))
    //     GLStateCache::PrintLastBindCallCounts();
}

void RenderTestState::Exit()
{

}