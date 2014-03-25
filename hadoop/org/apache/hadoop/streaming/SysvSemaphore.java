package org.apache.hadoop.streaming;

import java.io.File;

import jtux.UConstant;
import jtux.UErrorException;
import jtux.USysVIPC;

public class SysvSemaphore{
	public static int MAGIC = 0xBABE;
	public static int SEM_MUTEX = 0;
	public static int SEM_EMPTY = 1;
	public static int SEM_FULL = 2;
	
	private USysVIPC.s_sembuf[] P;
	private USysVIPC.s_sembuf[] V;
	
	private long sem_key;
	private int sysv_sem;	
	
	public SysvSemaphore(String filePath){
		//Debug.writeTime();
		//Debug.writeDebug("In SysvSemaphore::contructor!\n");
		sem_key = 0;
		sysv_sem = 0;
		initial_sysv_sem(filePath);		
	}
	
	@SuppressWarnings("finally")
	/***
	 * @author robeen
	 * @param filePath The file to generate the semaphore key using ftok
	 */
	protected boolean initial_sysv_sem(String filePath){
		boolean res = false;
		try {
			File keyfile = new File(filePath);
			if(!keyfile.exists()) 
				keyfile.createNewFile();
			sem_key = USysVIPC.ftok(keyfile.getAbsolutePath(), MAGIC);
			sysv_sem = USysVIPC.semget(sem_key, 3, UConstant.IPC_CREAT | 0666);
			P = new USysVIPC.s_sembuf[1];
			P[0] = new USysVIPC.s_sembuf();
			V = new USysVIPC.s_sembuf[1];
			V[0] = new USysVIPC.s_sembuf();
			if(sysv_sem < 0)
				res = false;
			else {
				res = true;
				USysVIPC.u_semun_int arg = new USysVIPC.u_semun_int();
				arg.val = 1; // Initial the sem_mutex = 1			
				USysVIPC.semctl(sysv_sem, SEM_MUTEX, UConstant.SETVAL, arg);
				arg.val = 0; // Initial the sem_empty = 0
				USysVIPC.semctl(sysv_sem, SEM_EMPTY, UConstant.SETVAL, arg);
				arg.val = 1; // Initial the sem_full = 1
				USysVIPC.semctl(sysv_sem, SEM_FULL, UConstant.SETVAL, arg);
			}
			
		} catch (UErrorException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally{
			return res;
		}
		
	}
	
	public void remove_sysv_sem(){
		try {
			//USysVIPC.u_semun_int arg = new USysVIPC.u_semun_int();
			//arg.val = 0;
			USysVIPC.semctl(sysv_sem, SEM_MUTEX, UConstant.IPC_RMID, null);
			USysVIPC.semctl(sysv_sem, SEM_EMPTY, UConstant.IPC_RMID, null);
			USysVIPC.semctl(sysv_sem, SEM_FULL, UConstant.IPC_RMID, null);
		} catch (UErrorException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void sysv_P(int sem) throws Exception{
		if(sem < 0 || sem >= 3 || P == null){
			throw new Exception();
		}
		P[0].sem_flg = 0;
		P[0].sem_num = (short) sem;
		P[0].sem_op = -1;

		USysVIPC.semop(sysv_sem, P, 1);
	}
	
	public void sysv_V(int sem) throws Exception{
		if(sem < 0 || sem >= 3 || V == null){
			new Exception();
		}
		V[0].sem_flg = 0;
		V[0].sem_num = (short) sem;
		V[0].sem_op = 1;
		
		USysVIPC.semop(sysv_sem, V, 1);
	}
}