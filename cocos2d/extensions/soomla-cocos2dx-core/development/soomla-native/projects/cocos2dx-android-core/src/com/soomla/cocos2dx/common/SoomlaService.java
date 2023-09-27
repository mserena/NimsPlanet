package com.soomla.cocos2dx.common;

import android.opengl.GLSurfaceView;


public interface SoomlaService {
    void setGlSurfaceView(GLSurfaceView glSurfaceView);
    void onPause();
    void onResume();
}
