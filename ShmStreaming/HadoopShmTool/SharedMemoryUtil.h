/*
 * =====================================================================================
 *
 *       Filename:  SharedMemroyUtil.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年02月10日 11时32分47秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Robeenly (), robeenly@gmail.com
 *        Company:  SJTU
 *
 * =====================================================================================
 */
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

static int MAGIC = 0xBABE;

int initShmID(char* filePath, int szBuffer){
	int shm_key = ftok(filePath, MAGIC);
	return shmget(shm_key, szBuffer, IPC_CREAT | 0666);
}

char* initShmBuffer(int shmID){
  	char* buffer;
	if(shmID != -1){
		buffer = (char*)shmat(shmID, 0, SHM_RND);
		return buffer;
	} else {
		return NULL;
	}
}

void detached_shm(long shmID, char* buffer){
  	shmdt(buffer);
	shmctl(shmID, IPC_RMID, 0);
}
  
