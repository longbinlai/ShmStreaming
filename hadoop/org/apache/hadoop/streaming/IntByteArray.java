package org.apache.hadoop.streaming;

public class IntByteArray{
	public static int COUNTBYTES = Integer.SIZE / Byte.SIZE;
	private static int[] MASK = {0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000};
	
	public static byte[] int2Bytes(int input){
		byte[] array = new byte[COUNTBYTES];
		if(input < 0)
			input = -1;
		for(int i = 0; i < COUNTBYTES; i++){
			array[i] = (byte)((input >> (Byte.SIZE * i)) & MASK[0]);
		}
		return array;
	}
	
	public static int bytes2Int(byte[] inArray){
		int res = 0;
		int tmp = 0;
		if(inArray.length < COUNTBYTES){
			return -1;
		}
		
		for(int i = 0; i < COUNTBYTES; i++){
			tmp |= inArray[i];
			res |= (tmp << (Byte.SIZE * i)) & MASK[i];
			tmp = 0;
		}
		return res;
	}
}