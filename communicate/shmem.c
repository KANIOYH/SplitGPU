/*
 * @Author: yh chen yh_chan_kanio@163.com
 * @Date: 2023-12-29 17:03:08
 * @LastEditors: yh chen yh_chan_kanio@163.com
 * @LastEditTime: 2023-12-30 19:46:35
 * @FilePath: /SplitGPU/communicate/shmem.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */


#include "shmem.h"

int sg_init_shmem(sg_shmem* shm) {
    shm->key = ftok("/tmp/sg_shm", 'A'); 
    shm->shmid = shmget(shm->key, SHM_SIZE, IPC_CREAT | 0666);
    if (shm->shmid == -1) {
        perror("shmget");
        exit(1);
    }

    shm->shmaddr = (char*) shmat(shm->shmid, NULL, 0);
    if (shm->shmaddr == (char*)-1) {
        perror("shmat");
        exit(1);
    }
    return 0;
}

int sg_create_shmem(sg_shmem** pp_shm) {
    sg_shmem *shm;
    shm = (sg_shmem *)malloc(sizeof(sg_shmem) + SHM_SIZE);
    sg_init_shmem(shm);
    *pp_shm = shm;
    return 0;
}

int sg_free_shmem(sg_shmem* shm) {
    if (shmdt(shm->shmaddr) == -1) {
        perror("shmdt");
        exit(1);
    }
    shmctl(shm->shmid, IPC_RMID, NULL);
    return 0;
}


