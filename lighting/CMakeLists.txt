project(lighting)

add_executable(color color.cpp ${thirdcpp})
target_link_libraries(color ${oglLibs})


