cmake_minimum_required(VERSION 3.4)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(ogldemo)
SET(CMAKE_BUILD_TYPE Debug)

if(WIN32)
    MESSAGE(STATUS “This is windows platform......”)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

    include_directories(${PROJECT_SOURCE_DIR}/thirdparty/glad/include)
    include_directories(${PROJECT_SOURCE_DIR}/thirdparty/stb)
    include_directories(${PROJECT_SOURCE_DIR}/thirdparty/glfw/include)
    include_directories(${PROJECT_SOURCE_DIR}/thirdparty/glm)
    include_directories(${PROJECT_SOURCE_DIR}/thirdparty/gli)
    include_directories(${PROJECT_SOURCE_DIR}/)
    configure_file(./common/root_directory.h.in configuration/root_directory.h)
    include_directories(${CMAKE_BINARY_DIR}/configuration)

    link_directories(${PROJECT_SOURCE_DIR}/thirdparty/glfw/build/src)

    set(oglLibs ${PROJECT_SOURCE_DIR}/thirdparty/glfw/build/src/libglfw3.a)
    set(thirdcpp ${CMAKE_SOURCE_DIR}/thirdparty/glad/src/glad.c )

    file(GLOB commoncpps ${CMAKE_SOURCE_DIR}/common/*.cpp)
    message(${commoncpps})
    
    add_subdirectory(advanced_ogl)
    add_subdirectory(introduction)
    add_subdirectory(lighting)
    add_subdirectory(rounded-rect)
    
else(WIN32)
    include_directories(${CMAKE_SOURCE_DIR}/thirdparty/glad/include)
    include_directories(${CMAKE_SOURCE_DIR}/thirdparty/)
    include_directories(${CMAKE_SOURCE_DIR}/thirdparty/gli)
    include_directories(${CMAKE_SOURCE_DIR}/thirdparty/glm)
    include_directories(${CMAKE_SOURCE_DIR})
    find_package(PkgConfig REQUIRED)
    #find_package (GLUT REQUIRED)
    pkg_check_modules(DEPS REQUIRED glfw3 gdk-pixbuf-2.0)
    #${GLUT_INCLUDE_DIR}
    include_directories(${DEPS_INCLUDE_DIRS} )

    set(thirdcpp ${CMAKE_SOURCE_DIR}/thirdparty/glad/src/glad.c)
    file(GLOB commoncpps ${CMAKE_SOURCE_DIR}/common/*.cpp)
    
    set(oglLibs
        ${DEPS_LIBRARIES}
        #${GLUT_glut_LIBRARY}
        -lGL
        -lX11
        #-lm
        -lpthread
        -ldl
    )
    configure_file(./common/root_directory.h.in configuration/root_directory.h)
    include_directories(${CMAKE_BINARY_DIR}/configuration)
    message("CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}")
    add_subdirectory(advanced_ogl)
    add_subdirectory(introduction)
    add_subdirectory(lighting)
    #add_subdirectory(blur-effect)
    add_subdirectory(rounded-rect)
endif()
