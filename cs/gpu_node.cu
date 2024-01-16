#include <cstddef>
#include <cstring>
#include <string>
#include <sys/types.h>
#include "gpu_node.h"
#include "cuda_param.h"

namespace SplitGPU {



GPU_node::GPU_node() {

    for(int i=SYMBOL_START;i<SYMBOL_END;i++) {
        func_id_map[cuda_func_name[i]] = i;
    }

    /* both using cuda driver api and cuda runtime api */
    cuInit(0);
    cuDeviceGet(&device, 0);
    cudaSetDevice(0);
    cuCtxGetCurrent(&context);

}

void GPU_node::exec_cuda(std::string cuda_name,char args[]) {

    auto item = func_id_map.find(cuda_name);
    if(item == func_id_map.end()) {
        printf("not find\n");
        return;
    }
        
    off_t offset = 0;
    switch (item->second) {
    case SYMBOL_cudaMalloc: {
        void** p_devPtr;
        size_t* p_size;
        UNPACK_PARAM(args,offset,p_devPtr);
        UNPACK_PARAM(args,offset,p_size);
        cudaError_t res = cudaMalloc(p_devPtr, *p_size);
        memcpy(&args[offset], &res, sizeof(res));
        printf("cudaMalloc\n");
    } break;
    case SYMBOL_cudaFree: {

    } break;
    case SYMBOL_cudaDeviceSynchronize: {
        cudaError_t res = cudaDeviceSynchronize();
        memcpy(&args[offset], &res, sizeof(res));
    };
    case SYMBOL_cudaMemcpy: {

    } break;
    default: {
        /*unregister cuda function*/
    }

    }
}

}