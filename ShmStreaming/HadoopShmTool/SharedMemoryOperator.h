/*
 * =====================================================================================
 *
 *       Filename:  SharedMemoryOperator.h
 *
 *
 *        Version:  1.0
 *        Created:  2012年02月10日 13时43分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Robeenly (), robeenly@gmail.com
 *        Company:  SJTU
 *
 * =====================================================================================
 */
#include "SharedMemoryUtil.h"
#include "unlock_fifo.h"
#include <string.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <fstream>

using namespace std;

typedef unsigned char uchar;

static int MASK[4] = {0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000};
static int COUNTBYTES = sizeof(int); 

static inline int charArray2Int(char* input){
  	int res = 0x0000;
	int i = 0;
	uchar bit[COUNTBYTES];
	if(input == NULL){
	  	return -1;
	}
	for(i = 0; i < COUNTBYTES; i++){
	  	bit[i] = (uchar)input[i];
		res |= ((bit[i] << (8 * i)) & MASK[i]);
	}

	return res;

}

static inline void int2CharArray(int count, char* bit){
	int i = 0;
	if(bit == NULL){
		bit = (char *)malloc(COUNTBYTES);
	}
  	memset(bit, '0', COUNTBYTES);
	for(i = 0; i < COUNTBYTES; i++){
	  	bit[i] = (char)((count >> (8 * i)) & MASK[0]);
	}
}

class SharedMemoryOperator{
private:
	int shmid;
	int buffersize;
	char* buffer;
	PUFIFO fifo;
	ofstream fout;
	sem_t * sem_empty;
	sem_t * sem_full;
	char * sem_empty_key;
	char * sem_full_key;

	void fifo_read_counter(void* buf){
		fifo_read_full(buf, COUNTBYTES);
	}

	void fifo_read_full(void* buf, unsigned int count){
		unsigned int pos = 0;
		unsigned int copied = 0;
		unsigned int left = count;
		if(count <= 0)
			return;

		while(true){
			copied = ufifo_out(fifo, (char *)buf + pos, left, sem_empty, sem_full);
			//if(copied == 0){
			//  	fout << "Read nothing from fifo!" << endl;
		//	}
			left -= copied;
			pos  += copied;
			if(left <= 0)
				break;
		}
	}

	int fifo_read(void* buf){
	    	int rcnt = 0;
		char* in = (char *)malloc(COUNTBYTES);
		fifo_read_counter(in);
		rcnt = charArray2Int(in);
		int sem_empty_value, sem_full_value;

		//fout << "After read counter, the counter being read is: "
		//     << rcnt << ";" << endl;
		if(rcnt == -1)
		  	return -1;

		fifo_read_full(buf, rcnt);

		//string data;
		//data.assign((char *)buf, rcnt);
		//fout <<	"After read data from fifo, Current fifo: " << endl
		//     << "; fifo->stat_empty = " << fifo->stat_empty
		//     << "; fifo->stat_full = " << fifo->stat_full
		//     << "; fifo->times_empty = " << fifo->times_empty
		//     << "; fifo->times_full = " << fifo->times_full << endl;
		//fout << "After read data, the data being read is: " << endl
		//     <<  data << endl << endl;

		free(in);
		return rcnt;
	}

	void fifo_write_counter(void* buf){
	  	fifo_write_full(buf, COUNTBYTES);
	}

	void fifo_write_full(void* buf, unsigned int count){
	  	unsigned int pos = 0;
		unsigned int copied = 0;
		unsigned int left = count;
		if(count <= 0)
		  	return;
		while(true){
		  	copied = ufifo_in(fifo, (char *)buf + pos, left, sem_empty, sem_full);
			left -= copied;
			pos  += copied;
			if(left <= 0)
			  	break;
		}
	}

	void fifo_write(void* buf, int count){
		if((count <= 0) && (count != -1))
			return;
		char* cb;
		cb = (char *)malloc(COUNTBYTES);
		int2CharArray(count, cb);

		if(count == -1){
		//	fout << "Write -1 back to hadoop!" << endl
		//             << "; fifo->stat_empty = " << fifo->stat_empty
		//             << "; fifo->stat_full = " << fifo->stat_full << endl;

			fifo_write_counter((void *)cb);
			return;
		}
		string line((char *)buf);
		//fout << "There are " << count  
		//     << " bytes of data to be writen, they are: " << endl;
		//fout << line << endl;
		fifo_write_counter((void *)cb);
		fifo_write_full(buf, count);

		//fout <<	"After write " << count << " bytes of data to fifo, Current fifo: " << endl
		//     << "; fifo->stat_empty = " << fifo->stat_empty
		//     << "; fifo->stat_full = " << fifo->stat_full << endl;
		//fout << "After read data, the data being read is: " << endl
		//fout << "After write!" << endl;

	}


	int initSharedMemory(){
		fout.open("/home/epcc/robeen/debug", ofstream::app);
		stringstream ss;
		char * debug = const_cast<char *>(ss.str().c_str());
		ss.str("");
		string line;
		getline(cin, line);
		char* keyfile = const_cast<char *>(line.c_str());
		int size = buffersize + 4096;
		// Here we have to clarify that size of the shared memory 
		// must be greater than that of the the fifo, which contains 
		// the real data, since we maintain the fifo control information
		// in the extra space of the shared memory. And because of that, 
		// we initiate the shared memory with size equals to <buffersize + 4096>
		shmid = initShmID(keyfile, size);
		//fout << "Get shared memory Id as: " << shmid << endl ;
		buffer = initShmBuffer(shmid);

		fifo = ufifo_init(buffer, buffersize, shmid, 300, 300);
		//fout << "fifo->TIME_TO_UNLOCK_EMPTY_SEM = " << fifo->TIME_TO_UNLOCK_EMPTY_SEM
		//     << "; fifo->TIME_TO_UNLOCK_FULL_SEM = " << fifo->TIME_TO_UNLOCK_FULL_SEM << endl;

		//sem_empty_key = (char *)malloc(50);
		//memset(sem_empty_key, '0', 50);
		//sem_full_key = (char *)malloc(50);
		//memset(sem_full_key, '0', 50);
		//sprintf(sem_empty_key, "sem_empty_%d", shmid);
		//sprintf(sem_full_key, "semm_full_%d", shmid);
		ss << "empty" << shmid;
		sem_empty_key = const_cast<char *>(ss.str().c_str());
		ss.str("");
		ss << "full" << shmid;
		sem_full_key = const_cast<char *>(ss.str().c_str());

		//fout << "sem_empty_key is: " << sem_empty_key << endl;
		//fout << "sem_full_key is: " << sem_full_key << endl;


		sem_empty = sem_open(sem_empty_key, 0666);
		sem_full = sem_open(sem_full_key, 0666);

		//fout << "The address of semaphore: &sem_empty = " << (unsigned long)&sem_empty
		//     << "; &sem_full = " << (unsigned long)&sem_full << endl;


		if(fifo == NULL || sem_empty == SEM_FAILED || sem_full == SEM_FAILED){
			//fout << "Initiate fifo or semaphore error!" << endl;
		  	return -1;
		}
	//	fout << "The address of the fifo is: " << &fifo << endl;

		return 0;
	}


public:
  	SharedMemoryOperator(){
		shmid = 0;
		buffersize = 4096;
		if(initSharedMemory() == -1){
		  	throw exception();
		}

	}

	// Size will be authomatically expand to time of 4096
	SharedMemoryOperator(int size){
		shmid = 0;
		buffersize = ((size % 4096) == 0) ? size : 
		  		(size + (4096 - (size % 4096)));
		if(initSharedMemory() == -1){
		  	throw exception();
		}
	}

	~SharedMemoryOperator(){
	  	fout.close();
	  	clearShm();
	}

	int getshmid(){
	  	return shmid;
	}

	int getBufferSize(){
	  	return buffersize;
	}

	void clearShm(){
	//  	ufifo_free(fifo);
		detached_shm(shmid, buffer);
		sem_unlink(sem_empty_key);
		sem_unlink(sem_full_key);
	}

	int readSharedMemory(char* output){
	  	if(output == NULL){
			output = (char *)malloc(buffersize);
			memset(output, '0', buffersize);
		}
		return fifo_read(output);

	}

	void writeSharedMemory(char* input, int size){
	  	fifo_write(input, size);
		// Which means reaching the end of current task, 
		// We should check whether current read process has been blocked since buffer is empty, 
		// and release the semaphore if needed.
		if(input == NULL){
			if(fifo->stat_empty == 1){
		  		sem_post(sem_empty);
			}
		}

	}

	//void writeDebug(string message){
	//  	fout << message;
//	}

};
