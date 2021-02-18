project(introduction)

add_executable(vbo vbo.cpp ${thirdcpp})
target_link_libraries(vbo ${oglLibs})

add_executable(vao vao.cpp ${thirdcpp})
target_link_libraries(vao ${oglLibs})

add_executable(vao2 ${thirdcpp} vao2.cpp)
target_link_libraries(vao2 ${oglLibs})


add_executable(textures ${thirdcpp} textures.cpp)
target_link_libraries(textures ${oglLibs})

add_executable(triangle ${thirdcpp} triangle.cpp)
target_link_libraries(triangle ${oglLibs})

add_executable(window ${thirdcpp} window.cpp)
target_link_libraries(window ${oglLibs})
