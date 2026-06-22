set(THIRD_PARTY_SOURCES
        Source/ThirdParty/glad/src/glad.c
        Source/ThirdParty/miniaudio/miniaudio.c
)

set(ENGINE_HEADERS
        Source/Runtime/Engine/Public/State/State.h
        Source/Runtime/Core/Public/Containers/EnumArray.h
        Source/Runtime/Core/Private/Collision/ColliderSupport.h
        Source/Runtime/Engine/Public/ECS/PhysCompFwd.h
        Source/Runtime/Core/Public/Collision/CollisionConstants.h
        Source/Runtime/Engine/Public/Collision/CollisionEvent.h
        Source/Runtime/Engine/Public/Collision/CollisionLayerMatrix.h
        Source/Runtime/Engine/Public/Utils/BitmaskOperators.h
        Source/Runtime/Core/Public/BitmaskFwd.h
        Source/Runtime/Engine/Public/ECS/Components/Collision.h
        Source/Runtime/Core/Private/Containers/MemoryPool.h
        Source/Runtime/Core/Public/Utils/Logger.h
        Source/Runtime/Engine/Public/Containers/RingArray.h
        Source/Runtime/Core/Private/Containers/SPSCQueue.h
        Source/Runtime/Engine/Public/Containers/Timeline.h
        Source/Runtime/Engine/Public/Network/NetworkConfig.h
        Source/Runtime/Engine/Public/ECS/Components/Camera.h
        Source/Runtime/Engine/Public/ECS/Components/Movement.h
        Source/Runtime/Engine/Public/ECS/Components/Render.h
        Source/Runtime/Engine/Public/Resource/ResourceFwd.h
        Source/Runtime/Engine/Public/ECS/Components/Rigidbody.h
        Source/Runtime/Engine/Public/ECS/Components/Text.h
        Source/Runtime/Engine/Public/Resource/AssetHash.h
        Source/Runtime/Engine/Public/Resource/Render/Font.h
        Source/Runtime/Engine/Public/ECS/Components/Transform.h
        Source/Runtime/Engine/Public/ECS/Components/UI.h
        Source/Runtime/Engine/Public/ECS/Components/UIEvents.h
        Source/Runtime/Engine/Public/ECS/Systems/CameraSystem.h
        Source/Runtime/Engine/Public/ECS/Systems/System.h
        Source/Runtime/Engine/Public/Engine.h
        Source/Runtime/Engine/Public/SimulationConfig.h
        Source/Runtime/Engine/Public/Services/Window.h
        Source/Runtime/Engine/Public/Services/ServiceFwd.h
        Source/Runtime/Engine/Public/Event/EventDispatcher.h
        Source/Runtime/Engine/Public/Event/EventSignal.h
        Source/Runtime/Engine/Public/Input/InputConstants.h
        Source/Runtime/Engine/Public/Input/InputRecord.h
        Source/Runtime/Engine/Public/Input/InputRecorder.h
        Source/Runtime/Engine/Public/Input/InputEvent.h
        Source/Runtime/Engine/Public/Input/InputUI.h
        Source/Runtime/Engine/Public/Input/KeyConstants.h
        Source/Runtime/Engine/Public/Input/InputEventFwd.h
        Source/Runtime/Engine/Public/Services/Input.h
        Source/Runtime/Engine/Public/Tick/TickClock.h
        Source/Runtime/Engine/Public/Tick/TickFwd.h
        Source/Runtime/Engine/Public/State/DefaultState.h
        Source/Runtime/Engine/Public/GameplayConfig.h
        Source/Runtime/Engine/Public/State/StateFwd.h
        Source/Runtime/Engine/Public/ECS/SystemUpdateContexts.h
        Source/Runtime/Engine/Public/ECS/Systems/CollisionSystem.h
        Source/Runtime/Engine/Public/Collision/CollisionEventFwd.h
        Source/Runtime/Engine/Public/ECS/Systems/KinematicMovementSystem.h
        Source/Runtime/Engine/Public/ECS/Systems/RenderSystem.h
        Source/Runtime/Engine/Public/Resource/Render/Mesh.h
        Source/Runtime/Engine/Public/Resource/AssetRegistry.h
        Source/Runtime/Engine/Public/Font/TextRenderer.h
        Source/Runtime/Engine/Public/Font/TextureAtlas.h
        Source/Runtime/Engine/Public/Font/AtlasManager.h
        Source/Runtime/Engine/Public/Resource/Render/Shader.h
        Source/Runtime/Engine/Public/Resource/Render/Texture.h
        Source/Runtime/Engine/Public/ECS/Systems/ScreenUIRenderSystem.h
        Source/Runtime/Engine/Public/ECS/Systems/TransformSystem.h
        Source/Runtime/Engine/Public/ECS/Systems/UISystem.h
        Source/Runtime/Engine/Public/ECS/SystemRegistry.h
        Source/Runtime/Engine/Public/ECS/SystemFwd.h
        Source/Runtime/Engine/Public/ECS/SystemConstants.h
        Source/Runtime/Engine/Public/ECS/SystemManager.h
        Source/Runtime/Engine/Public/Utils/RegistryEntry.h
        Source/Runtime/Engine/Public/Utils/Utf.h
        Source/Runtime/Engine/Public/Resource/Audio/Audio.h
        Source/Runtime/Engine/Public/TomatoFwd.h
        Source/Runtime/Core/Public/Containers/UnorderedPair.h
        Source/Runtime/Engine/Public/Collision/Narrow/GJK.h
        Source/Runtime/Engine/Public/Collision/Narrow/EPA.h
        Source/Runtime/Engine/Public/Collision/Broad/BroadPhase.h
        Source/Runtime/Engine/Public/Collision/CollisionFwd.h
        Source/Runtime/Engine/Public/Collision/Broad/SAP.h
        Source/Runtime/Engine/Public/Collision/Narrow/NarrowPhase.h
        Source/Runtime/Engine/Public/Collision/CollisionInfo.h
        Source/Runtime/Engine/Public/Math/Normal.h
        Source/Runtime/Engine/Public/Network/NetworkFwd.h
        Source/Runtime/Engine/Public/Network/NetConnection.h
        Source/Runtime/Core/Public/Network/SocketAddress.h
        Source/Runtime/Core/Public/Network/TCPSocket.h
        Source/Runtime/Core/Public/Network/UDPSocket.h
        Source/Runtime/Core/Public/Network/NetBitReader.h
        Source/Runtime/Core/Public/Network/NetBitWriter.h
        Source/Runtime/Engine/Public/Network/SocketUtil.h
        Source/Runtime/Engine/Public/Network/PacketTypes.h
        Source/Runtime/Engine/Public/Network/TCPNetDriver.h
        Source/Runtime/Engine/Public/Network/UDPNetDriver.h
        Source/Runtime/Engine/Public/Network/ClientNetwork.h
        Source/Runtime/Engine/Public/Network/WinsockInitializer.h
        Source/Runtime/Engine/Public/Network/NetTypes.h
        Source/Runtime/Engine/Public/GameNetwork/Message/InputMessage.h
        Source/Runtime/Engine/Public/GameNetwork/Message/NetMessage.h
        Source/Runtime/Engine/Public/GameNetwork/GamePlayNetSystem.h
        Source/Runtime/Engine/Public/ECS/Components/Hierarchy.h
        Source/Runtime/Engine/Private/ECS/Hierarchy/Hierarchy.h
        Source/Runtime/Engine/Public/ECS/Systems/IntegrationSystem.h
        Source/Runtime/Engine/Public/Prefab/Prefab.h
        Source/Runtime/Engine/Public/ECS/Systems/GravitySystem.h
        Source/Runtime/Engine/Private/Prefab/Character/CharacterMovement.h
        Source/Runtime/Engine/Public/ECS/Components/Lifetime.h
        Source/Runtime/Engine/Private/ECS/Systems/GarbageEntityCollectionSystem.h
        Source/Runtime/Engine/Public/GameNetwork/Rollback/RollbackConfig.h
        Source/Runtime/Engine/Public/ECS/Entity/Lifetime.h
        Source/Runtime/Engine/Public/ECS/Entity/Hierarchy.h
)

set(ENGINE_SOURCES
        Source/Runtime/Engine/Private/Collision/CollisionLayerMatrix.cpp
        Source/Runtime/Core/Private/Collision/ColliderSupport.cpp
        Source/Runtime/Engine/Private/ECS/Systems/CameraSystem.cpp
        Source/Runtime/Engine/Private/Engine.cpp
        Source/Runtime/Engine/Private/Services/Window.cpp
        Source/Runtime/Engine/Private/Input/InputRecorder.cpp
        Source/Runtime/Engine/Private/Services/Input.cpp
        Source/Runtime/Engine/Private/Tick/TickClock.cpp
        Source/Runtime/Engine/Private/ECS/Systems/CollisionSystem.cpp
        Source/Runtime/Engine/Private/ECS/Systems/KinematicMovementSystem.cpp
        Source/Runtime/Engine/Private/ECS/Systems/RenderSystem.cpp
        Source/Runtime/Engine/Private/Resource/Render/Font.cpp
        Source/Runtime/Engine/Private/Font/TextureAtlas.cpp
        Source/Runtime/Engine/Private/Font/TextRenderer.cpp
        Source/Runtime/Engine/Private/Font/AtlasManager.cpp
        Source/Runtime/Engine/Private/Resource/Render/Mesh.cpp
        Source/Runtime/Engine/Private/State/State.cpp
        Source/Runtime/Engine/Private/Resource/Render/Shader.cpp
        Source/Runtime/Engine/Private/Resource/Render/Texture.cpp
        Source/Runtime/Engine/Private/ECS/Systems/TransformSystem.cpp
        Source/Runtime/Engine/Private/ECS/SystemRegistry.cpp
        Source/Runtime/Engine/Private/ECS/SystemManager.cpp
        Source/Runtime/Engine/Private/Resource/Audio/Audio.cpp
        Source/Runtime/Engine/Private/Collision/Narrow/GJK.cpp
        Source/Runtime/Engine/Private/Collision/Narrow/EPA.cpp
        Source/Runtime/Engine/Private/Math/Normal.cpp
        Source/Runtime/Engine/Private/Collision/Broad/SAP.cpp
        Source/Runtime/Engine/Private/Utils/Utf.cpp
        Source/Runtime/Engine/Private/Input/InputUI.cpp
        Source/Runtime/Engine/Private/ECS/Systems/UISystem.cpp
        Source/Runtime/Engine/Private/ECS/Systems/ScreenUIRenderSystem.cpp
        Source/Runtime/Core/Private/Network/SocketAddress.cpp
        Source/Runtime/Core/Private/Network/TCPSocket.cpp
        Source/Runtime/Core/Private/Network/UDPSocket.cpp
        Source/Runtime/Core/Private/Network/NetBitReader.cpp
        Source/Runtime/Core/Private/Network/NetBitWriter.cpp
        Source/Runtime/Engine/Private/Network/SocketUtil.cpp
        Source/Runtime/Engine/Private/Network/TCPNetDriver.cpp
        Source/Runtime/Engine/Private/Network/UDPNetDriver.cpp
        Source/Runtime/Engine/Private/Network/CilentNetwork.cpp
        Source/Runtime/Engine/Private/GameNetwork/Message/InputMessage.cpp
        Source/Runtime/Engine/Private/GameNetwork/GamePlayNetSystem.cpp
        Source/Runtime/Engine/Private/ECS/Hierarchy/Hierarchy.cpp
        Source/Runtime/Engine/Private/ECS/Systems/IntegrationSystem.cpp
        Source/Runtime/Engine/Private/Collision/Broad/BroadPhase.cpp
        Source/Runtime/Engine/Private/ECS/Components/Transform.cpp
        Source/Runtime/Engine/Private/Prefab/Prefab.cpp
        Source/Runtime/Engine/Private/ECS/Systems/GravitySystem.cpp
        Source/Runtime/Engine/Private/Prefab/Character/CharacterMovement.cpp
        Source/Runtime/Engine/Private/ECS/Systems/GarbageEntityCollectionSystem.cpp
        Source/Runtime/Engine/Private/ECS/Entity/Hierarchy.cpp
        Source/Runtime/Engine/Private/ECS/Entity/Lifetime.cpp
)

add_library(engine STATIC
        ${ENGINE_HEADERS}
        ${ENGINE_SOURCES}
        ${THIRD_PARTY_SOURCES}
)

target_include_directories(engine
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Source/Runtime/Core/Public
        ${CMAKE_CURRENT_LIST_DIR}/Source/Runtime/Engine/Public

        ${CMAKE_CURRENT_LIST_DIR}/Source/ThirdParty
        ${CMAKE_CURRENT_LIST_DIR}/Source/ThirdParty/glad/include
        ${freetype_SOURCE_DIR}/include

        PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Source/Runtime/Core/Private
        ${CMAKE_CURRENT_LIST_DIR}/Source/Runtime/Engine/Private
)

if (NOT TARGET glfw)
    message(FATAL_ERROR "Target 'glfw' not found")
endif()

if (NOT TARGET glm)
    message(FATAL_ERROR "Target 'glm' not found")
endif()

if (NOT TARGET EnTT)
    message(FATAL_ERROR "Target 'EnTT' not found")
endif()

if (NOT TARGET freetype)
    message(FATAL_ERROR "Target 'freetype' not found")
endif()

# !!! Windows-ONLY !!!
target_link_libraries(engine
        PUBLIC
        project_options
        glm
        EnTT
        freetype

        PRIVATE
        glfw
        opengl32    # OpenGL
        ws2_32      # Winsock API
)

add_custom_command(TARGET engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/Resources
        $<TARGET_FILE_DIR:contents>/Resources/Engine
)

target_compile_definitions(engine PRIVATE
        ENGINE_RESOURCE_PATH="Resources/Engine/"
)