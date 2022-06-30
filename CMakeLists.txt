cmake_minimum_required(VERSION 3.16.2)

file(READ version.txt HG_VERSION)
string(STRIP ${HG_VERSION} HG_VERSION)

#
project(harfang_core VERSION ${HG_VERSION} LANGUAGES CXX C HOMEPAGE_URL "https://harfang3d.com")

set(CMAKE_CXX_STANDARD 98) # very conservative target

# fmt::format
add_subdirectory(extern/fmt)

# foundation
set(FOUNDATION_HDRS
foundation/assert.h
foundation/clock.h
foundation/color.h
foundation/math.h
foundation/matrix3.h
foundation/matrix4.h
foundation/matrix44.h
foundation/quaternion.h
foundation/rand.h
foundation/rotation_order.h
foundation/time.h
foundation/unit.h
foundation/vector2.h
foundation/vector3.h
foundation/vector4.h
)

set(FOUNDATION_SRCS
foundation/assert.cpp
foundation/clock.cpp
foundation/color.cpp
foundation/math.cpp
foundation/matrix3.cpp
foundation/matrix4.cpp
foundation/matrix44.cpp
foundation/quaternion.cpp
foundation/rand.cpp
foundation/time.cpp
foundation/unit.cpp
foundation/vector2.cpp
foundation/vector3.cpp
foundation/vector4.cpp
)

add_library(foundation STATIC ${FOUNDATION_SRCS} ${FOUNDATION_HDRS})
target_include_directories(foundation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# unit tests
add_executable(test_foundation tests/test_foundation.cpp)
target_link_libraries(test_foundation PUBLIC foundation)
target_include_directories(test_foundation PUBLIC extern/acutest)
