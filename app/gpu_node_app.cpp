/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-01-15 14:23:59
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-18 10:16:22
 * @FilePath: /SplitGPU/app/gpu_node_app.cpp
 * @Description: http server register for gpu pool
 * 
 */
#include <cuda_runtime.h>
#include "gpu_node.h"
#include "httplib.h"

using namespace std;
using namespace SplitGPU;
using namespace httplib;

GPU_node g_node;

/* pack and unpack param */
#define UNPACK_PARAM(str,var) {                 \
    string str_var = req.get_param_value(str);  \
    memcpy(&var,str_var.c_str(),sizeof(var));   \
}                                               \

//#define PACK_PARAM () 

void Post_cudaMalloc(const Request& req, Response& resp) {
    void* ptr;
    size_t size;
    cudaError_t result;
    std::string str_devPtr = req.get_param_value("devPtr");
    UNPACK_PARAM("size",size);
    result = g_node.exec_cudaMalloc(&ptr, size);
    char byte_result[sizeof(void*)+sizeof(cudaError_t)]={0};
    memcpy(&byte_result[0],&ptr,sizeof(void*));
    memcpy(&byte_result[sizeof(void*)],&result,sizeof(cudaError_t));
    resp.set_content(byte_result,sizeof(void*)+sizeof(cudaError_t),"cuda");
}

void Post_cudaMemcpy(const Request& req, Response& resp) {
    char* dst;
    char* src;
    size_t count;
    cudaMemcpyKind kind;
    cudaError_t result;
    UNPACK_PARAM("dst",dst)
    UNPACK_PARAM("src",src)
    UNPACK_PARAM("count",count)
    UNPACK_PARAM("kind",kind)
    switch (kind) {
    case cudaMemcpyHostToHost:  
    result = cudaSuccess;
    break;
    case cudaMemcpyHostToDevice: {
        string str_data = req.get_param_value("data");
        char* src_d = const_cast<char*>(str_data.data());
        result=cudaMemcpy(dst,const_cast<char*>(str_data.data()),count,kind);
        printf("res %d\n",result);
        char byte_result[sizeof(cudaError_t)];
        memcpy(&byte_result[0],&result,sizeof(cudaError_t));
        resp.set_content(byte_result,sizeof(cudaError_t),"cuda");
    } break;
    case cudaMemcpyDeviceToHost: {
        char byte_result[count+sizeof(cudaError_t)];
        result=cudaMemcpy(byte_result,src,count,kind);
        printf("res %d\n",result);
        memcpy(&byte_result[count],&result,sizeof(cudaError_t));
        resp.set_content(byte_result,count+sizeof(cudaError_t),"cuda");
    } break;
    case cudaMemcpyDeviceToDevice: {
        char byte_result[sizeof(cudaError_t)];
        result=cudaMemcpy(dst,src,count,kind);
        memcpy(&byte_result[count],&result,sizeof(cudaError_t));
        resp.set_content(byte_result,sizeof(cudaError_t),"cuda");
    } break;
    case cudaMemcpyDefault:
      break;
    }
}

void Post_cudaDeviceSynchronize(const Request& req, Response& resp) {
    cudaError_t result;
    result = g_node.exec_cudaDeviceSynchronize();
    char byte_result[sizeof(void*)+sizeof(cudaError_t)]={0};
    memcpy(&byte_result[0],&result,sizeof(void*));
    resp.set_content(byte_result,sizeof(cudaError_t),"cuda");
}

void Post_kernel(const Request& req, Response& resp) {
    dim3 gridDim;
    dim3 blockDim;
    size_t sharedMem;
    cudaStream_t stream;
    //char args[1024];
    void** args;
    *args = malloc(1024);
    UNPACK_PARAM("gridDim",gridDim)
    UNPACK_PARAM("blockDim",blockDim)
    UNPACK_PARAM("sharedMem",sharedMem)
    UNPACK_PARAM("stream",stream)
    string str_var = req.get_param_value("args");
    memcpy(args,str_var.c_str(),1024);
    g_node.exec_kernel(req.get_param_value("kernel"), gridDim, blockDim, args, sharedMem, stream);
    //free(*args);
    
}

int main() {

    CUresult res;
    cuInit(0);
    cuDeviceGet(&g_node.device, 0);
    cudaSetDevice(0);
    res = cuCtxGetCurrent(&g_node.context);
    if (res != CUDA_SUCCESS){
        printf("cuCtxGetCurrent\n");
        exit(EXIT_FAILURE);
    }  
    res = cuModuleLoad(&g_node.module, "/home/chenyuanhui/project/SplitGPU/data.fatbin");
    if (res != CUDA_SUCCESS){
        printf("cuModuleLoad\n");
        exit(EXIT_FAILURE);
    }  

    CUdeviceptr dptr;
    res = cuMemAlloc(&dptr, 4);
    if(res!=CUDA_SUCCESS) {
        printf("cuMemAlloc err:%d\n",res);
    }

    httplib::Server server;
    server.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
    res.set_content("Hello World!", "text/plain");
    });
    server.Post("cudaMalloc",Post_cudaMalloc);
    server.Post("cudaMemcpy",Post_cudaMemcpy);
    server.Post("cudaDeviceSynchronize",Post_cudaDeviceSynchronize);
    server.Post("cudaLaunchKernel",Post_kernel);
    server.listen("0.0.0.0", 8888);
    // while(true) {
    //     printf("get req\n");
    //     gpu_request* req;
    //     g_node.exec_cuda(req->func_name,req);
    // }
    return 0;
}