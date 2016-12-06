/****************************************************************************
Copyright (c) 2010-2011 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 ****************************************************************************/
package org.cocos2dx.lib;

import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
public class Cocos2dxRenderer implements GLSurfaceView.Renderer {
    // ===========================================================
    // Constants
    // ===========================================================

    private final static String TAG = "Cocos2dxRenderer";

    private final static long NANOSECONDSPERSECOND = 1000000000L;
    private final static long NANOSECONDSPERMICROSECOND = 1000000;

    // The final animation interval which is used in 'onDrawFrame'
    private static long sAnimationInterval = (long) (1.0 / 60 * Cocos2dxRenderer.NANOSECONDSPERSECOND);

    // The animation interval set by engine.
    // It could be updated by 'Director::getInstance()->setAnimationInterval(value);'
    // or 'Director::getInstance()->resume();', 'Director::getInstance()->startAnimation();'.
    private static long sAnimationIntervalSetByEngine = (long) (1.0 / 60 * Cocos2dxRenderer.NANOSECONDSPERSECOND);

    // The animation interval set by system.
    // System could set this variable through EngineDataManager to override the default FPS set by developer.
    // By using this variable, system will be able to control temperature better
    // and waste less power while device is in low battery mode, so game could be played longer when battery is nearly dead.
    // setAnimationInterval will reset sAnimationIntervalSetBySystem to -1 since last change last takes effect.
    // Currently, only HuaWei Android devices may use this variable.
    private static long sAnimationIntervalSetBySystem = -1;

    // The animation interval when scene is changing.
    // sAnimationIntervalSetByEngine & sAnimationIntervalSetBySystem will not take effect
    // while sAnimationIntervalWhenSceneChange is greater than 0,
    // but sAnimationIntervalSetByEngine will be effective while
    // Its priority is highest while it's valid ( > 0) , and it will be invalid (set to -1) after changing scene finishes.
    // Currently, only HuaWei Android devices may use this variable.
    private static long sAnimationIntervalWhenSceneChange = -1;

    // The animation interval when director is paused.
    // It could be updated by 'Director::getInstance()->pause();'
    // Its priority is higher than sAnimationIntervalSetBySystem.
    private static long sAnimationIntervalWhenDirectorPaused = -1;

    private static final int ANIMATION_INTERVAL_SET_BY_ENGINE = 0;
    private static final int ANIMATION_INTERVAL_SET_BY_ENGINE_DONT_RESET_SYSTEM = 1;
    private static final int ANIMATION_INTERVAL_SET_BY_SYSTEM = 2;
    private static final int ANIMATION_INTERVAL_WHEN_SCENE_CHANGE = 3;
    private static final int ANIMATION_INTERVAL_WHEN_DIRECTOR_PAUSED = 4;

    // ===========================================================
    // Fields
    // ===========================================================

    private long mLastTickInNanoSeconds;
    private int mScreenWidth;
    private int mScreenHeight;
    private boolean mNativeInitCompleted = false;

    // ===========================================================
    // Constructors
    // ===========================================================

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    private static void updateFinalAnimationInterval() {
        if (sAnimationIntervalWhenSceneChange > 0) {
            Log.d(TAG, "set by scene change...");
            sAnimationInterval = sAnimationIntervalWhenSceneChange;
        } else if (sAnimationIntervalWhenDirectorPaused > 0) {
            Log.d(TAG, "set by director paused");
            sAnimationInterval = sAnimationIntervalWhenDirectorPaused;
        } else if (sAnimationIntervalSetBySystem > 0) {
            Log.d(TAG, "set by system");
            sAnimationInterval = sAnimationIntervalSetBySystem;
        } else {
            Log.d(TAG, "set by engine");
            sAnimationInterval = sAnimationIntervalSetByEngine;
        }
        Log.d(TAG, "updateFinalAnimationInterval: " + Cocos2dxRenderer.NANOSECONDSPERSECOND / sAnimationInterval);
    }

    // For backward compatibility
    public static void setAnimationInterval(float interval) {
        setAnimationInterval(interval, ANIMATION_INTERVAL_SET_BY_ENGINE);
    }

    private static void setAnimationInterval(float interval, int type) {
        Log.d(TAG, "setAnimationInterval: " + Math.ceil(1.0f/interval) + ", type: " + type);
        final long nanoValue = (long) (interval * Cocos2dxRenderer.NANOSECONDSPERSECOND);
        if (type == ANIMATION_INTERVAL_SET_BY_ENGINE) {
            sAnimationIntervalWhenDirectorPaused = -1;
            // Reset sAnimationIntervalSetBySystem to -1 to make developer's FPS configuration take effect.
            sAnimationIntervalSetBySystem = -1;
            sAnimationIntervalSetByEngine = nanoValue;
        } else if (type == ANIMATION_INTERVAL_SET_BY_ENGINE_DONT_RESET_SYSTEM) {
            sAnimationIntervalWhenDirectorPaused = -1;
            sAnimationIntervalSetByEngine = nanoValue;
        } else if (type == ANIMATION_INTERVAL_SET_BY_SYSTEM) {
            if (interval > 0.0f) {
                sAnimationIntervalSetBySystem = nanoValue;
            } else {
                sAnimationIntervalSetBySystem = -1;
            }
        } else if (type == ANIMATION_INTERVAL_WHEN_SCENE_CHANGE) {
            if (interval > 0.0f) {
                sAnimationIntervalWhenSceneChange = nanoValue;
            } else {
                sAnimationIntervalWhenSceneChange = -1;
            }
        } else if (type == ANIMATION_INTERVAL_WHEN_DIRECTOR_PAUSED) {
            sAnimationIntervalWhenDirectorPaused = nanoValue;
        }
        updateFinalAnimationInterval();
    }

    public void setScreenWidthAndHeight(final int surfaceWidth, final int surfaceHeight) {
        this.mScreenWidth = surfaceWidth;
        this.mScreenHeight = surfaceHeight;
    }

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    @Override
    public void onSurfaceCreated(final GL10 GL10, final EGLConfig EGLConfig) {
        Cocos2dxRenderer.nativeInit(this.mScreenWidth, this.mScreenHeight);
        this.mLastTickInNanoSeconds = System.nanoTime();
        mNativeInitCompleted = true;
    }

    @Override
    public void onSurfaceChanged(final GL10 GL10, final int width, final int height) {
        Cocos2dxRenderer.nativeOnSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(final GL10 gl) {
        /*
         * No need to use algorithm in default(60 FPS) situation,
         * since onDrawFrame() was called by system 60 times per second by default.
         */
        if (Cocos2dxRenderer.sAnimationInterval <= 1.0 / 60 * Cocos2dxRenderer.NANOSECONDSPERSECOND) {
            Cocos2dxRenderer.nativeRender();
        } else {
            final long now = System.nanoTime();
            final long interval = now - this.mLastTickInNanoSeconds;
        
            if (interval < Cocos2dxRenderer.sAnimationInterval) {
                try {
                    Thread.sleep((Cocos2dxRenderer.sAnimationInterval - interval) / Cocos2dxRenderer.NANOSECONDSPERMICROSECOND);
                } catch (final Exception e) {
                }
            }
            /*
             * Render time MUST be counted in, or the FPS will slower than appointed.
            */
            this.mLastTickInNanoSeconds = System.nanoTime();
            Cocos2dxRenderer.nativeRender();
        }
    }

    // ===========================================================
    // Methods
    // ===========================================================

    private static native void nativeTouchesBegin(final int id, final float x, final float y);
    private static native void nativeTouchesEnd(final int id, final float x, final float y);
    private static native void nativeTouchesMove(final int[] ids, final float[] xs, final float[] ys);
    private static native void nativeTouchesCancel(final int[] ids, final float[] xs, final float[] ys);
    private static native boolean nativeKeyEvent(final int keyCode,boolean isPressed);
    private static native void nativeRender();
    private static native void nativeInit(final int width, final int height);
    private static native void nativeOnSurfaceChanged(final int width, final int height);
    private static native void nativeOnPause();
    private static native void nativeOnResume();

    public void handleActionDown(final int id, final float x, final float y) {
        Cocos2dxRenderer.nativeTouchesBegin(id, x, y);
    }

    public void handleActionUp(final int id, final float x, final float y) {
        Cocos2dxRenderer.nativeTouchesEnd(id, x, y);
    }

    public void handleActionCancel(final int[] ids, final float[] xs, final float[] ys) {
        Cocos2dxRenderer.nativeTouchesCancel(ids, xs, ys);
    }

    public void handleActionMove(final int[] ids, final float[] xs, final float[] ys) {
        Cocos2dxRenderer.nativeTouchesMove(ids, xs, ys);
    }

    public void handleKeyDown(final int keyCode) {
        Cocos2dxRenderer.nativeKeyEvent(keyCode, true);
    }

    public void handleKeyUp(final int keyCode) {
        Cocos2dxRenderer.nativeKeyEvent(keyCode, false);
    }

    public void handleOnPause() {
        /**
         * onPause may be invoked before onSurfaceCreated, 
         * and engine will be initialized correctly after
         * onSurfaceCreated is invoked. Can not invoke any
         * native method before onSurfaceCreated is invoked
         */
        if (! mNativeInitCompleted)
            return;

        Cocos2dxHelper.onEnterBackground();
        Cocos2dxRenderer.nativeOnPause();
    }

    public void handleOnResume() {
        Cocos2dxHelper.onEnterForeground();
        Cocos2dxRenderer.nativeOnResume();
    }

    private static native void nativeInsertText(final String text);
    private static native void nativeDeleteBackward();
    private static native String nativeGetContentText();

    public void handleInsertText(final String text) {
        Cocos2dxRenderer.nativeInsertText(text);
    }

    public void handleDeleteBackward() {
        Cocos2dxRenderer.nativeDeleteBackward();
    }

    public String getContentText() {
        return Cocos2dxRenderer.nativeGetContentText();
    }

    // ===========================================================
    // Inner and Anonymous Classes
    // ===========================================================
}
