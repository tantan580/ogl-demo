project(rounded-rect)

add_executable(rounded-rect ${thirdcpp} rounded-rect.cpp ${commoncpps})
target_link_libraries(rounded-rect ${oglLibs})
