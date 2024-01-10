/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:04:38
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-04 18:59:04
 * @FilePath: /SplitGPU/hook/cuda_hook.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <signal.h>
#include <cuda_runtime.h>

#include "hook/hook.h"
#include "hook/macro_common.h"
#include "sg_log.h"
#include "split_gpu.h"
#include "client.h"
#include "ipc.h"

using namespace SplitGPU;

Client client(SHARE_MEMORY);

static bool init = true;

void signal_handler(int signum) {
    // if(!init) {
    //     client.close();
    //     exit(-1);
    // }
    // SG_LOG("close!");
    exit(0);
}

void signal_reg() {
    if(init) {
        init = false;
        if (signal(SIGINT, signal_handler) == SIG_ERR) {
            perror("Failed to register signal handler");
        }
    }
}

#define SIGREGISTER signal_reg();

cudaError_t cudaLaunchKernel (const void *func, dim3 gridDim, dim3 blockDim, void **args,
                                                         size_t sharedMem, cudaStream_t stream) {
                                                            
    //excuda_client_batch_filt_check_commit(client->pid,client->queue,args);
    cudaError_t (*lcudaLaunchKernel) (const void *func, dim3 gridDim, dim3 blockDim, void **args,
                                                         size_t sharedMem, cudaStream_t stream) = 
        (cudaError_t (*) (const void *func, dim3 gridDim, dim3 blockDim, void **args,
                                                         size_t sharedMem, cudaStream_t stream))
            dlsym(RTLD_NEXT, "cudaLaunchKernel");
    CHECK_RET(client.request(SplitGPU::REQ_TYPE_LAUNCH,nullptr,0));
    return lcudaLaunchKernel(func, gridDim, blockDim, args, sharedMem, stream);
}


/* ---- memory alloc ---- */

HOOK_C_API HOOK_DECL_EXPORT cudaError_t cudaMallocManaged(void **devPtr, size_t size, unsigned int flags) {
    signal_reg();
    //HOOK_TRACE_PROFILE("cudaMallocManaged");
    using func_ptr = cudaError_t (*)(void **, size_t, unsigned int);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("cudaMallocManaged"));
    HOOK_CHECK(func_entry);
    cudaError_t ret;
    RET cli_ret = client.request(SplitGPU::REQ_TYPE_ALLOC,nullptr,size);
    if(cli_ret == SplitGPU::RET_ERR) {
        /*exceed*/
        ret = cudaErrorMemoryAllocation;
    } else {
        ret = func_entry(devPtr, size, flags);
        CHECK_RET(client.request(SplitGPU::REQ_TYPE_ALLOC,*devPtr,size));
    }
    return ret;
}

HOOK_C_API HOOK_DECL_EXPORT cudaError_t cudaMalloc(void **devPtr, size_t size) {
    signal_reg();
    //HOOK_TRACE_PROFILE("cudaMalloc");
    using func_ptr = cudaError_t (*)(void **, size_t);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("cudaMalloc"));
    HOOK_CHECK(func_entry);
    cudaError_t ret;
    RET cli_ret = client.request(SplitGPU::REQ_TYPE_ALLOC,nullptr,size);
    if(cli_ret == SplitGPU::RET_ERR) {
        /*exceed*/
        ret = cudaErrorMemoryAllocation;
    } else {
        ret = func_entry(devPtr, size);
        CHECK_RET(client.request(SplitGPU::REQ_TYPE_ALLOC,*devPtr,size));
    }
    return ret;
}



HOOK_C_API HOOK_DECL_EXPORT cudaError_t cudaMallocAsync(void **devPtr, size_t size, cudaStream_t hStream) {
    signal_reg();
    //HOOK_TRACE_PROFILE("cudaMallocAsync");
    using func_ptr = cudaError_t (*)(void **, size_t, cudaStream_t);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("cudaMallocAsync"));
    HOOK_CHECK(func_entry);
    cudaError_t ret;
    RET cli_ret = client.request(SplitGPU::REQ_TYPE_ALLOC,nullptr,size);
    if(cli_ret == SplitGPU::RET_ERR) {
        /*exceed*/
        ret = cudaErrorMemoryAllocation;
    } else {
        ret = func_entry(devPtr, size, hStream);
        CHECK_RET(client.request(SplitGPU::REQ_TYPE_ALLOC,*devPtr,size));
    }
    return ret;

}

HOOK_C_API HOOK_DECL_EXPORT cudaError_t cudaFreeAsync(void *devPtr, cudaStream_t hStream) {
    signal_reg();
    //HOOK_TRACE_PROFILE("cudaFreeAsync");
    using func_ptr = cudaError_t (*)(void *, cudaStream_t);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("cudaFreeAsync"));
    HOOK_CHECK(func_entry);
    CHECK_RET(client.request(SplitGPU::REQ_TYPE_FREE,devPtr,0));
    return func_entry(devPtr, hStream);
}

HOOK_C_API HOOK_DECL_EXPORT cudaError_t cudaFree(void *devPtr) {
    signal_reg();
    //HOOK_TRACE_PROFILE("cudaFree");
    using func_ptr = cudaError_t (*)(void *);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("cudaFree"));
    HOOK_CHECK(func_entry);
    CHECK_RET(client.request(SplitGPU::REQ_TYPE_FREE,devPtr,0));
    return func_entry(devPtr);
}

/* ---- intern func ---- */

HOOK_C_API HOOK_DECL_EXPORT void **__cudaRegisterFatBinary(void *fatCubin) {
    signal_reg();
    //HOOK_TRACE_PROFILE("__cudaRegisterFatBinary");
    using func_ptr = void **(*)(void *);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterFatBinary"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubin);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterFatBinaryEnd(void **fatCubinHandle) {
    signal_reg();
    //HOOK_TRACE_PROFILE("__cudaRegisterFatBinaryEnd");
    using func_ptr = void (*)(void **);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterFatBinaryEnd"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaUnregisterFatBinary(void **fatCubinHandle) {
    //cleanup memory
    signal_reg();
    client.close();
    //HOOK_TRACE_PROFILE("__cudaUnregisterFatBinary");
    using func_ptr = void (*)(void **);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaUnregisterFatBinary"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterVar(void **fatCubinHandle, char *hostVar, char *deviceAddress,
                                                   const char *deviceName, int ext, size_t size, int constant,
                                                   int global) {
    signal_reg();
    //HOOK_TRACE_PROFILE("__cudaRegisterVar");
    using func_ptr = void (*)(void **, char *, char *, const char *, int, size_t, int, int);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterVar"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle, hostVar, deviceAddress, deviceName, ext, size, constant, global);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterManagedVar(void **fatCubinHandle, void **hostVarPtrAddress,
                                                          char *deviceAddress, const char *deviceName, int ext,
                                                          size_t size, int constant, int global) {
    signal_reg();
    //HOOK_TRACE_PROFILE("__cudaRegisterManagedVar");
    using func_ptr = void (*)(void **, void **, char *, const char *, int, size_t, int, int);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterManagedVar"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle, hostVarPtrAddress, deviceAddress, deviceName, ext, size, constant, global);
}

HOOK_C_API HOOK_DECL_EXPORT char __cudaInitModule(void **fatCubinHandle) {
    signal_reg();
    //HOOK_TRACE_PROFILE("__cudaInitModule");
    if(init) {
        init = false;
        if (signal(SIGINT, signal_handler) == SIG_ERR) {
            perror("Failed to register signal handler");
        }
    }
    client.connect();
    using func_ptr = char (*)(void **);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaInitModule"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterTexture(void **fatCubinHandle, const struct textureReference *hostVar,
                                                       const void **deviceAddress, const char *deviceName, int dim,
                                                       int norm, int ext) {
    signal_reg();
    //HOOK_TRACE_PROFILE("__cudaRegisterTexture");
    using func_ptr = void (*)(void **, const struct textureReference *, const void **, const char *, int, int, int);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterTexture"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle, hostVar, deviceAddress, deviceName, dim, norm, ext);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterSurface(void **fatCubinHandle, const struct surfaceReference *hostVar,
                                                       const void **deviceAddress, const char *deviceName, int dim,
                                                       int ext) {
    signal_reg();
    //HOOK_TRACE_PROFILE("__cudaRegisterSurface");
    using func_ptr = void (*)(void **, const struct surfaceReference *, const void **, const char *, int, int);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterSurface"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle, hostVar, deviceAddress, deviceName, dim, ext);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterFunction(void **fatCubinHandle, const char *hostFun, char *deviceFun,
                                                        const char *deviceName, int thread_limit, uint3 *tid,
                                                        uint3 *bid, dim3 *bDim, dim3 *gDim, int *wSize) {
    signal_reg();
    //HOOK_TRACE_PROFILE("__cudaRegisterFunction");
    client.connect();
    using func_ptr =
        void (*)(void **, const char *, char *, const char *, int, uint3 *, uint3 *, dim3 *, dim3 *, int *);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterFunction"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle, hostFun, deviceFun, deviceName, thread_limit, tid, bid, bDim, gDim, wSize);
}

HOOK_C_API HOOK_DECL_EXPORT cudaError_t __cudaPopCallConfiguration(dim3 *gridDim, dim3 *blockDim, size_t *sharedMem,
                                                                   void *stream) {
    signal_reg();
    //HOOK_TRACE_PROFILE("__cudaPopCallConfiguration");
    using func_ptr = cudaError_t (*)(dim3 *, dim3 *, size_t *, void *);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaPopCallConfiguration"));
    HOOK_CHECK(func_entry);
    return func_entry(gridDim, blockDim, sharedMem, stream);
}

HOOK_C_API HOOK_DECL_EXPORT unsigned __cudaPushCallConfiguration(dim3 gridDim, dim3 blockDim, size_t sharedMem,
                                                                 struct CUstream_st *stream) {
    signal_reg();
    //HOOK_TRACE_PROFILE("__cudaPushCallConfiguration");
    using func_ptr = unsigned (*)(dim3, dim3, size_t, struct CUstream_st *);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaPushCallConfiguration"));
    HOOK_CHECK(func_entry);
    return func_entry(gridDim, blockDim, sharedMem, stream);
}