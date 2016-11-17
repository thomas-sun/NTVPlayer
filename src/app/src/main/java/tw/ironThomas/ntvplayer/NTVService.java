
package tw.ironThomas.ntvplayer;


public class NTVService
{
    public static native int  start(String server, int server_port, int local_port, String key);
    public static native void  stop();
    public static native void  command(String cmd);
    public static native void  quality(int qt);
    public static native void  record(int state, int w, int h);
    public static native String  info();

    static {
        System.loadLibrary("ntvplayer");
    }
}

