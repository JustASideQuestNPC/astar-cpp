cmake_minimum_required(VERSION 3.21)
project(astar-cpp LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(astar-cpp src/main.cpp src/pixel_renderer.h src/a_star.hpp)
target_link_libraries(astar-cpp PRIVATE sfml-graphics)
target_compile_features(astar-cpp PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET astar-cpp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:astar-cpp> $<TARGET_FILE_DIR:astar-cpp> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS astar-cpp)
