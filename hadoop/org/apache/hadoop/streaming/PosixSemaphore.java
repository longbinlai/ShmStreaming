package org.apache.hadoop.streaming;

import jtux.UConstant;
import jtux.UErrorException;
import jtux.UPosixIPC;

public class PosixSemaphore{
	private long sem_mutex;
	private long sem_empty;
	private long sem_full;
	
	public static int SEM_MUTEX = 0;
	public static int SEM_EMPTY = 1;
	public static int SEM_FULL = 2;
	
	private String mutex;
	private String empty;
	private String full;
	
	
	/**
	 * The PosixSemaphore constructor
	 * @param name String array as to contain mutex, empty and full respectively
	 */
	public PosixSemaphore(String[] name){
		sem_mutex = 0;
		sem_empty = 0;
		sem_full = 0;
		mutex = new String(name[SEM_MUTEX]);
		empty = new String(name[SEM_EMPTY]);
		full = new String(name[SEM_FULL]);
		
		initPosixSem();
	}
	
	/**
	 * Should specify the mutex, empty and full semaphore, respectively
	 * @param mutex
	 * @param empty
	 * @param full
	 * @return true if initiate successfully, otherwise false
	 */
	@SuppressWarnings("finally")
	private boolean initPosixSem(){
		boolean res = true;
		try {
			sem_mutex = UPosixIPC.sem_open(mutex, UConstant.O_CREAT, 0644, 1);
			sem_empty = UPosixIPC.sem_open(empty, UConstant.O_CREAT, 0644, 0);
			sem_full = UPosixIPC.sem_open(full, UConstant.O_CREAT, 0644, 1);
		} catch (UErrorException e) {
			// TODO Auto-generated catch block
			res = false;
			e.printStackTrace();
		} finally {
			return res;
		}
		
	}
	
	public void unlinkPosixSem(){
		try {
			UPosixIPC.sem_unlink(mutex);
			UPosixIPC.sem_unlink(empty);
			UPosixIPC.sem_unlink(full);
		} catch (UErrorException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public boolean P(int opr){
		boolean res = true;
		try {
			if (opr == SEM_MUTEX) {
				UPosixIPC.sem_wait(sem_mutex);
			} else if (opr == SEM_EMPTY){
				UPosixIPC.sem_wait(sem_empty);
			} else if(opr == SEM_FULL){
				UPosixIPC.sem_wait(sem_full);
			} else {
				res = false;
			}
		} catch (UErrorException e) {
			res = false;
			e.printStackTrace();
		} finally {
			return res;
		}
	}
	
	public boolean V(int opr){
		boolean res = true;
		try {
			if (opr == SEM_MUTEX) {
				UPosixIPC.sem_post(sem_mutex);
			} else if (opr == SEM_EMPTY){
				UPosixIPC.sem_post(sem_empty);
			} else if(opr == SEM_FULL){
				UPosixIPC.sem_post(sem_full);
			} else {
				res = false;
			}
		} catch (UErrorException e) {
			res = false;
			e.printStackTrace();
		} finally {
			return res;
		}
	}
	
	public String getSemMutex(){
		return mutex;
	}
	
	public String getSemEmpty(){
		return empty;
	}
	
	public String getSemFull(){
		return full;
	}
	
	
}