project(lighting)

add_executable(color color.cpp ${thirdcpp})
target_link_libraries(color ${oglLibs})

add_executable(ambient ambient.cpp ${commoncpps} ${thirdcpp} )
target_link_libraries(ambient ${oglLibs})

add_executable(phong phonglighting.cpp ${thirdcpp} ${commoncpps})
target_link_libraries(phong ${oglLibs})

add_executable(spec spec.cpp ${thirdcpp} ${commoncpps})
target_link_libraries(spec ${oglLibs})

add_executable(material material.cpp ${thirdcpp} ${commoncpps})
target_link_libraries(material ${oglLibs})
