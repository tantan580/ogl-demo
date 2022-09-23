
project(advanced-ogl)
add_executable(framebuffers framebuffers.cpp ${thirdcpp} ${commoncpps})
target_link_libraries(framebuffers ${oglLibs})

add_executable(stencil stencil.cpp ${thirdcpp})
target_link_libraries(stencil ${oglLibs})

add_executable(cubemaps ${thirdcpp} cubemaps.cpp ${commoncpps})
target_link_libraries(cubemaps ${oglLibs})

# add_executable(cube_map_array cube_map_array.cpp ${thirdcpp})
# target_link_libraries(cube_map_array ${oglLibs})
