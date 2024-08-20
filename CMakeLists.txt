cmake_minimum_required(VERSION 3.5.0)
cmake_policy(SET CMP0077 NEW)
project(drzengine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_PGELIB "Build PGE library" ON)
option(BUILD_FBLIB "Build FB library" OFF)
option(BUILD_DOC "Build documentation" ON)
option(BUILD_TEST_APPS "Build test apps" ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


set(SOURCES
  src/DrzEngine.cpp
  src/DrzGraphics.cpp
  src/DrzInputs.cpp
)

if(BUILD_PGELIB) 
  add_library(drzenginepge STATIC ${SOURCES} src/DrzEngine_PGE.cpp)
  # Specify the include directory for consumers
  target_include_directories(drzenginepge PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
  #TODO: add support for others platform using olc::pge like emscripten and windows here
endif()

if(BUILD_FBLIB)
  add_library(drzenginefb ${SOURCES} src/DrzEngine_FB.cpp)
  target_include_directories(drzenginefb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

if(BUILD_DOC)
  # check if Doxygen is installed
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
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
  else()
    message("Doxygen need to be installed to generate the doxygen documentation")
  endif()
endif()

if(BUILD_TEST_APPS)
  if(BUILD_PGELIB)
    #add test app
    add_executable(test_drzengine_pge test_drzengine_pge.cpp TestApp.cpp)
    target_link_libraries(test_drzengine_pge PRIVATE drzenginepge)
    
    #libpng
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIR})

    #link the library to the executable
    target_link_libraries(test_drzengine_pge PRIVATE ${PNG_LIBRARY})

    #libgl 
    #set opengl preferences
    set(OpenGL_GL_PREFERENCE "GLVND")

    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    target_link_libraries(test_drzengine_pge PRIVATE OpenGL::GL)

    #x11
    find_package(X11 REQUIRED)
    include_directories(${X11_INCLUDE_DIR})
    target_link_libraries(test_drzengine_pge PRIVATE X11::X11)

    #pthread
    find_package(Threads REQUIRED)
    target_link_libraries(test_drzengine_pge PRIVATE Threads::Threads)

  endif()
  if(BUILD_FBLIB)

    add_executable(test_drzengine_fb test_drzengine_fb.cpp TestApp.cpp)
    target_link_libraries(test_drzengine_fb PRIVATE drzenginefb) 

    #pthread
    find_package(Threads REQUIRED)
    target_link_libraries(test_drzengine_fb PRIVATE Threads::Threads)
  endif()
endif()