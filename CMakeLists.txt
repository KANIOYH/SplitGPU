cmake_minimum_required(VERSION 3.0.0)
project(SplitGPU VERSION 0.1.0 LANGUAGES C CXX)

enable_language(CUDA)

include(CTest)
enable_testing()

add_executable(Split_GPU_Application
                communicate/shmem.c
                communicate/ipc.cpp 
                manager/schedule.cpp
                manager/user_context.cpp
                cs/controller.cpp
                app/Split_GPU_Application.cpp)
target_include_directories(Split_GPU_Application PUBLIC "./include/" "/usr/local/cuda/include/")
target_link_libraries(Split_GPU_Application PUBLIC -pthread -lssl -lcrypto)

add_library(sgcudart SHARED 
            hook/cuda_hook.cpp
            cs/client.cpp
            communicate/ipc.cpp
            communicate/shmem.c)
target_include_directories(sgcudart PUBLIC "./include/" "/usr/local/cuda/include/")
add_custom_command(  
  TARGET sgcudart
  COMMAND rm libcuda*.so
  COMMAND ln -s  libsgcudart.so libcudart.so
)

add_executable(test_cuda test/test_cuda.cu)
target_include_directories(test_cuda PUBLIC "/usr/local/cuda/include/")
target_link_libraries(test_cuda PUBLIC -lcuda -lcudart)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

