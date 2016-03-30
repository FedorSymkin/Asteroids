package com.asteroids;

public class AsteroidsLib 
{
	static
	{
		System.loadLibrary("asteroids");
	}
	
	public static native void init(int width, int height);
	public static native void tick();
	public static native void touchevent(boolean down, float x, float y);
	public static native void keyevent(boolean down, int key);
	public static native void deinit();
	
	public static native int avgticktime();
	public static native int maxticktime();
}
