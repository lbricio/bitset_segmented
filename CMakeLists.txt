cmake_minimum_required(VERSION 3.20)
project(bitset_segmented LANGUAGES CXX)

if(WIN32)
    set(CMAKE_INSTALL_PREFIX "C:/libs/bitset_segmented")
else()
    set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
add_library(bitset_segmented SHARED
    src/bitset_segmented.cpp
)

target_include_directories(bitset_segmented
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(bitset_segmented PUBLIC cxx_std_20)

target_compile_options(bitset_segmented
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -O3>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /O2>
)

install(TARGETS bitset_segmented
        EXPORT bitset_segmentedTargets
        ARCHIVE DESTINATION lib       # para static libs se existirem
        LIBRARY DESTINATION lib       # para .so
        RUNTIME DESTINATION bin)      # para .dll

install(DIRECTORY include/ DESTINATION include)
