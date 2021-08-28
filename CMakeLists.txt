# ┌──────────────────────────────────────────────────────────────────┐
# │  Projects Settings                                               │
# └──────────────────────────────────────────────────────────────────┘

cmake_minimum_required(VERSION 3.0)
project(FluidScripten)
add_definitions("-std=c++11")

# ┌──────────────────────────────────────────────────────────────────┐
# │  Executable                                                      │
# └──────────────────────────────────────────────────────────────────┘
file(GLOB_RECURSE source_files src/*.cpp)
add_executable(FluidScripten.js ${source_files})

# emscripten specific

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FILESYSTEM=0  -sUSE_SDL=2 -O3 --closure 1 -g0 -sENVIRONMENT=web -sMALLOC='emmalloc' -sALLOW_MEMORY_GROWTH=0 -sINITIAL_MEMORY=1400mb -DNDEBUG  -sASSERTIONS=0  -fno-rtti  -flto -s SUPPORT_LONGJMP=0 -s USE_SDL=2 ")
