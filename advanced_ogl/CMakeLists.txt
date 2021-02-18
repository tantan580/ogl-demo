
project(advanced-ogl)
add_executable(framebuffers framebuffers.cpp ${thirdcpp})
target_link_libraries(framebuffers ${oglLibs})

add_executable(stencil stencil.cpp ${thirdcpp})
target_link_libraries(stencil ${oglLibs})

