/*
 * @Author: Yamphy Chan && yh_chan_kanio@163.com
 * @Date: 2023-09-27 09:39:17
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2024-04-08 23:37:08
 * @FilePath: /SplitGPU/communicate/shmem.h
 * @Description: 
 * 
 */
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define SHM_SIZE 204800

struct shmem_t {
    int shmid;
    key_t key;
    char *shmaddr;
};
typedef struct shmem_t sg_shmem;


int sg_init_shmem(sg_shmem* shm);

int sg_create_shmem(sg_shmem** pp_shm);

int sg_free_shmem(sg_shmem* shm);






