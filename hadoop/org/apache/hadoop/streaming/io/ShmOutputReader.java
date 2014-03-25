/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.apache.hadoop.streaming.io;

import java.io.DataInput;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.nio.charset.CharacterCodingException;

import jtux.UUtil;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.streaming.Debug;
import org.apache.hadoop.streaming.IntByteArray;
import org.apache.hadoop.streaming.PipeMapRed;
import org.apache.hadoop.streaming.SharedMemory;
import org.apache.hadoop.streaming.StreamKeyValUtil;
import org.apache.hadoop.util.StringUtils;
import org.apache.hadoop.util.UTF8ByteArrayUtils;

/**
 * OutputReader that reads the client's output as text.
 */
public class ShmOutputReader extends OutputReader<Text, Text> {
	private byte[] data;
	private int numKeyFields;
	private byte[] separator;
	private byte[] keyValueSeparator;
	private Text key;
	private Text value;
	
	private int MAXSHM;
	private long shmAddr;
	private long fifo;
	private int readCounter;
	
	private long sem_empty;
	private long sem_full;
	
	private int readCounter(){
		int rcnt = 0;
		byte[] in = new byte[IntByteArray.COUNTBYTES];
		UUtil.jaddr_from_fifo(fifo, shmAddr, in, IntByteArray.COUNTBYTES, sem_empty, sem_full);
		//UUtil.jaddr_from_fifo(fifo, shmAddr, in, IntByteArray.COUNTBYTES);
		rcnt = IntByteArray.bytes2Int(in);
		return rcnt;
	}
	
	private int readSharedMemory(){
		int rcnt = readCounter();
		if(rcnt != -1){
			readCounter = rcnt;
			//Debug.writeDebug("Try to read " + rcnt + " bytes of data from fifo!\n");
			UUtil.jaddr_from_fifo(fifo, shmAddr, data, rcnt, sem_empty, sem_full);
			//Debug.writeDebug("After read data from fifo!\n");
			//UUtil.jaddr_from_fifo(fifo, shmAddr, data, rcnt);
		}
		return rcnt;
	}

	@Override
	public void initialize(PipeMapRed pipeMapRed) throws IOException {
		super.initialize(pipeMapRed);
		//Debug.writeTime();
		//Debug.writeDebug("In ShmOutputReader::initialize!\n");
		numKeyFields = pipeMapRed.getNumOfKeyFields();
		separator = pipeMapRed.getFieldSeparator();
		keyValueSeparator = pipeMapRed.getKeyValueSeparator();
		SharedMemory shm = pipeMapRed.getSharedMemory(1);
		MAXSHM = shm.getBufferSize();
		shmAddr = shm.getShmAddr();
		fifo = shm.getFifo();
		data = new byte[MAXSHM];
		
		sem_empty = shm.getSemEmpty();
		sem_full = shm.getSemFull();

		key = new Text();
		value = new Text();
		readCounter = 0;
	}

	@Override
	public boolean readKeyValue() throws IOException {
		//Debug.writeTime();
		//Debug.writeDebug("In ShmOutputReader::readKeyValue!\n");
		//Debug.writeDebug("Before read, the fifo is: fifo -> in = "
		//		+ UUtil.fifo_in(fifo) + "; fifo -> out = "
		//		+ UUtil.fifo_out(fifo) + "; \n");

		int rcnt = readSharedMemory();
		
		//Debug.writeDebug("After read, the fifo is: fifo -> in = "
		//		+ UUtil.fifo_in(fifo) + "; fifo -> out = "
		//		+ UUtil.fifo_out(fifo) + "; \n");

		if(rcnt == -1){
			//Debug.writeDebug("Accept -1 from tool, end of read key/value!\n");
			return false;
		}
		//Debug.writeDebug("Try to read " + rcnt + " bytes of data from outer process!\n");
		byte[] realData = new byte[rcnt];
		System.arraycopy(data, 0, realData, 0, rcnt);
		//Debug.writeDebug("Data: " + new String(realData) + "\n");
		splitKeyVal(realData, rcnt, key, value);
		return true;
	}

	@Override
	public Text getCurrentKey() throws IOException {
		return key;
	}

	@Override
	public Text getCurrentValue() throws IOException {
		return value;
	}

	@Override
	public String getLastOutput() {
		try {
			return new String(data, 0, readCounter, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			return "<undecodable>";
		}
	}

	// split a UTF-8 line into key and value
	private void splitKeyVal(byte[] line, int length, Text key, Text val)
			throws IOException {
		// Need to find numKeyFields separators
		int pos = UTF8ByteArrayUtils.findBytes(line, 0, length, separator);
		for (int k = 1; k < numKeyFields && pos != -1; k++) {
			pos = UTF8ByteArrayUtils.findBytes(line, pos + separator.length,
					length, separator);
		}
		try {
			if (pos == -1) {
				key.set(line, 0, length);
				val.set("");
			} else {
				StreamKeyValUtil.splitKeyVal(line, 0, length, key, val, pos,
						separator.length);
			}
		} catch (CharacterCodingException e) {
			throw new IOException(StringUtils.stringifyException(e));
		}
	}

}
