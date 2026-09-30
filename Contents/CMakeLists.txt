set(CONTENTS_HEADERS
		Source/PlayTest/TestState.h
		Source/PlayTest/LoadState.h
		Source/PlayTest/CollisionTestComponent.h
        Source/MyState.h
		Source/PlayTest/UIController.h
        Source/Game/WaveManager.h
		Source/jung/WaveManagerFwd.h
)

set(CONTENTS_SOURCES
        Source/Main.cpp
		Source/PlayTest/TestState.cpp
		Source/PlayTest/LoadState.cpp
		Source/PlayTest/UIController.cpp
		Source/PlayTest/TimerTestSystem.cpp
		Source/Game/GameState.cpp
        Source/Game/WaveSystem.cpp
        Source/Game/WavePool.h
        Source/Game/WavePool.cpp
        Source/Game/WaveColliderPool.h
        Source/Game/WaveColliderPool.cpp
        Source/Game/WaveManager.cpp
)

add_executable(contents
        ${CONTENTS_SOURCES}
)

target_include_directories(contents
		PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/Source
)

target_link_libraries(contents
        PRIVATE
        # Force inclusion of all object files in the static library for SystemRegistry
        $<LINK_LIBRARY:WHOLE_ARCHIVE, engine>
        project_options
)

target_compile_definitions(contents
		PRIVATE
		TMT_PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

add_custom_command(TARGET contents POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/Resources
        $<TARGET_FILE_DIR:contents>/Resources/Contents
)