#include <stdio.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <unistd.h>

#define CUDA_CHECK(call)\
{\
  const cudaError_t error=call;\
  if(error!=cudaSuccess)\
  {\
      printf("\033[31;31m<%s line:%d %s> ERROR \033[0m",__FUNCTION__,__LINE__,__TIME__);\
      printf("code:%d,reason:%s\n",error,cudaGetErrorString(error));\
      exit(1);\
  }\
}


__global__ void parallel_add_val(void* dptr,int add,size_t n) {

    char* val = (char*)dptr;
    size_t i = blockIdx.x * blockDim.x + threadIdx.x;
    if(i==1) {
        // if(val[i]!=0) {
        //     printf("kernel not zero\n");
        // }
        //printf("Yes add-kernel start!\n");
    }
    if(i<n) {
        val[i] += add;
    }
        
}

void test_parallel_add(void* dptr,int add,size_t n) {
    dim3 block(32);
    dim3 grid((n + block.x -1)/block.x);
    //dim3 grid((n)/block.x);
    cudaDeviceSynchronize();
    
    parallel_add_val<<<grid,block>>>(dptr,add,n);
    auto func = parallel_add_val;
    printf("func addr:%p\n",(void*)(*func));
    cudaDeviceSynchronize();
    //printf("Yes add-kernel finsh!\n");
    void* host_ptr = malloc(n);
    cudaMemcpy(host_ptr, dptr, n, cudaMemcpyDeviceToHost);
    //check
    for(int i=0;i<n;i+=10) {
        if( ((char*)host_ptr)[i] != add ) {
            printf("check fail,%d",i);
            exit(-1);
        }
    }
    printf("%p %p %p\n",&dptr,nullptr,&n);
    printf("%ld %ld\n",sizeof(dptr),sizeof(n));
    parallel_add_val<<<grid,block>>>(dptr,-1*add,n);
    printf("check pass!\n");
}

int main() {

    // auto ret = cuInit(0);
    // printf("init ret:%d\n",ret);
    // CUdeviceptr cudptr;
    // cuMemAlloc(&cudptr,1024);
    void* dptr, *dptr2;
    char* ptr,*cptr;
    size_t size = 1024;
    auto res = cudaMalloc(&dptr,size);
    res = cudaMalloc(&dptr2,size);
    ptr = (char*)malloc(size);
    cptr = (char*)malloc(size);
    for(int i=0;i<size;i++) {
        ptr[i] = i%10;
    }
    printf("res %d\n",res);
    res = cudaMemcpy(dptr, ptr, size, cudaMemcpyHostToDevice);
    printf("res %d\n",res);
    res = cudaMemcpy(cptr, dptr, size, cudaMemcpyDeviceToHost);
    printf("res %d\n",res);
    for(int i=0;i<size;i++) {
        if(cptr[i] != i%10) {
            printf("fail,i %d, cptr:%d\n",i,cptr[i]);
            exit(0);
        }
    }
    printf("pass!\n");
    return 0;
}
