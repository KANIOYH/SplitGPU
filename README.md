<!--
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2024-02-06 20:32:36
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-02-06 20:34:52
 * @FilePath: /SplitGPU/README.md
 * @Description: 
 * 
-->

# SplitGPU

A simple GPU cuda container-level virtualization project

## Features:
- Lock-free message (request) queue based on shared memory to accelerate Guest CUDA request delivery performance
- Implement over-division of video memory resources through UVM to alleviate the problem of GPU memory wall
- Implement GPU computing power with different weights during time slice rotation, and intercept instructions issued by the kernel function
- (Supportable) Remote call mode can intercept device code execution stack and parse cuda ptx to obtain kernel function execution information (RPC mode)

## overview
- Scheduling manager: The issued CUDA requests are recorded and managed, and the scheduling kernel function delivers the GPU rate and manages storage resources.
- Tenant adjuster: open internal dynamic adjustment service, the system can hot update tenant weights, add or delete tenants
- Tenant CUDA: a dynamic interception library that is transparently inserted into CUDA programs and communicates implicitly with the scheduling manager
- Container supervisor: monitor CUDA program messages in the container and report exceptions
