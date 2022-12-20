cmake_minimum_required (VERSION 3.18)
project(RollbackGame)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /w14640)
    add_compile_definitions(_USE_MATH_DEFINES)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -flto)
    add_link_options(-flto)
endif()

add_subdirectory(externals/optick-4.0/)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(OpenMP REQUIRED)

file(GLOB_RECURSE src_files src/*.cpp include/*.h)
add_executable(gameoflife ${src_files})
target_link_libraries(gameoflife PRIVATE optick sfml-system sfml-graphics sfml-window imgui::imgui ImGui-SFML::ImGui-SFML OpenMP::OpenMP_CXX)
target_include_directories(gameoflife PRIVATE include/)
find_package(benchmark CONFIG REQUIRED)

add_executable(bench bench/compute.cpp src/conway.cpp)
target_link_libraries(bench PRIVATE benchmark::benchmark benchmark::benchmark_main)
target_include_directories(bench PRIVATE include/)