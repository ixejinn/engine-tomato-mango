set(CONTENTS_HEADERS
        Source/TestState.h
        Source/LoadState.h
        Source/MyState.h
        Source/UIController.h
)

set(CONTENTS_SOURCES
        Source/Main.cpp
        Source/TestState.cpp
        Source/LoadState.cpp
        Source/CollisionTestComponent.h
        Source/UIController.cpp
        Source/TimerTestSystem.cpp
        Source/CollisionTestState.h
        Source/CollisionTestState.cpp
 "Source/jung/MyState.cpp")

add_executable(contents
        ${CONTENTS_SOURCES}
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