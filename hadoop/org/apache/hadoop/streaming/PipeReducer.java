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

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.Iterator;
import java.util.UUID;
import java.net.URLDecoder;

import jtux.UErrorException;

import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.SkipBadRecords;
import org.apache.hadoop.mapreduce.MRJobConfig;
import org.apache.hadoop.streaming.io.InputWriter;
import org.apache.hadoop.streaming.io.OutputReader;
import org.apache.hadoop.streaming.io.ShmInputWriter;
import org.apache.hadoop.streaming.io.ShmOutputReader;
import org.apache.hadoop.util.StringUtils;

import org.apache.hadoop.io.Writable;

/**
 * A generic Reducer bridge. It delegates operations to an external program via
 * stdin and stdout.
 */
public class PipeReducer extends PipeMapRed implements Reducer {

	private byte[] reduceOutFieldSeparator;
	private byte[] reduceInputFieldSeparator;
	private int numOfReduceOutputKeyFields = 1;
	private boolean skipping = false;
	
	protected int reduceInputTime2UnlockEmptySem;
	protected int reduceInputTime2UnlockFullSem;
	protected int reduceOutputTime2UnlockEmptySem;
	protected int reduceOutputTime2UnlockFullSem;

	String getPipeCommand(JobConf job) {
		String str = job.get("stream.reduce.streamprocessor");
		if (str == null) {
			return str;
		}
		try {
			return URLDecoder.decode(str, "UTF-8");
		} catch (UnsupportedEncodingException e) {
			System.err
					.println("stream.reduce.streamprocessor in jobconf not found");
			return null;
		}
	}

	boolean getDoPipe() {
		String argv = getPipeCommand(job_);
		// Currently: null is identity reduce. REDUCE_NONE is no-map-outputs.
		return (argv != null) && !StreamJob.REDUCE_NONE.equals(argv);
	}

	public void configure(JobConf job) {
		super.configure(job);
		// disable the auto increment of the counter. For streaming, no of
		// processed records could be different(equal or less) than the no of
		// records input.
		SkipBadRecords.setAutoIncrReducerProcCount(job, false);
		skipping = job.getBoolean(MRJobConfig.SKIP_RECORDS, false);

		try {
			//this.reduceInputTime2UnlockEmptySem = 
				  job_.getInt("stream.shm.reduce.input.unlock.empty.semaphore", 100);
			  this.reduceInputTime2UnlockFullSem = 
				  job_.getInt("stream.shm.reduce.input.unlock.full.semaphore", 100);
			  this.reduceOutputTime2UnlockEmptySem = 
				  job_.getInt("stream.shm.reduce.output.unlock.empty.semaphore", 100);
			  this.reduceOutputTime2UnlockFullSem = 
				  job_.getInt("stream.shm.reduce.output.unlock.full.semaphore", 100);
			  
			 // Debug.writeDebug("reduceInputTime2UnlockEmptySem = " + this.reduceInputTime2UnlockEmptySem
			//		  	+ "; reduceInputTime2UnlockFullSem = " + this.reduceInputTime2UnlockFullSem + "\n");
			  		  
			reduceOutFieldSeparator = job_.get(
					"stream.reduce.output.field.separator", "\t").getBytes(
					"UTF-8");
			reduceInputFieldSeparator = job_.get(
					"stream.reduce.input.field.separator", "\t").getBytes(
					"UTF-8");
			this.numOfReduceOutputKeyFields = job_.getInt(
					"stream.num.reduce.output.key.fields", 1);
			if (reduceInputWriterClass_.getCanonicalName().equals(
					ShmInputWriter.class.getCanonicalName())) {
				UUID uid = UUID.randomUUID();
				shm_out = new SharedMemory(MAXSHM, keyDir + "/" + uid, 
						reduceInputTime2UnlockEmptySem, reduceInputTime2UnlockFullSem);
				//shm_out = new SharedMemory(MAXSHM, keyDir + "/" + uid);
				this.writeKeyFileOut(shm_out.getKeyFile());
			}

			if (reduceOutputReaderClass_.getCanonicalName().equals(
					ShmOutputReader.class.getCanonicalName())) {
				UUID uid = UUID.randomUUID();
				shm_in = new SharedMemory(MAXSHM, keyDir + "/" + uid, 
						reduceOutputTime2UnlockEmptySem, reduceOutputTime2UnlockFullSem);
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

	public void reduce(Object key, Iterator values, OutputCollector output,
			Reporter reporter) throws IOException {

		// init
		if (doPipe_ && outThread_ == null) {
			startOutputThreads(output, reporter);
		}
		try {
			while (values.hasNext()) {
				Writable val = (Writable) values.next();
				numRecRead_++;
				maybeLogRecord();
				if (doPipe_) {
					if (outerrThreadsThrowable != null) {
						mapRedFinished();
						throw new IOException(
								"MROutput/MRErrThread failed:"
										+ StringUtils
												.stringifyException(outerrThreadsThrowable));
					}
					inWriter_.writeKey(key);
					inWriter_.writeValue(val);
				} else {
					// "identity reduce"
					output.collect(key, val);
				}
			}
			if (doPipe_ && skipping) {
				// flush the streams on every record input if running in skip
				// mode
				// so that we don't buffer other records surrounding a bad
				// record.
				if (shm_out == null)
					clientOut_.flush();
			}
		} catch (IOException io) {
			// a common reason to get here is failure of the subprocess.
			// Document that fact, if possible.
			String extraInfo = "";
			try {
				int exitVal = sim.exitValue();
				if (exitVal == 0) {
					extraInfo = "subprocess exited successfully\n";
				} else {
					extraInfo = "subprocess exited with error code " + exitVal
							+ "\n";
				}
				;
			} catch (IllegalThreadStateException e) {
				// hmm, but child is still running. go figure.
				extraInfo = "subprocess still running\n";
			}
			;
			appendLogToJobLog("failure");
			mapRedFinished();
			throw new IOException(extraInfo + getContext() + io.getMessage());
		}
	}

	public void close() {
		appendLogToJobLog("success");
		//Debug.writeDebug("The reducer finished, call mapRedFinished()!\n");
		mapRedFinished();
	}

	@Override
	public byte[] getInputSeparator() {
		return reduceInputFieldSeparator;
	}

	@Override
	public byte[] getFieldSeparator() {
		return reduceOutFieldSeparator;
	}

	@Override
	public int getNumOfKeyFields() {
		return numOfReduceOutputKeyFields;
	}

	@Override
	InputWriter createInputWriter() throws IOException {
		return super.createInputWriter(reduceInputWriterClass_);
	}

	@Override
	OutputReader createOutputReader() throws IOException {
		return super.createOutputReader(reduceOutputReaderClass_);
	}

}
