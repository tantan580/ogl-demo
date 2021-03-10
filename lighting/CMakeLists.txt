project(lighting)

add_executable(color color.cpp ${thirdcpp})
target_link_libraries(color ${oglLibs})

add_executable(ambient ambient.cpp ${thirdcpp})
target_link_libraries(ambient ${oglLibs})

add_executable(phong phonglighting.cpp ${thirdcpp})
target_link_libraries(phong ${oglLibs})

add_executable(spec spec.cpp ${thirdcpp})
target_link_libraries(spec ${oglLibs})
