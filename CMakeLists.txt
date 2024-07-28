cmake_minimum_required(VERSION 3.5.0)
project(drzengine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(drzenginepge src/DrzEngine.cpp src/DrzEngine_PGE.cpp)
add_library(drzenginefb src/DrzEngine.cpp src/DrzEngine_FB.cpp)

add_executable(test_drzengine_pge test_drzengine_pge.cpp)
add_executable(test_drzengine_fb test_drzengine_fb.cpp)

#link the library to the executable
target_link_libraries(test_drzengine_pge drzenginepge)
target_link_libraries(test_drzengine_fb drzenginefb)

#libpng
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

#link the library to the executable
target_link_libraries(test_drzengine_pge ${PNG_LIBRARY})

#libgl 
#set opengl preferences
set(OpenGL_GL_PREFERENCE "GLVND")

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(test_drzengine_pge ${OPENGL_LIBRARIES})

#x11
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
target_link_libraries(test_drzengine_pge ${X11_LIBRARIES})

#pthread
find_package(Threads REQUIRED)
target_link_libraries(test_drzengine_pge Threads::Threads)
target_link_libraries(test_drzengine_fb Threads::Threads)

