cmake_minimum_required(VERSION 2.8)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
project(blur-effect)
SET(CMAKE_BUILD_TYPE Debug)

if(WIN32)
    add_executable(${PROJECT_NAME} ${project_file})   
    add_executable(blur blur-effect/) 
    target_link_libraries(${PROJECT_NAME} glfw3 opengl32)
else(WIN32)
    include_directories(./)

    file(GLOB project_file main.cpp blur_shader.h  gaussian.h ../../glad/src/glad.c )
    add_executable(${PROJECT_NAME} ${project_file})
    target_link_libraries(${PROJECT_NAME}
        ${DEPS_LIBRARIES}
        -lGL
        #-lm
        -lpthread
        -ldl
        )
endif()
