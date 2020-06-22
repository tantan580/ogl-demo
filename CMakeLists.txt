cmake_minimum_required(VERSION 2.8)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)
project(ogldemo)
SET(CMAKE_BUILD_TYPE Debug)

if(WIN32)
    MESSAGE(STATUS “This is windows.”)
    #include_directories($(PROJECT_SOURCE_DIR)/3rdparty/glad/include)
    #include_directories($(PROJECT_SOURCE_DIR)/3rdparty/stb-master)
    #include_directories($(PROJECT_SOURCE_DIR)/3rdparty/glfw-master/include)
    include_directories("G:\\OpenGL\\demo\\3rdparty\\glad\\include\\")
    include_directories("G:\\OpenGL\\demo\\3rdparty\\stb-master\\")
    include_directories("G:\\OpenGL\\demo\\3rdparty\\glfw-master\\include\\")
    include_directories("G:\\OpenGL\\demo\\3rdparty\\glm-master")                     
    link_directories("G:\\OpenGL\\demo\\3rdparty\\glfw-master\\build\\src")
    
    #link_directories($(PROJECT_SOURCE_DIR)/3rdpartyglfw-master/build/src)
    
    file(GLOB project_file introduction/01_4.cpp common.h shader.h texture.h glad.c)
    add_executable(${PROJECT_NAME} ${project_file})   
    add_executable(blur blur-effect/) 
    target_link_libraries(${PROJECT_NAME} glfw3 opengl32)
else(WIN32)
    include_directories(../glad/include)
    include_directories(./)
    #find_package(glfw3 REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(DEPS REQUIRED glfw3 gdk-pixbuf-2.0)
    include_directories(${DEPS_INCLUDE_DIRS})

    file(GLOB project_file advanced_ogl/framebuffers.cpp ../glad/src/glad.c )
    #introduction/textures.cpp   lighting/color.cpp
    add_executable(${PROJECT_NAME} ${project_file})
    target_link_libraries(${PROJECT_NAME}
        ${DEPS_LIBRARIES}
        -lGL
        #-lm
        -lpthread
        -ldl
        )
endif()
