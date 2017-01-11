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

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import org.cocos2dx.enginedata.EngineDataManager;
import org.cocos2dx.lib.Cocos2dxActivity;
import android.opengl.GLSurfaceView;

public class Cocos2dxRenderer implements GLSurfaceView.Renderer {
	// ===========================================================
	// Constants
	// ===========================================================

	private final static long NANOSECONDSPERSECOND = 1000000000L;
	private final static long NANOSECONDSPERMICROSECOND = 1000000;

    // The final animation interval which is used in 'onDrawFrame'
    private static long sAnimationInterval = (long) (1.0 / 60 * Cocos2dxRenderer.NANOSECONDSPERSECOND);

    // The animation interval set by engine.
    // It could be updated by 'Director::getInstance()->setAnimationInterval(value);'
    // or 'Director::getInstance()->resume();', 'Director::getInstance()->startAnimation();'.
    private static long sAnimationIntervalByEngineOrGame = (long) (1.0 / 60 * Cocos2dxRenderer.NANOSECONDSPERSECOND);

    // The animation interval set by system.
    // System could set this variable through EngineDataManager to override the default FPS set by developer.
    // By using this variable, system will be able to control temperature better
    // and waste less power while device is in low battery mode, so game could be played longer when battery is nearly dead.
    // setAnimationInterval will reset sAnimationIntervalBySystem to -1 since last change last takes effect.
    // Currently, only HuaWei Android devices may use this variable.
    private static long sAnimationIntervalBySystem = -1;

    // The animation interval when scene is changing.
    // sAnimationIntervalByEngineOrGame & sAnimationIntervalBySystem will not take effect
    // while sAnimationIntervalBySceneChange is greater than 0,
    // but sAnimationIntervalByEngineOrGame will be effective while
    // Its priority is highest while it's valid ( > 0) , and it will be invalid (set to -1) after changing scene finishes.
    // Currently, only HuaWei Android devices may use this variable.
    private static long sAnimationIntervalBySceneChange = -1;

    // The animation interval when director is paused.
    // It could be updated by 'Director::getInstance()->pause();'
    // Its priority is higher than sAnimationIntervalBySystem.
    private static long sAnimationIntervalByDirectorPaused = -1;

    private static final int ANIMATION_INTERVAL_BY_GAME = 0;
    private static final int ANIMATION_INTERVAL_BY_ENGINE = 1;
    private static final int ANIMATION_INTERVAL_BY_SYSTEM = 2;
    private static final int ANIMATION_INTERVAL_BY_SCENE_CHANGE = 3;
    private static final int ANIMATION_INTERVAL_BY_DIRECTOR_PAUSED = 4;

	// ===========================================================
	// Fields
	// ===========================================================

	private long mLastTickInNanoSeconds;
	private int mScreenWidth;
	private int mScreenHeight;

	// ===========================================================
	// Constructors
	// ===========================================================

	// ===========================================================
	// Getter & Setter
	// ===========================================================

    private static void updateFinalAnimationInterval() {
        if (sAnimationIntervalBySceneChange > 0) {
            sAnimationInterval = sAnimationIntervalBySceneChange;
        } else if (sAnimationIntervalByDirectorPaused > 0) {
            sAnimationInterval = sAnimationIntervalByDirectorPaused;
        } else if (sAnimationIntervalBySystem > 0) {
            sAnimationInterval = sAnimationIntervalBySystem;
        } else {
            sAnimationInterval = sAnimationIntervalByEngineOrGame;
        }
    }

    // For backward compatibility
    public static void setAnimationInterval(double animationInterval) {
        setAnimationInterval((float)animationInterval, ANIMATION_INTERVAL_BY_GAME);
    }

    private static void setAnimationInterval(float interval, int type) {
        final long nanoValue = (long) (interval * Cocos2dxRenderer.NANOSECONDSPERSECOND);
        if (type == ANIMATION_INTERVAL_BY_GAME) {
            long oldValue = sAnimationIntervalBySystem != -1 ? sAnimationIntervalBySystem : sAnimationIntervalByEngineOrGame;
            sAnimationIntervalByDirectorPaused = -1;
            // Reset sAnimationIntervalBySystem to -1 to make developer's FPS configuration take effect.
            sAnimationIntervalBySystem = -1;

            // Notify system that FPS configuration has been changed by game.
            if (Cocos2dxEngineDataManager.isInited() && oldValue != nanoValue)
            {
                Cocos2dxEngineDataManager.notifyFpsChanged(
                        (float)Math.ceil(1.0f/ oldValue * Cocos2dxRenderer.NANOSECONDSPERSECOND),
                        (float)Math.ceil(1.0f/interval)
                );
            }

            sAnimationIntervalByEngineOrGame = nanoValue;
        } else if (type == ANIMATION_INTERVAL_BY_ENGINE) {
            sAnimationIntervalByDirectorPaused = -1;
            sAnimationIntervalByEngineOrGame = nanoValue;
        } else if (type == ANIMATION_INTERVAL_BY_SYSTEM) {
            if (interval > 0.0f) {
                sAnimationIntervalBySystem = nanoValue;
            } else {
                sAnimationIntervalBySystem = -1;
            }
        } else if (type == ANIMATION_INTERVAL_BY_SCENE_CHANGE) {
            if (interval > 0.0f) {
                sAnimationIntervalBySceneChange = nanoValue;
            } else {
                sAnimationIntervalBySceneChange = -1;
            }
        } else if (type == ANIMATION_INTERVAL_BY_DIRECTOR_PAUSED) {
            sAnimationIntervalByDirectorPaused = nanoValue;
        }
        updateFinalAnimationInterval();
    }

	public void setScreenWidthAndHeight(final int pSurfaceWidth, final int pSurfaceHeight) {
		this.mScreenWidth = pSurfaceWidth;
		this.mScreenHeight = pSurfaceHeight;
	}

	// ===========================================================
	// Methods for/from SuperClass/Interfaces
	// ===========================================================

	@Override
	public void onSurfaceCreated(final GL10 pGL10, final EGLConfig pEGLConfig) {
		Cocos2dxRenderer.nativeInit(this.mScreenWidth, this.mScreenHeight);
		this.mLastTickInNanoSeconds = System.nanoTime();
	}

	@Override
	public void onSurfaceChanged(final GL10 pGL10, final int pWidth, final int pHeight) {
	}

	@Override
	public void onDrawFrame(final GL10 gl) {
		/*
         * No need to use algorithm in default(60 FPS) situation,
         * since onDrawFrame() was called by system 60 times per second by default.
         */
        if (Cocos2dxRenderer.sAnimationInterval <= 1.0 / 60 * Cocos2dxRenderer.NANOSECONDSPERSECOND && Cocos2dxActivity.sRegistered) {
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

	private static native void nativeTouchesBegin(final int pID, final float pX, final float pY);
	private static native void nativeTouchesEnd(final int pID, final float pX, final float pY);
	private static native void nativeTouchesMove(final int[] pIDs, final float[] pXs, final float[] pYs);
	private static native void nativeTouchesCancel(final int[] pIDs, final float[] pXs, final float[] pYs);
	private static native boolean nativeKeyDown(final int pKeyCode);
	private static native void nativeRender();
	private static native void nativeInit(final int pWidth, final int pHeight);
	private static native void nativeOnPause();
	private static native void nativeOnResume();

	public void handleActionDown(final int pID, final float pX, final float pY) {
		Cocos2dxRenderer.nativeTouchesBegin(pID, pX, pY);
	}

	public void handleActionUp(final int pID, final float pX, final float pY) {
		Cocos2dxRenderer.nativeTouchesEnd(pID, pX, pY);
	}

	public void handleActionCancel(final int[] pIDs, final float[] pXs, final float[] pYs) {
		Cocos2dxRenderer.nativeTouchesCancel(pIDs, pXs, pYs);
	}

	public void handleActionMove(final int[] pIDs, final float[] pXs, final float[] pYs) {
		Cocos2dxRenderer.nativeTouchesMove(pIDs, pXs, pYs);
	}

	public void handleKeyDown(final int pKeyCode) {
		Cocos2dxRenderer.nativeKeyDown(pKeyCode);
	}

	public void handleOnPause() {
		Cocos2dxRenderer.nativeOnPause();
	}

	public void handleOnResume() {
		Cocos2dxRenderer.nativeOnResume();
	}

	private static native void nativeInsertText(final String pText);
	private static native void nativeDeleteBackward();
	private static native String nativeGetContentText();

	public void handleInsertText(final String pText) {
		Cocos2dxRenderer.nativeInsertText(pText);
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
