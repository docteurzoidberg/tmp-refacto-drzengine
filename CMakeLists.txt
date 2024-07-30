cmake_minimum_required(VERSION 3.5.0)
project(drzengine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

include_directories(include)

add_library(drzenginepge src/DrzEngine.cpp src/DrzEngine_PGE.cpp)
add_library(drzenginefb src/DrzEngine.cpp src/DrzEngine_FB.cpp)

add_executable(test_drzengine_pge test_drzengine_pge.cpp TestApp.cpp)
add_executable(test_drzengine_fb test_drzengine_fb.cpp TestApp.cpp)

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

