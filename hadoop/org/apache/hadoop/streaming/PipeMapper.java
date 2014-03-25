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

package org.apache.hadoop.streaming;

import java.io.*;
import java.net.URLDecoder;
import java.util.UUID;

import jtux.UErrorException;
import jtux.UUtil;

import org.apache.hadoop.io.BytesWritable;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.SkipBadRecords;
import org.apache.hadoop.mapred.TextInputFormat;
import org.apache.hadoop.mapreduce.MRJobConfig;
import org.apache.hadoop.streaming.io.InputWriter;
import org.apache.hadoop.streaming.io.OutputReader;
import org.apache.hadoop.streaming.io.ShmInputWriter;
import org.apache.hadoop.streaming.io.ShmOutputReader;
import org.apache.hadoop.streaming.io.TextInputWriter;
import org.apache.hadoop.util.StringUtils;

/**
 * A generic Mapper bridge. It delegates operations to an external program via
 * stdin and stdout.
 */
public class PipeMapper extends PipeMapRed implements Mapper {

	private boolean ignoreKey = false;
	private boolean skipping = false;

	private byte[] mapOutputFieldSeparator;
	private byte[] mapInputFieldSeparator;

	private int numOfMapOutputKeyFields = 1;
	
	private long just = 0;
	private long now = 0;
	private double time = 0.0;
	private int kvRecord = 0;
	protected int mapInputTime2UnlockEmptySem;
	protected int mapInputTime2UnlockFullSem;
	protected int mapOutputTime2UnlockEmptySem;
	protected int mapOutputTime2UnlockFullSem;

	String getPipeCommand(JobConf job) {
		String str = job.get("stream.map.streamprocessor");
		if (str == null) {
			return str;
		}
		try {
			return URLDecoder.decode(str, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			System.err
					.println("stream.map.streamprocessor in jobconf not found");
			return null;
		}
	}

	boolean getDoPipe() {
		return true;
	}

	public void configure(JobConf job) {
		super.configure(job);
		//Debug.writeDebug("Start configuring mapper!\n");
		// disable the auto increment of the counter. For streaming, no of
		// processed records could be different(equal or less) than the no of
		// records input.
		  this.mapInputTime2UnlockEmptySem = 
			  job_.getInt("stream.shm.map.input.unlock.empty.semaphore", 300);
		  this.mapInputTime2UnlockFullSem = 
			  job_.getInt("stream.shm.map.input.unlock.full.semaphore", 300);
		  this.mapOutputTime2UnlockEmptySem = 
			  job_.getInt("stream.shm.map.output.unlock.empty.semaphore", 600);
		  this.mapOutputTime2UnlockFullSem = 
			  job_.getInt("stream.shm.map.output.unlock.full.semaphore", 600);
		  
		//  Debug.writeDebug("mapInputTime2UnlockEmptySem = " + this.mapInputTime2UnlockEmptySem
		//		  + ";\n" + "mapInputTime2UnlockFullSem = " + this.mapInputTime2UnlockFullSem 
		//		  + ";\n" + "mapOutputTime2UnlockEmptySem = " + this.mapOutputTime2UnlockEmptySem
		//		  + ";\n" + "mapOutputTime2UnlockFullSem = " + this.mapOutputTime2UnlockFullSem + ";\n");
		  
		SkipBadRecords.setAutoIncrMapperProcCount(job, false);
		skipping = job.getBoolean(MRJobConfig.SKIP_RECORDS, false);
		//Debug.writeDebug("The java native library: " + System.getProperty("java.library.path") + "\n");
		// TODO
		if (mapInputWriterClass_.getCanonicalName().equals(
				TextInputWriter.class.getCanonicalName())
				|| mapInputWriterClass_.getCanonicalName().equals(
						ShmInputWriter.class.getCanonicalName())) {
			String inputFormatClassName = job.getClass(
					"mapred.input.format.class", TextInputFormat.class)
					.getCanonicalName();
			ignoreKey = inputFormatClassName.equals(TextInputFormat.class
					.getCanonicalName());
		}

		try {
			mapOutputFieldSeparator = job.get(
					"stream.map.output.field.separator", "\t")
					.getBytes("UTF-8");
			mapInputFieldSeparator = job.get(
					"stream.map.input.field.separator", "\t").getBytes("UTF-8");
			numOfMapOutputKeyFields = job.getInt(
					"stream.num.map.output.key.fields", 1);
			if (mapInputWriterClass_.getCanonicalName().equals(
					ShmInputWriter.class.getCanonicalName())) {
				UUID uid = UUID.randomUUID();
				shm_out = new SharedMemory(MAXSHM, keyDir + "/" + uid, 
						this.mapInputTime2UnlockEmptySem, this.mapInputTime2UnlockFullSem);
				//Debug.writeDebug("Create new shm_out!\n");
				//shm_out = new SharedMemory(MAXSHM, keyDir + "/" + uid);
				//Debug.writeDebug(shm_out.getKeyFile());
				this.writeKeyFileOut(shm_out.getKeyFile());
			}

			if (mapOutputReaderClass_.getCanonicalName().equals(
					ShmOutputReader.class.getCanonicalName())) {
				UUID uid = UUID.randomUUID();
				shm_in = new SharedMemory(MAXSHM, keyDir + "/" + uid,
						this.mapOutputTime2UnlockEmptySem, this.mapOutputTime2UnlockFullSem);
				//Debug.writeDebug(shm_in.getKeyFile());
				//Debug.writeDebug("Create new shm_in!\n");
				//shm_in = new SharedMemory(MAXSHM, keyDir + "/" + uid);
				this.writeKeyFileOut(shm_in.getKeyFile());
			}
		} catch (UnsupportedEncodingException e) {
			throw new RuntimeException(
					"The current system does not support UTF-8 encoding!", e);
		} catch (UErrorException e) {
			// TODO Auto-generated catch block
			throw new RuntimeException("Initiate the shared memory error!", e);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			throw new RuntimeException("IOError while opening the keyFile", e);
		}
	}

	// Do NOT declare default constructor
	// (MapRed creates it reflectively)

	public void map(Object key, Object value, OutputCollector output,
			Reporter reporter) throws IOException {
		if (outerrThreadsThrowable != null) {
			mapRedFinished();
			throw new IOException("MROutput/MRErrThread failed:"
					+ StringUtils.stringifyException(outerrThreadsThrowable));
		}
		try {
			// 1/4 Hadoop in
			numRecRead_++;
			maybeLogRecord();
			if (debugFailDuring_ && numRecRead_ == 3) {
				throw new IOException("debugFailDuring_");
			}
			// 2/4 Hadoop to Tool
			// TODO Adding debug information

			if (numExceptions_ == 0) {
				//just = System.currentTimeMillis();
				if (!this.ignoreKey) {
					inWriter_.writeKey(key);
				}
				inWriter_.writeValue(value);
				//if (skipping) {
					// flush the streams on every record input if running in
					// skip mode
					// so that we don't buffer other records surrounding a bad
					// record.
				if (shm_out == null)
					clientOut_.flush();
				//}
				//kvRecord++;
				//now = System.currentTimeMillis();
				//time += (double)(now - just);
				//if(kvRecord % 1000 == 0){
				//	Debug.writeDebug("Pop 1000 kvPair outside consumes: " + time + " ms!\n");
				//	time = 0.0;
				//}

			} else {
				numRecSkipped_++;
			}
		} catch (IOException io) {
			numExceptions_++;
			if (numExceptions_ > 1
					|| numRecWritten_ < minRecWrittenToEnableSkip_) {
				// terminate with failure
				String msg = logFailure(io);
				appendLogToJobLog("failure");
				mapRedFinished();
				throw new IOException(msg);
			} else {
				// terminate with success:
				// swallow input records although the stream processor
				// failed/closed
			}
		}
	}

	public void close() {
		appendLogToJobLog("success");
		mapRedFinished();
	}

	@Override
	public byte[] getInputSeparator() {
		return mapInputFieldSeparator;
	}

	@Override
	public byte[] getFieldSeparator() {
		return mapOutputFieldSeparator;
	}

	@Override
	public int getNumOfKeyFields() {
		return numOfMapOutputKeyFields;
	}

	@Override
	InputWriter createInputWriter() throws IOException {
		return super.createInputWriter(mapInputWriterClass_);
	}

	@Override
	OutputReader createOutputReader() throws IOException {
		return super.createOutputReader(mapOutputReaderClass_);
	}

}
