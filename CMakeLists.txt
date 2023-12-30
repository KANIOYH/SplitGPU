cmake_minimum_required(VERSION 3.0.0)
project(SplitGPU VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

# add_executable(SplitGPU main.cpp)
# target_include_directories(SplitGPU PUBLIC "./include/")
# target_include_directories(SplitGPU PUBLIC "/usr/local/cuda/include/")
include_directories("include/")
add_executable(cs   communicate/shmem.c
                    communicate/ipc.cpp 
                    test/test_ipc.cpp)
target_include_directories(cs PUBLIC "./include/")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

