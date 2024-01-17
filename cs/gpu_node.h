/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-12 21:09:10
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-17 13:17:56
 * @FilePath: /SplitGPU/cs/gpu_node.h
 * @Description: 
 * 
 */
#pragma once
#include <map>
#include <string>

#include <cuda.h>
#include <cuda_runtime.h>

#include "cuda_param.h"



namespace SplitGPU {

cudaError_t exec_cudaMalloc(void** devPtr, size_t size);
cudaError_t exec_cudaDeviceSynchronize();
cudaError_t exec_cudaMemcpy();

class GPU_node {
public:
    GPU_node();
    // void launch_kernel(std::string kernel_name, dim3 gridDim, dim3 blockDim, 
    //                     size_t sharedMem, cudaStream_t stream, void **args);
    //void exec_cuda(std::string cuda_name,gpu_request* req, cudaError_t& result);
private:
    CUdevice device;
    CUcontext context;
    std::map<std::string, int> func_id_map;
    std::map<std::string, int> kernel_id_map;
};

}