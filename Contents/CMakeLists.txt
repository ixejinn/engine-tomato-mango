set(CONTENTS_HEADERS
        Source/TestState.h
        Source/LoadState.h
        Source/UIController.h
)

set(CONTENTS_SOURCES
        Source/Main.cpp
        Source/TestState.cpp
        Source/LoadState.cpp
        Source/CollisionTestComponent.h
        Source/UIController.cpp
)

add_executable(contents
        ${CONTENTS_SOURCES}
)

target_link_libraries(contents
        PRIVATE
        # Force inclusion of all object files in the static library for SystemRegistry
        $<LINK_LIBRARY:WHOLE_ARCHIVE, engine>
        project_options
)

add_custom_command(TARGET contents POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/Resources
        $<TARGET_FILE_DIR:contents>/Resources/Contents
)