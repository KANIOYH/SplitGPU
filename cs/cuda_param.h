/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-13 09:24:43
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-16 15:24:27
 * @FilePath: /SplitGPU/cs/cuda_param.h
 * @Description: 
 * 
 */
#pragma once

#include <string>
namespace SplitGPU {

#define PACK_PARAM(args,offset,var) {           \
    memcpy(&args[offset],&var,sizeof(var));     \
    offset += sizeof(var);                      \
}                                               \
    
#define UNPACK_PARAM(args,offset,var) {         \
    var = (__typeof__(var))(&args[offset]);     \
    offset += sizeof(*var);                     \
}                                               \

/* kernel param*/
#define ARG_SIZE (64)
#define CUDA_REQ_TYPE (0)
#define KERNEL_REQ_TYPE (1)

struct gpu_request {
    bool type;
    char func_name[32];
    char fargs[ARG_SIZE];
    char kargs[ARG_SIZE];
};

enum cuda_func_list {
    SYMBOL_START,
    SYMBOL_cudaMalloc,
    SYMBOL_cudaFree,
    SYMBOL_cudaDeviceSynchronize,
    SYMBOL_cudaMemcpy,
    SYMBOL_END,
};

static std::string cuda_func_name[] = {
    "symbol_start",
    "cudaMalloc",
    "cudaFree",
    "cudaDeviceSynchronize",
    "cudaMemcpy",
    "symbol_end",
};

}