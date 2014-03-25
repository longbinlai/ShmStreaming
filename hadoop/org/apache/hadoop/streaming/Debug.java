package org.apache.hadoop.streaming;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.apache.hadoop.io.BytesWritable;
import org.apache.hadoop.io.Text;

public class Debug {
	public static boolean INITMAP_ = false;
	public static boolean INITRED_ = false;
	static File file = new File("/home/epcc/hadoop-0.21.0/hadoop.log");
	public static synchronized void writeDebug(String Message)
	{
		try {
			//File file = new File("SUR_ECCS.log");
			FileWriter filewriter = new FileWriter(file, true);
			// filewriter.write(dateNowStr+"\n"+Message+"\n");
			filewriter.write(Message);
			filewriter.flush();
			filewriter.close();
		} catch (IOException e) {
			System.out.println("create file failed");
		}
	}
	
	public static synchronized void writeMapRedObject(Object object){
		byte[] bval;
	    int valSize;
		try {
			FileWriter filewriter = new FileWriter(file, true);
			if (object instanceof BytesWritable) {
				BytesWritable val = (BytesWritable) object;
				bval = val.getBytes();
				filewriter.write(new String(bval));
			} else if (object instanceof Text) {
				Text val = (Text) object;
				bval = val.getBytes();
				filewriter.write(new String(bval));
			} else {
				String sval = object.toString();
				filewriter.write(sval);
			}
			filewriter.flush();
			filewriter.close();
		} catch (IOException e) {
			System.out.println("create file failed");
		}
	}
	
	public static synchronized void writeTime()
	{
		// get the current time
		Date dateNow= new Date();
		SimpleDateFormat  dateFormat=new SimpleDateFormat ("yyyy.MM.dd HH:mm:ss");
	    String dateNowStr = dateFormat.format(dateNow);
	    try {
	    	//File file = new File("SUR_ECCS.log");
			FileWriter filewriter = new FileWriter(file, true);
			// filewriter.write(dateNowStr+"\n"+Message+"\n");
			filewriter.write("\n*******************************************\n");
			filewriter.write("At " + dateNowStr + ":\n");
			filewriter.flush();
			filewriter.close();
	    } catch(IOException e)
	    {
	    	System.out.println("create file failed");
	    }
	    
	}

}