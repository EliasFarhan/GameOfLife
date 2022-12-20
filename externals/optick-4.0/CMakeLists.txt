file(GLOB_RECURSE optick_files src/*.cpp include/*.h)

add_library(optick STATIC ${optick_files})
target_include_directories(optick PUBLIC include/)
target_compile_definitions(optick PUBLIC OPTICK_ENABLE_GPU=0)