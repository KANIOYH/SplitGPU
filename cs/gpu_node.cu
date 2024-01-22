#include <cstddef>
#include <cstring>
#include <string>
#include <sys/types.h>
#include "gpu_node.h"
#include "cuda_param.h"

namespace SplitGPU {



GPU_node::GPU_node() {
    /* both using cuda driver api and cuda runtime api */
//     CUresult res;
//     cuInit(0);
//     cuDeviceGet(&device, 0);
//     cudaSetDevice(0);
//     res = cuCtxGetCurrent(&context);
//     if (res != CUDA_SUCCESS){
//         printf("cuCtxGetCurrent\n");
//         exit(EXIT_FAILURE);
//     }  
//     res = cuModuleLoad(&module, "/home/chenyuanhui/project/SplitGPU/data.fatbin");
//     if (res != CUDA_SUCCESS){
//         printf("cuModuleLoad\n");
//         exit(EXIT_FAILURE);
//     }   
}

CUfunction GPU_node::get_func(std::string func_name) {
    CUfunction func;
    printf("func name:%s\n",func_name.c_str());
    auto res = cuModuleGetFunction(&func, module, "_Z16parallel_add_valPvim");
    if (res != CUDA_SUCCESS){
        printf("cuModuleGetFunction\n");
        exit(EXIT_FAILURE);
    }  
    return func;
}

cudaError_t GPU_node::exec_cudaMalloc(void** devPtr, size_t size) {
    cudaError_t result;
    result = cudaMalloc(devPtr,size);
    return result;
}
cudaError_t GPU_node::exec_cudaDeviceSynchronize() {
    cudaError_t result = cudaSuccess;;
    return result;
}
cudaError_t GPU_node::exec_cudaMemcpy(void* dst,void* src, size_t size, cudaMemcpyKind kind) {
    cudaError_t result = cudaSuccess;
    return result;   
}

cudaError_t GPU_node::exec_kernel(std::string func_name,dim3 gridDim, dim3 blockDim, void **args,size_t sharedMem, cudaStream_t stream) {
    CUresult res;
    cuInit(0);
    cuDeviceGet(&device, 0);
    cudaSetDevice(0);
    res = cuCtxGetCurrent(&context);
    if (res != CUDA_SUCCESS){
        printf("cuCtxGetCurrent\n");
        exit(EXIT_FAILURE);
    }  

    cudaError_t result = cudaSuccess;
    CUresult curesult;
    //CUdeviceptr dptr = (CUdeviceptr)args;
    printf("------------\n");
    for(int i=0;i<156;i++) {
        printf("%02x ",((unsigned char*)args)[i]);
        if(i!=0 && i%8==0)
            printf("\n");
    }
    printf("\n------------\n");
    CUfunction f;
    res = cuModuleGetFunction(&f, module, func_name.data());
    if (res != CUDA_SUCCESS){
        printf("cuModuleGetFunction\n");
        exit(EXIT_FAILURE);
    }    
    curesult = cuLaunchKernel(f,gridDim.x, gridDim.y, gridDim.z,
                         blockDim.x, blockDim.y, blockDim.z,
                   sharedMem, stream, 0,(void**)&args);
    cuCtxSynchronize();
    // int threadsPerBlock = 256;
    // int blocksPerGrid =(128 + threadsPerBlock - 1) / threadsPerBlock;
    // int val = 1;
    // void* arg[] = {&val};
    // curesult = cuLaunchKernel(kernel,
    //                blocksPerGrid, 1, 1, threadsPerBlock, 1, 1,
    //                0, 0, arg, 0);
    if(curesult!=CUDA_SUCCESS) {
        printf("launch err:%d\n",curesult);
    // cuCtxSynchronize();
    } else {
        printf("launch ok\n");
    }
    return result;   
}

}