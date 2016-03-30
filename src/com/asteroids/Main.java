package com.asteroids;

import android.app.Activity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Window;

import com.asteroids.GLWidget;


public class Main extends Activity {
    GLWidget glWidget;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) 
    {
        super.onCreate(savedInstanceState);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        glWidget = new GLWidget(getApplication());
        setContentView(glWidget);  
    }
       
    @Override
    protected void onStop() 
    {
        super.onStop(); 
        AsteroidsLib.deinit();
        finish();
    }
    
    public boolean onKeyDown(int keyCode, KeyEvent event) 
    {
        if (keyCode == KeyEvent.KEYCODE_BACK) 
        {
        	finish();
        	return true;
        }
        return super.onKeyDown(keyCode, event);
    }
}
