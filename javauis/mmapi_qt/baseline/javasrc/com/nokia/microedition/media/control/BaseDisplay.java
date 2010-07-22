/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


package com.nokia.microedition.media.control;

import org.eclipse.swt.widgets.*;
import javax.microedition.lcdui.*;
import org.eclipse.swt.graphics.Rectangle;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.microedition.media.control.VideoItem;

public abstract class BaseDisplay
{
    // represents native side of display and initialized by derived class
    protected int nativeDisplayHandle;
    protected org.eclipse.swt.widgets.Control iControl;
    private Rectangle rect;
    private int[] rectDimension = new int[ 4 ];
    protected boolean iseSWT;

    public BaseDisplay()
    {
        rect = new Rectangle(0,0,0,0);
    }
    /*  protected void setESWTControl()
        {
            eswtCanvasControl =
        }
        */
    public void GetCallbackInUiThread(int aPlaceHolder)
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"_+inside BaseDisplay : GetCallbackInUiThread ++");
        final int val = aPlaceHolder;
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"_+inside BaseDisplay : GetCallbackInUiThread - aPlaceHolder = "+aPlaceHolder+"nativeDisplayHandle = "+nativeDisplayHandle);
        try
        {
            org.eclipse.swt.widgets.Display disp = null;
            if (iseSWT)
            {
                disp = org.eclipse.swt.widgets.Display.getDefault();
            }
            else
            {
                disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            }
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"_+inside BaseDisplay : GetCallbackInUiThread - got eswtdisplay");
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"_+inside BaseDisplay - before __nativeMethodCallInUiThread");
                    _nativeMethodCallInUiThread(nativeDisplayHandle , val);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : GetCallbackInUiThread....exception is " + e.toString());
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"_+inside BaseDisplay : GetCallbackInUiThread --");

    }

    public void setContentBound()
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : setContentBound +");
        try
        {

            org.eclipse.swt.widgets.Display disp = null;
            if (iseSWT)
            {
                disp = org.eclipse.swt.widgets.Display.getDefault();
            }
            else
            {
                disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            }
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    Rectangle arect = iControl.getBounds();
                    rect.x = rect.x + arect.x;
                    rect.y = rect.y + arect.y;
                    iControl.setBounds(rect);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : setContentBound....exception is  " + e.toString());
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : setContentBound -");
    }


    public void removeContentBound()
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : removeContentBound +");
        try
        {

            org.eclipse.swt.widgets.Display disp = null;
            if (iseSWT)
            {
                disp = org.eclipse.swt.widgets.Display.getDefault();
            }
            else
            {
                disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            }
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    Rectangle emptyRect = new Rectangle(0,0,0,0);
                    iControl.setBounds(emptyRect);
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : removeContentBound....exception is  " + e.toString());
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : removeContentBound -");
    }

    public void redrawControl()
    {
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : redrawControl +");
        try
        {

            org.eclipse.swt.widgets.Display disp = null;
            if (iseSWT)
            {
                disp = org.eclipse.swt.widgets.Display.getDefault();
            }
            else
            {
                disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            }
            disp.syncExec(new Runnable()
            {
                public void run()
                {
                    Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : redrawControl before redraw");
                    // redraw entire bounds of receiver
                    iControl.redraw();
                }
            });
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : redrawControl after redraw");
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : redrawControl....exception is  " + e.toString());
        }
        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"inside BaseDisplay : redrawControl -");
    }
    /* called from native to reset the java side rect
    Before the call of this function array is updated from native side
    which can be used to refresh the UI screen rect and/or
    set/remove bound rect
    */
    public void setRect()
    {

        rect.x = rectDimension[0];
        rect.y = rectDimension[1];
        rect.width = rectDimension[2];
        rect.height = rectDimension[3];

        Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"BaseDisplay : setRect rect is reset with values: x =" + rect.x +"y ="+ rect.y +"width ="+rect.width+"height = "+ rect.height);

    }

    public Rectangle getBounds()
    {
        try
        {

            org.eclipse.swt.widgets.Display disp = null;
            if (iseSWT)
            {
                disp = org.eclipse.swt.widgets.Display.getDefault();
            }
            else
            {
                disp = com.nokia.mj.impl.nokialcdui.LCDUIInvoker.getEswtDisplay();
            }
            disp.syncExec(new Runnable()
            {
                public void run()
                {

                    rect = iControl.getBounds();
                }
            });
        }
        catch (Exception e)
        {
            Logger.LOG(Logger.EJavaMMAPI,Logger.EInfo,"  " + e.toString());
        }
        return rect;
    }
    // abstract functions implemented by concrete class
    protected abstract void setDisplaySize(int aWidth, int aHeight);
    protected abstract void setDisplayFullScreen(final boolean aFullScreenMode);
    protected abstract void setDisplayLocation(int aX, int aY);
    protected abstract void setVisible(boolean aVisible);
    protected abstract int getDisplayHeight();
    protected abstract int getDisplayWidth();
    protected abstract int getDisplayX();
    protected abstract int getDisplayY();
    protected abstract int getSourceWidth();
    protected abstract int getSourceHeight();
    protected abstract void getBoundRect();
    public abstract void setWindowResources(VideoItem aVideoItem);
    protected abstract void setNativeHandle(int handle);
    public abstract void setContainerVisibilityToNative(final boolean active);
    // Native methods
    private native void _nativeMethodCallInUiThread(int nativeDisplayHandle , int placeholder);

}
