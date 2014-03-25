package jtux;

public class UUtil {
	/*
		Not sure yet where this should be.
	*/
	static {
		System.loadLibrary("jtux");
		System.out.println("Loaded");
	}

	public static class IntHolder {
		public int value;
	}

	native static void check_type_sizes() throws UErrorException;
	/**
		Calls strerror.
		<p>
		<font size="-1"><b><i>Click {@link <a href="doc-files/synopses.html#strerror">here</a>} for Posix/SUS C API.</i></b></font>
	*/
	native static String strerror(int errnum);
	public native static long GetSymbol(String category, String symbol);
	public native static String GetSymbolStr(String category, int code);
	/**
		Convenience functions (non-standard) to move byte array to and from shared
		memory.
	*/
	public native static void jaddr_to_seg(long addr, byte[] data, int datasize);
	public native static void jaddr_from_seg(long addr, byte[] data, int datasize);
	
        public native static void jaddr_to_fifo(long fifo, long addr, byte[] data, int datasize, long sem_empty, long sem_full);
        public native static void jaddr_from_fifo(long fifo, long addr, byte[] data, int datasize, long sem_empty, long sem_full);

        public native static long fifo_init(long daddr, int size, int semkey, int to_unlock_empty_sem, int to_unlock_full_sem);
	public native static void fifo_free(long fifo);

	public native static int fifo_in(long fifo);
	public native static int fifo_out(long fifo);
	public native static int fifo_stat(long fifo);
	public native static int fifo_stat_empty(long fifo);
	public native static int fifo_stat_full(long fifo);
	static {
		try {
			check_type_sizes();
		}
		catch (UErrorException e) {
			System.err.println("Jtux error: Java native types don't match POSIX/SUS types.");
			System.exit(1);
		}
	}

	static void StringBufferSet(StringBuffer sb, String s) {
		sb.delete(0, sb.length());
		sb.insert(0, s);
	}

	/**
		Following here as a convenience so the JNI code doesn't have to chase
		down the method in StringBuffer itself.
	*/
	static String StringBufferGet(StringBuffer sb) {
		return sb.toString();
	}
}
