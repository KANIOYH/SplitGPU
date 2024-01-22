#include <cstdio>
#include <cuda.h>
#include <stdio.h>
#include <string>
#include <vector>
int main() {
    int N = 128;
    size_t size = N * sizeof(float);
    // Allocate the host input vector A
    float *h_A = (float *)malloc(size);

    // Allocate the host input vector B
    float *h_B = (float *)malloc(size);

    // Allocate the host output vector C
    float *h_C = (float *)malloc(size);
    float *h_Cb = (float *)malloc(size);

    for (int i = 0; i < N; ++i) {
        h_A[i] = i;
        h_B[i] = i;
        h_C[i] = h_A[i]+h_B[i]+1;
    }
    CUresult res;
    cuInit(0);
    CUdevice device;
    res = cuDeviceGet(&device, 0);
    if (res != CUDA_SUCCESS){
        printf("cuDeviceGet\n");
        exit(EXIT_FAILURE);
    }
    CUcontext context;
    res = cuCtxCreate(&context, 0, device);
    if (res != CUDA_SUCCESS){
        printf("cuCtxCreate\n");
        exit(EXIT_FAILURE);
    }

    CUdeviceptr d_A;
    cuMemAlloc(&d_A, size);
    CUdeviceptr d_B;
    cuMemAlloc(&d_B, size);
    CUdeviceptr d_C;
    cuMemAlloc(&d_C, size);
    cuMemcpyHtoD(d_A, h_A, size);
    cuMemcpyHtoD(d_B, h_B, size);
    CUmodule module;
    res = cuModuleLoad(&module, "/home/chenyuanhui/project/SplitGPU/data.fatbin");
    if (res != CUDA_SUCCESS){
        printf("cuModuleLoad\n");
        exit(EXIT_FAILURE);
    }    
    CUfunction f;
    res = cuModuleGetFunction(&f, module, "_Z14parallel_helloi");
    if (res != CUDA_SUCCESS){
        printf("cuModuleGetFunction\n");
        exit(EXIT_FAILURE);
    }    
    unsigned char* header = (unsigned char*)f;
    int threadsPerBlock = 256;
    int blocksPerGrid =
            (N + threadsPerBlock - 1) / threadsPerBlock;
    int val = 1;
    void* args[] = { &val};
    cuLaunchKernel(f,
                   blocksPerGrid, 1, 1, threadsPerBlock, 1, 1,
                   0, 0, args, 0);
    cuCtxSynchronize();

    // CUmodule module;
    // res = cuModuleLoad(&module, "/home/chenyuanhui/project/cuda_hook/sample/cuda/vector_add.fatbin");
    // if (res != CUDA_SUCCESS){
    //     printf("cuModuleLoad\n");
    //     exit(EXIT_FAILURE);
    // }    
    // CUfunction vecAdd;
    // res = cuModuleGetFunction(&vecAdd, module, "_Z9vectorAddPKfS0_Pfi");
    // if (res != CUDA_SUCCESS){
    //     printf("cuModuleGetFunction\n");
    //     exit(EXIT_FAILURE);
    // }    
    // unsigned char* header = (unsigned char*)vecAdd;

    // for(int i=0;i<32;i++) {
    //     if(i%4==0)
    //         printf("\n");
    //     unsigned char v = header[i];
    //     printf("%02x ",v);
    // }
    // int threadsPerBlock = 256;
    // int blocksPerGrid =
    //         (N + threadsPerBlock - 1) / threadsPerBlock;

    // void* args[] = { &d_A, &d_B, &d_C, &N ,&threadsPerBlock};
    // cuLaunchKernel(vecAdd,
    //                blocksPerGrid, 1, 1, threadsPerBlock, 1, 1,
    //                0, 0, args, 0);
    // cuCtxSynchronize();
    // cuMemcpyDtoH(h_Cb, d_C, size);
    // for (int i = 0; i < N; ++i) {
    //     if( abs(h_Cb[i]-h_C[i]) > 0.01 ) {
    //         printf("umatch %f %f %d\n",h_Cb[i],h_C[i],i);
    //         exit(-1);
    //     }
    // }
    printf("aaa check pass!\n");
    return 0;
}