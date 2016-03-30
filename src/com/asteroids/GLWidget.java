package com.asteroids;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

import com.asteroids.AsteroidsLib;

public class GLWidget extends GLSurfaceView 
{	
	public GLWidget(Context context) 
	{
		super(context);
		setEGLContextClientVersion(2);
		super.setEGLConfigChooser(8 , 8, 8, 8, 16, 0);
		setRenderer(new Renderer());
	}
	
    public boolean onTouchEvent(final MotionEvent event) 
    {	
        switch(event.getAction())
        {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:	
            	AsteroidsLib.touchevent(true, event.getX(), event.getY());
            break;         
            
            case MotionEvent.ACTION_UP:
            	AsteroidsLib.touchevent(false, event.getX(), event.getY());
            	break;
        }
   
        return true;
    }
      
	private static class Renderer implements GLSurfaceView.Renderer
	{
		public void onSurfaceCreated(GL10 unused, EGLConfig config) 
		{
			
	    }

	    public void onDrawFrame(GL10 unused) 
	    {
	    	AsteroidsLib.tick();
	    }

	    public void onSurfaceChanged(GL10 unused, int width, int height) 
	    {
	    	AsteroidsLib.init(width, height);  
	    }
	}
}
