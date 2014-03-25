package org.apache.hadoop.streaming.io;

import java.io.DataOutput;
import java.io.IOException;
import jtux.UUtil;

import org.apache.hadoop.io.BytesWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.streaming.Debug;
import org.apache.hadoop.streaming.IntByteArray;
import org.apache.hadoop.streaming.PipeMapRed;
import org.apache.hadoop.streaming.PosixSemaphore;
import org.apache.hadoop.streaming.SharedMemory;
import org.apache.hadoop.streaming.SysvSemaphore;

public class ShmInputWriter extends InputWriter<Object, Object> {
	private int MAXSHM;
	private long shmAddr;
	private long fifo;
	private int readCounter;
	// To temporarily store the key-value pair
	private byte [] data;
	private byte[] inputSeparator;
	private int kvRecord;
	private long just;
	private long now;
	private double time;
	
	private long sem_empty;
	private long sem_full;

	// private static int TOADD = 10;
	public void initialize(PipeMapRed pipeMapRed) throws IOException {
		//Debug.writeTime();
		//Debug.writeDebug("In ShmInputWriter::initialize!\n");
		super.initialize(pipeMapRed);
		inputSeparator = pipeMapRed.getInputSeparator();
		MAXSHM = pipeMapRed.getBufferSize();
		SharedMemory shm = pipeMapRed.getSharedMemory(0);
		shmAddr = shm.getShmAddr();
		fifo = shm.getFifo();
		
		sem_empty = shm.getSemEmpty();
		sem_full = shm.getSemFull();
		//Debug.writeDebug("The shmAddr = " + shmAddr + ".\n");
		// Write the sharedMemory key file to the outer process
		readCounter = 0;
		data = new byte[MAXSHM];
	}

	@Override
	public void writeKey(Object key) throws IOException {
		// TODO Auto-generated method stub
		writeUTF8(key, 0);
	}

	@Override
	public void writeValue(Object value) throws IOException {
		// TODO Auto-generated method stub
		writeUTF8(value, 1);
	}

	/** Write an object to the output stream using UTF-8 encoding
	 * 
	 * @param object
	 * @param type 0 stands for key; 1 stands for value
	 * @throws IOException
	 */
	
	private void writeUTF8(Object object, int type)
			throws IOException {
		byte[] bval;
		int valSize;
		if (object instanceof BytesWritable) {
			BytesWritable val = (BytesWritable) object;
			bval = val.getBytes();
			valSize = val.getLength();
		} else if (object instanceof Text) {
			Text val = (Text) object;
			bval = val.getBytes();
			valSize = val.getLength();
		} else {
			String sval = object.toString();
			bval = sval.getBytes("UTF-8");
			valSize = bval.length;
		}
		writeSharedMemory(type, bval, valSize);

	}

	private void writeSharedMemory(int type, byte[] bval, int valSize) {
		// Copy the object to the shared memroy
		int pos = readCounter + IntByteArray.COUNTBYTES;
		if (type == 0) {
			readCounter += (valSize + inputSeparator.length);
			if(readCounter > MAXSHM - IntByteArray.COUNTBYTES){
				// Data too large for the shared memory
				return;
			}
			// Debug.writeDebug("The key of " + valSize +
			// " bytes being writen.\n");
			// To copy the key into the container
			System.arraycopy(bval, 0, data, pos, valSize);
			pos += valSize;
			
			// To copy the separator between key and value
			System.arraycopy(inputSeparator, 0, data, pos,
					inputSeparator.length);
			pos += inputSeparator.length;
		} else {
			// Debug.writeDebug("Try to write value to the fifo!\n");
			// To copy the value into the container
			// Debug.writeDebug("The value of " + valSize +
			// " bytes being writen.\n");
			readCounter += valSize;
			if(readCounter > MAXSHM - IntByteArray.COUNTBYTES){
				// Data too large for the shared memory
				readCounter = 0;
				return;
			}
			System.arraycopy(bval, 0, data, pos, valSize);
			// Finally copy the readCounter
			System.arraycopy(IntByteArray.int2Bytes(readCounter), 0, data, 0,
					IntByteArray.COUNTBYTES);
			readCounter += IntByteArray.COUNTBYTES;
			// Debug.writeDebug("After write value, readCounter = "
			// + readCounter + ".\n");
			// Debug.writeDebug("Before writing to the fifo!\n");
			try {
				//Debug.writeDebug("Before write " + readCounter + "bytes of data to the fifo!\n");
				UUtil.jaddr_to_fifo(fifo, shmAddr, data, readCounter, sem_empty, sem_full);
				//UUtil.jaddr_to_fifo(fifo, shmAddr, data, readCounter);
				//Debug.writeDebug("After write data!\n");
				//Debug.writeDebug("Try to write " + readCounter + " bytes of data to fifo: " +
				//		new String(data, 0, readCounter) + "\n");
				// Reset the readCounter for the next round
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} finally {
				readCounter = 0;
			}
		}

	}

}