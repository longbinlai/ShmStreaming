package org.apache.hadoop.streaming;

import java.io.File;
import java.io.IOException;

import jtux.UConstant;
import jtux.UErrorException;
import jtux.UPosixIPC;
import jtux.USysVIPC;
import jtux.UUtil;

public class SharedMemory{
	private static int MAGIC = 0xBABE;
	private int MAXSHM;
	private int arrayid;
	private long shm_key;
	private long arrayAddr;
	private long fifo;
	
	private long sem_empty;
	private long sem_full;
	private String sem_empty_key;
	private String sem_full_key;
	
	private String keyFile;
	

	public SharedMemory(int bufferSize, String filePath) throws UErrorException, IOException{
		//Debug.writeTime();
		//Debug.writeDebug("In SharedMemory::constructor!\n");
		MAXSHM = bufferSize;
		arrayid = 0;
		shm_key = 0;
		arrayAddr = 0;
		initial_shm(filePath);
		fifo = UUtil.fifo_init(arrayAddr, MAXSHM, arrayid, 300, 300);
		//fifo = UUtil.fifo_init(arrayid, MAXSHM);
	}
	
	public SharedMemory(int bufferSize, String filePath, int time2UnlockEmptySem, 
			int time2UnlockFullSem) throws UErrorException, IOException{
		//Debug.writeTime();
		//Debug.writeDebug("In SharedMemory::constructor!\n");
		MAXSHM = bufferSize;
		arrayid = 0;
		shm_key = 0;
		arrayAddr = 0;
		if(!initial_shm(filePath))
			throw new IOException("Initiate shared memory error!");
		
		if(!initial_posix_sem())
			throw new IOException("Initiate semaphore error!");
		
		// We using arrayid as the key to initiate the semaphore of fifo
		//Debug.writeDebug("The arrayid = " + arrayid 
		//				+ "; time2unlockemptysem = " + time2UnlockEmptySem 
		//				+ "; time2unlockfullsem = " + time2UnlockFullSem + "!\n");
		fifo = UUtil.fifo_init(arrayAddr, MAXSHM, arrayid, time2UnlockEmptySem, time2UnlockFullSem);
		//Debug.writeDebug("After initiate fifo!\n");
	}
	
	public int getBufferSize(){
		return MAXSHM;
	}
	
	public long getShmAddr(){
		return arrayAddr;
	}
	
	public String getKeyFile(){
		return keyFile;
	}
	
	public long getFifo(){
		return fifo;
	}
	
	public long getSemEmpty(){
		return sem_empty;
	}
	
	public long getSemFull(){
		return sem_full;
	}
	
	@SuppressWarnings("finally")
	private boolean initial_posix_sem(){
		boolean res = true;
			try {
			  	sem_empty_key = "semempty" + String.valueOf(arrayid);
				sem_full_key = "semfull" + String.valueOf(arrayid);
			  	
				sem_empty = UPosixIPC.sem_open(sem_empty_key, UConstant.O_CREAT, 0666, 0);
				sem_full = UPosixIPC.sem_open(sem_full_key, UConstant.O_CREAT, 0666, 0);
			} catch (UErrorException e) {
				// TODO Auto-generated catch block
				res = false;
				e.printStackTrace();
			} finally {
				return res;
			}

	}
	
	@SuppressWarnings("finally")
	protected boolean initial_shm(String filePath) throws UErrorException, IOException{
		boolean res = false;
		File file = new File(filePath);
		if (!file.exists()){
			file.createNewFile();
		}
		keyFile = file.getAbsolutePath();
		shm_key = USysVIPC.ftok(keyFile, MAGIC);
		//Debug.writeDebug("Key file is: " + keyFile + "\n");
		//Debug.writeDebug("Shm-key is: " + shm_key + "\n");
		// Here we have to clarify that size of the shared memory
		// must be greater than that of the the fifo, which contains
		// the real data, since we maintain the fifo control information
		// in the extra space of the shared memory. And because of that,
		// we initiate the shared memory with size equals to <buffersize + 4096>
		int size = MAXSHM + 4096;
		arrayid = USysVIPC.shmget(shm_key, size, UConstant.IPC_CREAT | 0666);
		if (arrayid != -1) {
			res = true;
			arrayAddr = USysVIPC.shmat(arrayid, 0, UConstant.SHM_RND);
		}
		return res;

	}
	
	public void detached_shm(){
		//Debug.writeTime();
		//Debug.writeDebug("In SharedMemory::detached_shm!\n");
		try{
			if(arrayid != 0 && arrayid != -1){
				File file = new File(this.keyFile);
				if(file.exists())
					file.delete();
				UUtil.fifo_free(fifo);
				USysVIPC.shmdt(arrayAddr);
				USysVIPC.shmctl(arrayid, UConstant.IPC_RMID, null);
				
				//UPosixIPC.sem_close(sem_empty);
				//UPosixIPC.sem_close(sem_full);
				UPosixIPC.sem_unlink(sem_empty_key);
				UPosixIPC.sem_unlink(sem_full_key);
			}
		} catch (UErrorException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	// TODO end
}