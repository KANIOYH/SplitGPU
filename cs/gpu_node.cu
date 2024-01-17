#include <cstddef>
#include <cstring>
#include <string>
#include <sys/types.h>
#include "gpu_node.h"
#include "cuda_param.h"

namespace SplitGPU {



GPU_node::GPU_node() {
    /* both using cuda driver api and cuda runtime api */
    cuInit(0);
    cuDeviceGet(&device, 0);
    cudaSetDevice(0);
    cuCtxGetCurrent(&context);
}

// void GPU_node::exec_cuda(std::string cuda_name,gpu_request* req, cudaError_t& result) {
//     char* args = req->fargs;
//     printf("%s\n",cuda_name.c_str());
//     auto item = func_id_map.find(cuda_name);
//     if(item == func_id_map.end()) {
//         printf("not find\n");
//         return;
//     }
        
//     off_t offset = 0;
//     switch (item->second) {
//     case SYMBOL_cudaMalloc: {
//         void** p_devPtr;
//         size_t* p_size;
//         UNPACK_PARAM(args,offset,p_devPtr);
//         UNPACK_PARAM(args,offset,p_size);
//         result = cudaMalloc(p_devPtr, *p_size);
//         memcpy(&args[offset], &result, sizeof(result));
//         printf("cudaMalloc,%p %ld\n",*p_devPtr,*p_size);
//     } break;
//     case SYMBOL_cudaFree: {

//     } break;
//     case SYMBOL_cudaDeviceSynchronize: {
//         result = cudaDeviceSynchronize();
//         memcpy(&args[offset], &result, sizeof(result));
//     };
//     case SYMBOL_cudaMemcpy: {
//         char** p_dst;
//         char** p_src;
//         size_t* p_count;
//         cudaMemcpyKind* p_kind;
//         UNPACK_PARAM(args,offset,p_dst)
//         UNPACK_PARAM(args,offset,p_src)
//         UNPACK_PARAM(args,offset,p_count)
//         UNPACK_PARAM(args,offset,p_kind)
//         switch(*p_kind) {
//         case cudaMemcpyHostToHost:
//         case cudaMemcpyHostToDevice: {
//             printf("host to device\n");
//             result = cudaMemcpy(*p_dst, req->extra_data,
//                                              *p_count, *p_kind);
//             memcpy(&args[offset], &result, sizeof(result));
//         } break;
//         case cudaMemcpyDeviceToHost:
//         case cudaMemcpyDeviceToDevice:
//         case cudaMemcpyDefault:
//           break;
//         }
//     } break;
//     default: {
//         /*unregister cuda function*/
//     }

//     }
// }

cudaError_t exec_cudaMalloc(void** devPtr, size_t size) {
    cudaError_t result;
    result = cudaMalloc(devPtr,size);
    return result;
}
cudaError_t exec_cudaDeviceSynchronize() {
    cudaError_t result = cudaSuccess;;
    return result;
}
cudaError_t exec_cudaMemcpy(void* dst,void* src, size_t size, cudaMemcpyKind kind) {
    cudaError_t result = cudaSuccess;
    return result;   
}

}