/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:04:38
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-01-02 11:32:35
 * @FilePath: /SplitGPU/hook/cuda_hook.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>
#include <cuda_runtime.h>

#include "hook/hook.h"
#include "hook/macro_common.h"
#include "split_gpu.h"
#include "client.h"
#include "ipc.h"

using namespace SplitGPU;

Client client(SHARE_MEMORY);

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

/* ---- intern func ---- */

HOOK_C_API HOOK_DECL_EXPORT void **__cudaRegisterFatBinary(void *fatCubin) {
    //HOOK_TRACE_PROFILE("__cudaRegisterFatBinary");
    using func_ptr = void **(*)(void *);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterFatBinary"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubin);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterFatBinaryEnd(void **fatCubinHandle) {
    //HOOK_TRACE_PROFILE("__cudaRegisterFatBinaryEnd");
    using func_ptr = void (*)(void **);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterFatBinaryEnd"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaUnregisterFatBinary(void **fatCubinHandle) {
    //cleanup memory
    //HOOK_TRACE_PROFILE("__cudaUnregisterFatBinary");
    using func_ptr = void (*)(void **);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaUnregisterFatBinary"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterVar(void **fatCubinHandle, char *hostVar, char *deviceAddress,
                                                   const char *deviceName, int ext, size_t size, int constant,
                                                   int global) {
    //HOOK_TRACE_PROFILE("__cudaRegisterVar");
    using func_ptr = void (*)(void **, char *, char *, const char *, int, size_t, int, int);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterVar"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle, hostVar, deviceAddress, deviceName, ext, size, constant, global);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterManagedVar(void **fatCubinHandle, void **hostVarPtrAddress,
                                                          char *deviceAddress, const char *deviceName, int ext,
                                                          size_t size, int constant, int global) {
    //HOOK_TRACE_PROFILE("__cudaRegisterManagedVar");
    using func_ptr = void (*)(void **, void **, char *, const char *, int, size_t, int, int);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterManagedVar"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle, hostVarPtrAddress, deviceAddress, deviceName, ext, size, constant, global);
}

HOOK_C_API HOOK_DECL_EXPORT char __cudaInitModule(void **fatCubinHandle) {
    //HOOK_TRACE_PROFILE("__cudaInitModule");
    client.connect();
    using func_ptr = char (*)(void **);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaInitModule"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterTexture(void **fatCubinHandle, const struct textureReference *hostVar,
                                                       const void **deviceAddress, const char *deviceName, int dim,
                                                       int norm, int ext) {
    //HOOK_TRACE_PROFILE("__cudaRegisterTexture");
    using func_ptr = void (*)(void **, const struct textureReference *, const void **, const char *, int, int, int);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterTexture"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle, hostVar, deviceAddress, deviceName, dim, norm, ext);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterSurface(void **fatCubinHandle, const struct surfaceReference *hostVar,
                                                       const void **deviceAddress, const char *deviceName, int dim,
                                                       int ext) {
    //HOOK_TRACE_PROFILE("__cudaRegisterSurface");
    using func_ptr = void (*)(void **, const struct surfaceReference *, const void **, const char *, int, int);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaRegisterSurface"));
    HOOK_CHECK(func_entry);
    return func_entry(fatCubinHandle, hostVar, deviceAddress, deviceName, dim, ext);
}

HOOK_C_API HOOK_DECL_EXPORT void __cudaRegisterFunction(void **fatCubinHandle, const char *hostFun, char *deviceFun,
                                                        const char *deviceName, int thread_limit, uint3 *tid,
                                                        uint3 *bid, dim3 *bDim, dim3 *gDim, int *wSize) {
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
    //HOOK_TRACE_PROFILE("__cudaPopCallConfiguration");
    using func_ptr = cudaError_t (*)(dim3 *, dim3 *, size_t *, void *);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaPopCallConfiguration"));
    HOOK_CHECK(func_entry);
    return func_entry(gridDim, blockDim, sharedMem, stream);
}

HOOK_C_API HOOK_DECL_EXPORT unsigned __cudaPushCallConfiguration(dim3 gridDim, dim3 blockDim, size_t sharedMem,
                                                                 struct CUstream_st *stream) {
    //HOOK_TRACE_PROFILE("__cudaPushCallConfiguration");
    using func_ptr = unsigned (*)(dim3, dim3, size_t, struct CUstream_st *);
    static auto func_entry = reinterpret_cast<func_ptr>(HOOK_CUDART_SYMBOL("__cudaPushCallConfiguration"));
    HOOK_CHECK(func_entry);
    return func_entry(gridDim, blockDim, sharedMem, stream);
}