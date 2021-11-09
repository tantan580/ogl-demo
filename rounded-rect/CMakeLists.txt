project(rounded-rect)

add_executable(rounded-rect rounded-rect.cpp ${thirdcpp} ${commoncpp})
target_link_libraries(rounded-rect ${oglLibs})