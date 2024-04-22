cmake_minimum_required(VERSION 3.0.0)
project(SplitGPU VERSION 0.1.0 LANGUAGES C CXX)

enable_language(CUDA)

include(CTest)
enable_testing()

add_executable(sp_gpu_d
                communicate/shmem.c
                communicate/ipc.cpp 
                manager/schedule.cpp
                manager/user_context.cpp
                cs/controller.cpp
                app/sp_gpu_d.cpp)
target_include_directories(sp_gpu_d PUBLIC "./include/" "/usr/local/cuda/include/")
target_link_libraries(sp_gpu_d PUBLIC -pthread -lssl -lcrypto)

add_library(cudart SHARED 
            hook/cuda_hook.cpp
            cs/client.cpp
            communicate/ipc.cpp
            communicate/shmem.c)
target_include_directories(cudart PUBLIC "./include/" "/usr/local/cuda/include/")
# add_custom_command(  
#   TARGET sgcudart
#   COMMAND rm libcuda*.so
#   COMMAND ln -s  libsgcudart.so libcudart.so
# )

add_executable(test_cuda test/test_cuda.cu)
target_include_directories(test_cuda PUBLIC "/usr/local/cuda/include/")
target_link_libraries(test_cuda PUBLIC -lcuda -lcudart)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

