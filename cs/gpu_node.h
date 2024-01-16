/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-12 21:09:10
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-16 14:54:26
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

class GPU_node {
public:
    GPU_node();
    // void launch_kernel(std::string kernel_name, dim3 gridDim, dim3 blockDim, 
    //                     size_t sharedMem, cudaStream_t stream, void **args);
    void exec_cuda(std::string cuda_name,char args[]);
private:
    CUdevice device;
    CUcontext context;
    std::map<std::string, int> func_id_map;
    std::map<std::string, int> kernel_id_map;
};

}