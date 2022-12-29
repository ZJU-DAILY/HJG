set(CMAKE_CXX_STANDARD 11)

add_executable(test_nndescent test_nndescent.cpp)
target_link_libraries(test_nndescent ${PROJECT_NAME})
