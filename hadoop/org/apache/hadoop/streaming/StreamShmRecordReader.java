package org.apache.hadoop.streaming;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.FileSplit;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.util.LineReader;

class StreamShmRecordReader extends StreamBaseRecordReader{

	private static byte[] LINESEPARATOR;
	// Should spare space for rcnt, key and key-value separator
	private static int SPACE = 100;

	private byte[] array; // Array to temporarily store data from HDFS
	private String line = null;
	private boolean restore = false;
	private int MAXSHM; // Should make it configurable
	private BufferedReader inReader =  null;
	long pos_;
	
	private int readCounts;

	public StreamShmRecordReader(FSDataInputStream in, FileSplit split,
			Reporter reporter, JobConf job, FileSystem fs) throws IOException {
		super(in, split, reporter, job, fs);
		//Debug.writeTime();
		//Debug.writeDebug("In StreamShmRecordReader::constructor!\n");
		MAXSHM = Integer.valueOf(checkJobGet(CONF_NS + "buffersize"));
		// The size of shared memory should limit to time of 4096, 
		// or in the linux system, a page size, which is regulated in
		// linux user API: shmget, ****man shmget for detail***
		if(MAXSHM % 4096 != 0){ 
			MAXSHM += (4096 - MAXSHM % 4096);
		}
		init();

		// TODO Auto-generated constructor stub
	}
	
	private void init() throws IOException{
	   // Debug.writeDebug("StreamShmRecordReader.init: " + " start_=" + start_ + " end_=" + end_ + " length_="
	   //          + length_ + " start_ > in_.getPos() =" + (start_ > in_.getPos()) + " " + start_ + " > "
	   //          + in_.getPos() + "\n");
		readCounts = 0;
	    if (start_ > in_.getPos()) {
	      in_.seek(start_);
	    }
	    pos_ = start_;
	    inReader = new BufferedReader(new InputStreamReader(
				new BufferedInputStream(in_)));
	    array = new byte[MAXSHM];
	    if(start_ != 0)
	    	seekNextRecordBoundary();
	   // Debug.writeDebug("After adjust, currently start = " + start_ 
	    //				+ " position of in is "  + in_.getPos() + ".\n");
	}

	@Override
	public boolean next(Text key, Text value) throws IOException {
		// TODO Auto-generated method stub
		//Debug.writeTime();
		//Debug.writeDebug("In StreamShmRecordReader::next!\n");
		if(pos_ >= end_){
			return false;
		} 
		readLine();
		numRecStats(array, 0, readCounts);
		key.set("");
		value.set(array, 0, readCounts);
		//Debug.writeDebug("After reading " + readCounts + " bytes, the value is: \n");
		//Debug.writeMapRedObject(value);
		//Debug.writeDebug("\n@@@@@\n");
		return true;
	}

	@Override
	public void seekNextRecordBoundary() throws IOException {
		// TODO Auto-generated method stub
		int rcnt = 0;
		String tmp = inReader.readLine();
		rcnt = tmp.length();
		start_+= (rcnt + 1); // newLine should be included
		pos_ = start_;
	}
	
	private String checkJobGet(String prop) throws IOException {
		String val = job_.get(prop);
	    if (val == null) {
	      throw new IOException("JobConf: missing required property: " + prop);
	    }
	    return val;
	}
	
	private void readLine() throws IOException{
		while(true){
			line = inReader.readLine();
			readCounts = line.length();
			pos_ += (readCounts + 1);
			if(readCounts == 0)
				continue;
			else {
				System.arraycopy(line.getBytes(), 0, array, 0, readCounts);
				break;
			}
		}
	}
	
	private boolean readLineToPadArray() throws IOException{
		int rcnt = 0;
		int bufferPos = IntByteArray.COUNTBYTES;
		int bufferCurrent = bufferPos;
		//int separatorLen = LINESEPARATOR.length;
		while(true){
			if (!restore){
				line = inReader.readLine();
				rcnt = line.length();
				pos_ += (rcnt + 1);
			} else {
				rcnt = line.length();
				restore = false;
			}
			
			if (rcnt == 0) {
				continue;
			} else {
				//bufferCurrent += (rcnt + separatorLen);
				if (bufferCurrent >= MAXSHM - SPACE) {
					restore = true;
					break;
				}
				System.arraycopy(line.getBytes(), 0, array, bufferPos, rcnt);
				bufferPos += rcnt;
				//System.arraycopy(LINESEPARATOR, 0, array, bufferPos, separatorLen);
				//bufferPos += separatorLen;
				//Debug.writeDebug("Currently, bufferPos = " + bufferPos 
				//				+ " bufferCurrent = " + bufferCurrent
				///				+ " pos = " + pos_
				//				+ " inputStream pos = " + in_.getPos()
				//				+ "\n");
				
				//Debug.writeDebug("The data being read is: \n");
				//Debug.writeDebug(line + "\n @@@@@\n");
				if(pos_ >= end_){
					// Read another line after the splitter
					// So next time to read the file, we start from just the line after the splitter
					// as we show in seekNextRecordBoundary.
					break;
				}
				//array[pos - 1] = ' '; // Pad the space to desperate the two words among lines
			}
		}
		// Donot take into account the first {COUNTBYTES} bytes occupying by the read counts	
		readCounts = bufferPos - IntByteArray.COUNTBYTES;
		
		byte[] output = new byte[IntByteArray.COUNTBYTES];
		output = IntByteArray.int2Bytes(readCounts);
		System.arraycopy(output, 0, array, 0, IntByteArray.COUNTBYTES);
		
		return true;
	}	
	
}